#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <bits/stdc++.h>
#include <cstdlib>
#include <fstream>

using namespace std;

#define N 20
#define M 5
#define w 1
#define x 1
#define z 1
#define THINKING 2
#define HUNGRY 1
#define EATING 0

ofstream filewrite;
ifstream fileread;

std::chrono::time_point<std::chrono::system_clock> start, endt;

//int* phil; //int phil[N];
//int* teamCount;  //    int teamCount[N/M];
//int* state;   //      int state[N];
//sem_t* S;// sem_t S[N];   //!--state semaphore
//sem_t* leader;// sem_t leader[N/M];

int phil[N];

int teamCount[N/M];
int state[N];

sem_t mutex1; //!--mutex
sem_t S[N];   //!--state semaphore

sem_t binder; //!--binding station semaphore
pthread_mutex_t lock1;
pthread_mutex_t console;

sem_t leader[N/M];
//&--Reader_writer-----------------------//
int submissions=0;
pthread_mutex_t rc_mutex;
sem_t db;
int rc =0;

void reader(int stf){
    //while(true){
        pthread_mutex_lock(&rc_mutex);
        rc=rc+1;
        if(rc==1)
        sem_wait(&db);
        pthread_mutex_unlock(&rc_mutex);

        sleep(z);

        endt = chrono::high_resolution_clock::now();
        double time_taken = chrono::duration_cast<chrono::nanoseconds>(endt - start).count();
        time_taken *= 1e-9;
        pthread_mutex_lock(&console);
        filewrite<<"Staff "<<stf<<" has started reading the entry book at time "<<(int)time_taken<<". No. of submission = "<<submissions<<endl;
        pthread_mutex_unlock(&console);


        pthread_mutex_lock(&rc_mutex);
        rc=rc-1;
        if(rc==0)
        sem_post(&db);
        pthread_mutex_unlock(&rc_mutex);
   // }
}

void writer(void){
    //while(true){
    sem_wait(&db);
    sleep(z);
    submissions++;
    sem_post(&db);
    // }
}





//&--Dining_Philosophers----------------//

void test(int phnum)
{
    //!--Test if any adjacent student is not using the printer
    bool edible=true;
    if (state[phnum-1]==HUNGRY){
        for(int i=1;i<=N;i++){
            if((i%4+1)==(phnum%4+1)&&state[i-1]==EATING){
                 edible=false;
            }
        }
    }

    //!--check if this student need to print anything
    if(state[phnum-1]!=HUNGRY)
        edible=false;

    if (edible) {

        // state that eating
        state[phnum-1] = EATING;
        sleep(w); //!Printing time

        sem_post(&S[phnum-1]);
    }
}

void take_fork(int phnum)
{

    sem_wait(&mutex1);

    state[phnum-1] = HUNGRY;

    //printf("Philosopher %d is Hungry\n", phnum);

    //!--start printing if no adjacent student is printing
    test(phnum);

    sem_post(&mutex1);

    sem_wait(&S[phnum-1]);

    sleep(1);
}

void put_fork(int phnum)
{
    sem_wait(&mutex1);

    state[phnum-1] = THINKING;

    //printf("Philosopher %d is thinking\n", phnum );

    //!--maintaining that the group mate priority at printing
    for(int i=1;i<=N;i++){
            if((i%4+1)==(phnum%4+1)&&((i>((phnum-1)/M)*M)&&i<=(((phnum-1)/M)*M+M)))
                test(i);
        }

    //!--calling other group members after that
    for(int i=1;i<=N;i++){
            if((i%4+1)==(phnum%4+1)&&((i<=((phnum-1)/M)*M)||i>(((phnum-1)/M)*M+M)))
                test(i);
        }

    sem_post(&mutex1);
}

void* student(void* num)
{
        int* i = (int*)num;

        srand(time(0));
        sleep(rand() % 10 + 1);//!Randomness in student arrival at printing stations

        //#--Time-----------------------------------------
         endt = chrono::high_resolution_clock::now();
        double time_taken = chrono::duration_cast<chrono::nanoseconds>(endt - start).count();
        time_taken *= 1e-9;
        //cout<<"Arrival time of "<<*i<<" is "<<time_taken<<endl;
        filewrite<<"Student "<<*i<<" has arrived at the print station at time "<<(int)time_taken<<endl;

        //------------------------------------------------

        take_fork(*i);

        sleep(0);

        put_fork(*i);

        endt = chrono::high_resolution_clock::now();
        time_taken = chrono::duration_cast<chrono::nanoseconds>(endt - start).count();
        time_taken *= 1e-9;
        pthread_mutex_lock(&console);
        filewrite<<"Student "<<*i<<" has finished printing at time "<<(int)time_taken<<endl;
        pthread_mutex_unlock(&console);


        pthread_mutex_lock(&lock1);//!--mutex is used to handle teamCount as it[s same position accessed by multiple treads

        int tmp=*i;
        teamCount[(tmp-1)/M]++;

        if(teamCount[(tmp-1)/M]==M){
            sem_post(&leader[(tmp-1)/M]);
        }
        pthread_mutex_unlock(&lock1);



        if(tmp%M==0){
           sem_wait(&leader[tmp-1]);//!--for all the member of a group to finish

           endt = chrono::high_resolution_clock::now();
           time_taken = chrono::duration_cast<chrono::nanoseconds>(endt - start).count();
           time_taken *= 1e-9;

           pthread_mutex_lock(&console);
           filewrite<<"Group "<<((tmp-1)/M+1)<<" has finished printing at time "<<(int)time_taken<<endl;
           pthread_mutex_unlock(&console);


           sem_wait(&binder);//!--entering binding station

           sleep(x);

           endt = chrono::high_resolution_clock::now();
           time_taken = chrono::duration_cast<chrono::nanoseconds>(endt - start).count();
           time_taken *= 1e-9;

           pthread_mutex_lock(&console);
           filewrite<<"Group "<<((tmp-1)/M+1)<<" has finished binding at time "<<(int)time_taken<<endl;
           pthread_mutex_unlock(&console);

           sem_post(&binder);

           writer();
        }
}

void* staff1f(void* num){
    int cnt1=0;
    while(true){
        reader(1);
        sleep(5);
        if(submissions==N/M)
            cnt1++;
        if(cnt1==2)
            break;

    }

}

void* staff2f(void* num){
    int cnt1=0;
    while(true){
        reader(2);
        sleep(5);
        if(submissions==N/M)
            cnt1++;
        if(cnt1==2)
            break;

    }
}

int main()
{
   // int n,m,X,Y,Z;
    filewrite.open("out.txt");
    start = chrono::high_resolution_clock::now();
    int i;
  /*  fileread.open("in.txt");
    fileread>>n;
    fileread>>m;
    fileread>>X;
    fileread>>Y;
    fileread>>Z;


 phil=new int[N];
 teamCount=new int[N/M];
 state=new int[N];
 S=new sem_t[N];
 leader=new sem_t[N/M];
*/



    pthread_t thread_id[N];
    pthread_t staff1;
    pthread_t staff2;

    for(int i=0;i<N;i++)
        state[i]=2;
    // initialize the semaphores
    sem_init(&mutex1, 0, 1);
    sem_init(&binder, 0, 2);
    sem_init(&db, 0, 1);

    for (i = 0; i < N; i++){
        sem_init(&S[i], 0, 0);
        phil[i]=i+1;
    }


    for (i = 0; i < N/M; i++){
        sem_init(&leader[i], 0, 0);
        teamCount[i]=0;
    }


    for (i = 0; i < N; i++) {
        //cout<<rand()<<endl;
        //!--creating students
        sleep(1);
        pthread_create(&thread_id[i], NULL, student, &phil[i]);
        //printf("Philosopher %d is thinking\n", i + 1);
    }
    sleep(1);
    pthread_create(&staff1, NULL, staff1f, &phil[i]);
    sleep(1);
    pthread_create(&staff2, NULL, staff2f, &phil[i]);


    for (i = 0; i < N; i++)
        pthread_join(thread_id[i], NULL);

    pthread_join(staff1, NULL);
    pthread_join(staff2, NULL);

    filewrite.close();

    return 0;
}
