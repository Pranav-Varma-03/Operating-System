#include <iostream>
#include <bits/stdc++.h>
#include <atomic>
#include <pthread.h>
#include <time.h>
#include <cmath>
#include <fstream>
#include <unistd.h>
#include <chrono>
using namespace std;
using namespace chrono;
int num_threads,k,lam1,lam2;
double sum=0,max_wait=0;

int waiting[10000];
atomic<int>  bounded_cas_lock(0);
class ThreadData_op{
    public:
    vector<string> result;
    vector<double> waiting_time;
};

struct thread_data_ip {
    int thread_num; //thread number
    double t1;
    double t2; 
};

void entry_sec(int id){

    int exp_val = 0;
    int new_val = 1;
    bool key = false;

    waiting[id-1] = 1;

        while ((waiting[id-1] == 1) && (key == false))
        {   
            key = bounded_cas_lock.compare_exchange_strong( exp_val, new_val );
            // Doing Nothing
            exp_val = 0;
            new_val = 1;
        }

    waiting[id-1] = 0;
    return;
}

void exit_sec(int id){

    int j = (id + 1) % num_threads;

    while((j != id-1) && (waiting[j] == 0)){

        j = (j + 1) % num_threads;
    }

    if( j == id-1){
        bounded_cas_lock = 0;
    }else{
        waiting[j] = 0;
    }

    return;
}


void* testCS(void *Data){

    struct thread_data_ip *info;
    info = (struct thread_data_ip *)Data;
    ThreadData_op* t_data = new ThreadData_op();
    int id,t1,t2;
    id = info->thread_num;
    t1 = info->t1;
    t2 = info->t2;
    for(int i=0;i<k;i++){

    auto start= high_resolution_clock::now();
        
        time_t now = time(NULL);
        struct tm* timeinfo = localtime(&now);
        string reqEnterTime = to_string(timeinfo->tm_min) + ":" + to_string(timeinfo->tm_sec); 
        t_data->result.push_back(to_string(i+1) + "th CS Request at "+reqEnterTime+" by_thread "+ to_string(id));
        entry_sec(id); //ENTRY SECTION
        
    auto stop= high_resolution_clock::now();
    auto time_diff = duration_cast<microseconds>(stop-start);

    t_data->waiting_time.push_back((double)(time_diff.count()));

        time_t now2 = time(NULL);
        struct tm* timeinfo2 = localtime(&now2);
        string actEnterTime = to_string(timeinfo2->tm_min) + ":" + to_string(timeinfo2->tm_sec); 
        t_data->result.push_back(to_string(i+1) + "th CS Entry at "+actEnterTime+" by_thread "+to_string(id));
        usleep(t1*1000);
        exit_sec(id); //SLEEP Critical Section.
        time_t now3 = time(NULL);
        struct tm* timeinfo3 = localtime(&now3);
        string exitTime = to_string(timeinfo3->tm_min) + ":" + to_string(timeinfo3->tm_sec); 
        t_data->result.push_back(to_string(i+1) + "th CS Exit at "+exitTime+" by_thread "+to_string(id));
        usleep(t2*1000); //Simulation of Remainder Section.
    }   


return (void *)t_data;
}

int main(){

    ifstream fin("inp.txt");
    fin>>num_threads>>k>>lam1>>lam2;
    int rc; //to catch error at thread
    //Creating K number of Threads as per Input.
    pthread_t threads[num_threads];
    pthread_attr_t attr;
    //setting default attribute.
    pthread_attr_init(&attr);

    for(int i=0;i<10000;i++){
        waiting[i] = 0;
    }

    struct thread_data_ip array_th[num_threads];

    for(int i=0;i<num_threads;i++){
        array_th[i].thread_num = i+1;
        
        int seed = chrono::system_clock::now().time_since_epoch().count();
        default_random_engine generator(seed);
        exponential_distribution<double> t_1(1.0 / lam1);
        exponential_distribution<double> t_2(1.0 / lam2);
        double t1 = t_1(generator);
        double t2 = t_2(generator);

        array_th[i].t1 = t1;
        array_th[i].t2 = t2;
        }

    for(int i=0;i<num_threads;i++){
        rc = pthread_create(&threads[i],&attr,testCS,(void*)&array_th[i]);

        //Error : while creating threads.
        if(rc){
            printf("Error occured\n");
            exit(-1);
        }
    }

    ThreadData_op* t_data = NULL;
    ofstream fout("output_bounded_cas.txt");
    ThreadData_op array[num_threads];

    for(int i = 0; i < num_threads; i++ ) {

      rc = pthread_join(threads[i], (void**)&t_data);

      if (rc) {
         printf("Error:unable to join,\n");
         exit(-1);
        }
        array[i] = *t_data;
    }

    for(int i = 0; i < num_threads; i++ ) {

        for(int j=0;j<array[i].result.size();j++){
            fout<<array[i].result[j]<<endl;

            if(max_wait < array[i].waiting_time[j]){
                max_wait = array[i].waiting_time[j];
            }
            sum += array[i].waiting_time[j];
        }
    }

    // cout<<" Average wait time "<<(sum/(num_threads*k))/1000<<endl;
    // cout<<" Max wait time "<<(max_wait)/1000<<endl;

    return 0;
}
