#include <iostream>
#include <vector>
#include <pthread.h>
#include <cmath>
#include<ctime>
#include<chrono>
#include <fstream>
#include <omp.h>
using namespace std;
using namespace chrono;

int order = 0;
int num_threads = 0;
int validsudoko = 1;

//checks is element in arrays(row/col/grid) have repeating elements or not
int exist(int value,vector<int> arr){

    int vbool = 1;
    int count = 0;

    for(int i=0;i<arr.size();i++){

        if(value == arr[i]){
            count ++;
            // if(count > 1){
            //     cout<<arr[i]<<endl;
            // }
        }

    }

    if(count>1){
        vbool = 0;
    }

    return vbool;
}


int main(){
    // takes number of threads and Sudoku Grid.
    ifstream fin("input.txt");
    fin>>num_threads>>order;
    
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
    // Rows = sudoko;
    for(int i = 0;i<order;i++){
        Rows.push_back(make_pair("r" + to_string(i+1),sudoko[i]));
    }

    // CREATING COLS VEC FROM SUDOKO
    vector<pair<string,vector<int>>> Cols;

    for(int i=0;i<order;i++){
        // pair<string,vector<int>> v2;
        vector<int> v2;
        for(int j=0;j<order;j++){
            // v2.first = "c";
            v2.push_back(sudoko[j][i]);
            // v2.make_pair("c",sudoko[j][i]);
        }

        Cols.push_back(make_pair("c" + to_string(i+1),v2));
    }

    // CREATING SUB-GRIDS VEC FROM SUDOKO.
    vector<pair<string,vector<int>>> Grids;
    int grid_count = 0;
    for(int i=0;i<order;i = i + sqrt(order)){

        for(int j=0;j<order; j=j+ sqrt(order)){
            vector<int> v3;

            for(int k=0;k<sqrt(order);k++){

                for(int l = 0; l<sqrt(order);l++){
                    v3.push_back(sudoko[k+i][l+j]);
                }
            }
            grid_count++;
            Grids.push_back(make_pair("g" + to_string(grid_count),v3));
        }

    }

    //This adds all above generated Row, col and Sub grids into Input Data Vector <inpData>.
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

    //Creating Open MP threads
    omp_set_num_threads(num_threads);

    //This stores every row/col/grid validity and prints onto output File.
    vector<string> result(3*order);
    srand(time(NULL));
    auto started = chrono::high_resolution_clock::now(); /* Start time */

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        int per_thread = inpData.size()/nthreads;
        int bvalid = 0;
        int start = id*per_thread;
        int end = (id + 1)*per_thread;

        if(id == nthreads - 1){
            end = inpData.size();
        }

        for(int i = start;i<end;i++){

            for(int j = 0;j<order;j++){
                // row[i][j] element exist in row[i]. then vrow = 0;
                bvalid = exist(inpData[i].second[j],inpData[i].second);

                if(bvalid == 0){
                    break;
                }
            }
            
            // this tells -> computed Vector is a ROW.
             if(inpData[i].first[0] == 'r'){

                inpData[i].first.erase(0, 1);

                if(bvalid == 0){
                    result[i] = "THREAD "+to_string(id+1) +" : checks row "+ inpData[i].first +" and is Invalid";
                    validsudoko=0;
                }else{
                    result[i] = "THREAD "+to_string(id+1) +" : checks row "+inpData[i].first+" and is valid";
                    
                }
            }else if(inpData[i].first[0] == 'c'){  // this tells -> computed Vector is a COL.

                inpData[i].first.erase(0, 1);

                if(bvalid == 0){
                // cout<<" row loop broken"<<endl;
                    result[i] = "THREAD "+to_string(id+1) +" : checks col "+ inpData[i].first +" and is Invalid";
                    validsudoko=0;
                }else{
                     result[i] = "THREAD "+to_string(id+1) +" : checks col "+inpData[i].first+" and is valid";
                }
            }else if (inpData[i].first[0] == 'g'){  // this tells -> computed Vector is a Grid.

                inpData[i].first.erase(0, 1);

                if(bvalid == 0){
                // cout<<" row loop broken"<<endl;
                     result[i] = "THREAD "+to_string(id+1) +" : checks grid "+ inpData[i].first +" and is Invalid";
                     validsudoko=0;
                }else{
                    result[i] = "THREAD "+to_string(id+1) +" : checks grid "+inpData[i].first+" and is valid";
                }
            }

        

        }
    }

    auto done = chrono::high_resolution_clock::now(); /* End time */

    auto time = chrono::duration_cast<chrono::microseconds>(done-started);

    ofstream fout("output.txt");
    
    ofstream newnw("time.txt");
    
    newnw << (double)(time.count()) << endl;


    for(int i =0;i<result.size();i++){
        fout<<result[i]<<endl;
    }
    if(validsudoko == 1){
        fout<<"Sudoku is valid."<<endl;
    }else{
        fout<<"Sudoku is invalid."<<endl;
    }
    fout<<"The total time taken is "<<(double)(time.count())<<" microseconds.";

return 0;
}
