#include <iostream>
#include <vector>
#include <pthread.h>
#include <cmath>
#include<ctime>
#include<chrono>
#include <fstream>
using namespace std;
using namespace chrono;

int validsudoko=1;

//below values are later updated when we read data from input FILE.
int order = 0; //order of input Matrix.
int per_thread = 1; //number of integers per thread
int num_threads = 1;//Input K.

//checks is element in arrays(row/col/grid) have repeating elements or not
int exist(int value,vector<int> arr){

    int vbool = 1;
    int count = 0;

    for(int i=0;i<arr.size();i++){

        if(value == arr[i]){
            count ++;
        }

    }

    if(count>1){
        vbool = 0;
    }

    return vbool;
}

//This stores status (valid / invalid) of each thread.
class ThreadData_op{
public:
vector<string> result;
};

// individual thread data is being sent.
struct thread_data_ip {
    int thread_num; //thread number
    vector<pair<string,vector<int>>> data; //

};

// Function that each Thread executes with its correspoding Data
void* t_data(void *Data){
    
    struct thread_data_ip *info;
    info = (struct thread_data_ip *)Data;
    
    ThreadData_op* t_data = new ThreadData_op();
    int bvalid = 0;
    

    for(int i=0;i<info->data.size();i++){

        for(int j = 0;j<order;j++){
            // row[i][j] element exist in row[i]. then vrow = 0;
            bvalid = exist(info->data[i].second[j],info->data[i].second);

            if(bvalid == 0){
                break;
            }
        }

        // this tells -> computed Vector is a ROW.
        if(info->data[i].first[0] == 'r'){

            info->data[i].first.erase(0, 1);

            if(bvalid == 0){
            t_data->result.push_back("checks row "+ info->data[i].first +" and is Invalid");
                validsudoko=0;
            }else{
            t_data->result.push_back("checks row "+info->data[i].first+" and is valid");
            }
        }
        // this tells -> computed Vector is a COL.
        else if(info->data[i].first[0] == 'c'){

            info->data[i].first.erase(0, 1);

            if(bvalid == 0){
            // cout<<" row loop broken"<<endl;
            t_data->result.push_back("checks col "+ info->data[i].first +" and is Invalid");
                validsudoko=0;
            }else{
            t_data->result.push_back("checks col "+info->data[i].first+" and is valid");
            }
        }
         // this tells -> computed Vector is a Grid.
        else if (info->data[i].first[0] == 'g'){

            info->data[i].first.erase(0, 1);

            if(bvalid == 0){
            // cout<<" row loop broken"<<endl;
            t_data->result.push_back("checks grid "+ info->data[i].first +" and is Invalid");
                validsudoko=0;
            }else{
            t_data->result.push_back("checks grid "+info->data[i].first+" and is valid");
            }
        }


    }
return (void *)t_data;
}

int main(){

    ifstream fin("input.txt");
    fin>>num_threads>>order;

    //Creating K number of Threads as per Input.
    pthread_t threads[num_threads];
    pthread_attr_t attr;
    //setting default attribute.
    pthread_attr_init(&attr);

    int rc;// to catch error while creation of Thread.
    int i; //used in loops
    int j=0; //used in loops.

    // takes number of threads and Sudoku Grid.
    int num;
    vector<vector<int>> sudoko;

    // Creates Sudoku Grid from input.

    for(int i = 0;i<order;i++){
        vector<int> v1;

        for(int j=0;j<order;j++){
            fin>>num;
            v1.push_back(num);
        }

        sudoko.push_back(v1);


    }

        // CREATING ROWS VEC FROM SUDOKO

    vector<pair<string,vector<int>>> Rows;

    for(int i = 0;i<order;i++){
        Rows.push_back(make_pair(("r"+ to_string(i+1)),sudoko[i]));
    }

  
    // CREATING ROWS VEC FROM SUDOKO

    vector<pair<string,vector<int>>> Cols;

    for(int i=0;i<order;i++){
        // pair<string,vector<int>> v2;
        vector<int> v2;
        for(int j=0;j<order;j++){
            // v2.first = "c";
            v2.push_back(sudoko[j][i]);
            // v2.make_pair("c",sudoko[j][i]);
        }

        Cols.push_back(make_pair(("c" + to_string(i+1)),v2));
    }

    // CREATING GRIDS VEC FROM SUDOKO.
    vector<pair<string,vector<int>>> Grids;
    int grid_count=0;
    for(int i=0;i<order;i = i + sqrt(order)){

        for(int j=0;j<order; j=j+ sqrt(order)){
            vector<int> v3;

            for(int k=0;k<sqrt(order);k++){

                for(int l = 0; l<sqrt(order);l++){
                    v3.push_back(sudoko[k+i][l+j]);
                }
            }
            grid_count ++;
            Grids.push_back(make_pair(("g" + to_string(grid_count)),v3));
        }

    }
    vector<pair<string,vector<int>>> inpData;
    for(int i = 0;i<order;i++){
        inpData.push_back(Rows[i]);
    }
    for(int i = 0;i<order;i++){
        inpData.push_back(Cols[i]);
    }
    for(int i = 0;i<order;i++){
        inpData.push_back(Grids[i]);
    }

    Grids.clear();
    Cols.clear();
    Rows.clear();
    sudoko.clear();


    //each element in this array stores. Thread Number and its Data Set.
    struct thread_data_ip array_data[num_threads];

    // limiting max number of threads to 3*order.
    if(num_threads > 3*order){
            if((order*3)%27 == 0){
            int countk = 0;
            per_thread = (order*3)/27;
            for(int i=0;i<27;i++){
            array_data[i].thread_num = i+1;
            
                for(int j=0;j<per_thread;j++){
                    array_data[i].data.push_back(inpData[0]);
                    inpData.erase(inpData.begin());
                    // countk++;
                }
            }
        }
        //Distributing numbers to threads: othercase
        else{

            per_thread = (order*3)/27;
            for(int i=0;i<27;i++){
                array_data[i].thread_num = i+1;
                for(int j=0;j<per_thread;j++){
                    array_data[i].data.push_back(inpData[0]);
                    inpData.erase(inpData.begin());
                }
            }

            //Adding to the Last Thread.
            for(int j=0;j<(order*3)%(27);j++){
                    array_data[27-1].data.push_back(inpData[0]);
                    inpData.erase(inpData.begin());
                }
        }
    }else{
        if((order*3)%num_threads == 0){
        int countk = 0;
        per_thread = (order*3)/num_threads;
        for(int i=0;i<num_threads;i++){
        array_data[i].thread_num = i+1;
        
            for(int j=0;j<per_thread;j++){
                array_data[i].data.push_back(inpData[0]);
                inpData.erase(inpData.begin());
            }
        }
    }
    //Distributing numbers to threads: othercase
    else{

        per_thread = (order*3)/(num_threads);
        //Adding to first k-1 threads.
        for(int i=0;i<num_threads;i++){
            array_data[i].thread_num = i+1;
            for(int j=0;j<per_thread;j++){
                array_data[i].data.push_back(inpData[0]);
                inpData.erase(inpData.begin());
            }
        }

        //Adding to the Last Thread.
        // array_data[num_threads-1].thread_num = num_threads;
        for(int j=0;j<(order*3)%(num_threads);j++){
                array_data[num_threads-1].data.push_back(inpData[0]);
                inpData.erase(inpData.begin());
            }
    }
    }


    srand(time(NULL));
    auto started = chrono::high_resolution_clock::now(); /* Start time */
    int z1=0;
    for(int z2=0;z2<num_threads;z2++){
        rc = pthread_create(&threads[z1],&attr,t_data,(void*)&array_data[z1]);
        z1++;
        //Error : while creating threads.
        if(rc){
            printf("Error occured\n");
            exit(-1);
        }
    }

  
    //this stores, output from threads with perfect numbers as their values.
    ThreadData_op* t_data = NULL;
    ofstream fout("output.txt");
    ofstream newne("time.txt");

    ThreadData_op array[num_threads];

    for( i = 0; i < num_threads; i++ ) {

        //this join -> we wait till all threads to complete..
      rc = pthread_join(threads[i], (void**)&t_data);

      if (rc) {
         printf("Error:unable to join,\n");
         exit(-1);
        }
        array[i] = *t_data;
    }
     // // Recording the end clock tick.
    // end = clock();
    auto done = chrono::high_resolution_clock::now(); /* End time */
    auto time = chrono::duration_cast<chrono::microseconds>(done-started);

    // // Calculating total time taken by the program.
    newne<<(double)(time.count()) << endl;
    

    //Writing onto the Output File.
    for( i = 0; i < num_threads; i++ ) {

        for(int j=0;j<array[i].result.size();j++){
            fout<<"Thread "<< (i+1)<<" "<<array[i].result[j]<<endl;
        }
    }
    if(validsudoko == 1){
        fout<<"Sudoku is valid."<<endl;
    }else{
        fout<<"Sudoku is invalid."<<endl;
    }
    fout<<"The total time taken is "<<(double)(time.count())<<" microseconds.";
    
    return 0;
}
