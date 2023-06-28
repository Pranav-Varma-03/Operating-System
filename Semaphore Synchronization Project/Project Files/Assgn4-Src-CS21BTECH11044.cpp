#include <iostream>
#include <bits/stdc++.h>
#include <atomic>
#include <pthread.h>
#include <time.h>
#include <cmath>
#include <fstream>
#include <unistd.h>
#include <chrono>
#include <semaphore.h>
using namespace std;
using namespace chrono;

int ps_threads,cr_threads,lam_ps,lam_cr,k;
int car_arr[1000000];
int key=0;
sem_t car;
sem_t arr;

class ThreadData_op{
    public:
    double pass_time; // stores, total time spent in museum by each passenger.
    double car_time; //this records, individual passenger total car ride time.
};

struct thread_data_ip {
    int ps_id; //passenger number  
};


void* Park(void *Data){

        int seed = chrono::system_clock::now().time_since_epoch().count();
        default_random_engine generator(seed);
        exponential_distribution<double> t_1(1.0 / lam_ps);
        exponential_distribution<double> t_2(1.0 / lam_cr);

        double t1=0;
        double t2=0;

        double car_ride=0; // temp stores tot car ride time by passenger with id.

    ofstream fout;
    fout.open("output.txt", ios_base::app);

    struct thread_data_ip *info;
    info = (struct thread_data_ip *)Data;
    ThreadData_op* t_data = new ThreadData_op();

    int id,car_id;
    id = info->ps_id; // id: thread number => passenger number.

    // clock tick start when Passenger enters PARK.
    auto start_pass = high_resolution_clock::now();

    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    // Records System time => Entry time of passenger.
    string EnterTime = to_string(timeinfo->tm_min) + ":" + to_string(timeinfo->tm_sec); 
    fout<<"Passenger " << to_string(id) << " enters the Museum at " << EnterTime<<endl;


    for(int i=0;i<k;i++){

        t1 = t_1(generator);
        t2 = t_2(generator);

    time_t now2 = time(NULL);
    struct tm* timeinfo2 = localtime(&now2);
    // Records System time => Request time of passenger for car.
    string reqTime = to_string(timeinfo2->tm_min) + ":" + to_string(timeinfo2->tm_sec); 
    fout<<"Passenger " << to_string(id) << " made request at " << reqTime<<endl;


    // value(car) : number of cars available at the moment.
    // if available then would decrease value by 1 and Enter CS. else will spin in busy wait till some other passenger thread invokes signal {sem_post()}
    sem_wait(&car);

    //this wait(arr) is Mutex Lock used to update the Car Array without other thread interruptions.
    //car_arr : stores whether car is availabe or not using the index as car id.
    sem_wait(&arr);
        car_arr[key % cr_threads] = 0;// if car is available, then car index is stored in car_id and made unavailable.
        car_id = key % cr_threads;  
        key++;
    sem_post(&arr);
    
    fout<<"Car " << to_string(car_id + 1) << " accepts Passenger " + to_string(id) << " request." <<endl;

    auto start_car = high_resolution_clock::now();

    time_t now3 = time(NULL);
    struct tm* timeinfo3 = localtime(&now3);
    string startTime = to_string(timeinfo3->tm_min) + ":" + to_string(timeinfo3->tm_sec); 

    fout<<"Passenger " << to_string(id) << " starts riding at "<< startTime << endl;
    
    fout<<"Car " << to_string(car_id + 1) << " is Riding Passenger " << to_string(id)<<endl;

    //CRITICAL SECTION. 
    // LAM_C
    usleep(t2*1000);

    time_t now4 = time(NULL);
    struct tm* timeinfo4 = localtime(&now4);
    string endTime = to_string(timeinfo4->tm_min) + ":" + to_string(timeinfo4->tm_sec);

    auto stop_car = high_resolution_clock::now(); 
    auto time_diff_car = duration_cast<microseconds>(stop_car - start_car);

    car_ride += (double)(time_diff_car.count());

    fout<<"Car " << to_string(car_id + 1) << " has finished Passenger " << to_string(id) << "'s tour."<<endl;

    fout<<"Passenger " << to_string(id) << " finished riding at "<< endTime <<endl;
    
    sem_wait(&arr);
        car_arr[car_id ] = 1; // after the trip, car is available hence, its made available.
    sem_post(&arr);
    
    sem_post(&car);
    //REMAINDER SECTION.
    //LAM_P
    usleep(t1*1000);

    }

    auto stop_pass = high_resolution_clock::now();
    auto time_diff_pass = duration_cast<microseconds>(stop_pass - start_pass);

    t_data->pass_time = (double)(time_diff_pass.count());
    t_data->car_time = car_ride;


    time_t now5 = time(NULL);
    struct tm* timeinfo5 = localtime(&now5);
    string exitTime = to_string(timeinfo5->tm_min) + ":" + to_string(timeinfo5->tm_sec); 

    fout<<"Passenger " << to_string(id) << " Exits the museum at "<< exitTime <<endl;
 
    fout.close();
    return (void *)t_data;
}

int main(){

    double sum_pass=0,sum_car=0;

    ifstream fin("inp-params.txt");
    fin>>ps_threads>>cr_threads>>lam_ps>>lam_cr>>k;

    sem_init(&car,0,cr_threads);
    sem_init(&arr,0,1); //MUTEX lock, used for locking Car Array.

    for(int i=0;i<cr_threads;i++){
        car_arr[i] = 1;
    }

    int rc; //to catch error at thread

    //Creating ps_threads number of Threads as per Input.
    pthread_t threads[ps_threads];
    pthread_attr_t attr;
    //setting default attribute.
    pthread_attr_init(&attr);

    struct thread_data_ip array_th[ps_threads];

    for(int i=0;i<ps_threads;i++){
        array_th[i].ps_id = i+1;
    }
// Thread Creation.
    for(int i=0;i<ps_threads;i++){
        rc = pthread_create(&threads[i],&attr,Park,(void*)&array_th[i]);

        //Error : while creating threads.
        if(rc){
            printf("Error occured\n");
            exit(-1);
        }
    }
    ThreadData_op* t_data = NULL;
    ThreadData_op array[ps_threads];


//Joining of threads.
    for(int i = 0; i < ps_threads; i++ ) {

        rc = pthread_join(threads[i], (void**)&t_data);
        if (rc){
            printf("Error:unable to join,\n");
            exit(-1);
            }
        array[i] = *t_data; //stores output of thread data.

    }


//Calculations for graph plotting from received output in Threads.
    for(int i = 0; i < ps_threads; i++ ) {

        sum_pass += array[i].pass_time;
        sum_car += array[i].car_time;
    }

    cout<<"Average passenger time in milli secs :"<<(sum_pass)/(ps_threads*1000)<<endl;
    cout<<"Average car ride in milli secs :"<<(sum_car)/(cr_threads*1000)<<endl;

    // remove("output.txt");
    return 0;
}