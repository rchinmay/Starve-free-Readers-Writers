//Necessary Headers
#include<pthread.h>
#include<iostream>
#include<semaphore.h>
#include<assert.h>

//Initialize global variables and semaphores
//More description given in README.md

sem_t begin, finish, writer_wait;

bool writer_waiting=false;
int data=0;                 //Shared data (global variable)
int reader_enter=0, reader_exit=0;

//Reader Function

void *reader(void *pid){
  //***************************************
  //ENTRY SECTION
  //****************************************

  //Wait to execute entry section
  sem_wait(&begin);
  //Increment reader_enter count
  reader_enter++;
  //Signal begin semaphore and allow other processes to enter
  sem_post(&begin);

  //***************************************
  //CRITICAL SECTION
  //***************************************

  //Reads the Global Shared Data and prints it
  std::cout<<"Data read from ID:"<<*((int*)pid)<<" is: "<<data<<std::endl;

  //***************************************
  //EXIT SECTION
  //***************************************

  //Wait to execute exit section
  sem_wait(&finish);
  //Increment reader_exit count
  reader_exit++;
  //Check if no reader is in Critical section, and any writer is waiting
  if(writer_waiting && reader_enter==reader_exit){
    sem_post(&writer_wait); //Give preference to waiting writer
  }
  //Signal finish semaphore and allow other processes to enter exit section
  sem_post(&finish);
  //***************************************
  // Reader Function Complete
}

void *writer(void* pid){
  //***************************************
  //ENTRY SECTION
  //***************************************

  //Ensure no reader process is in entry section
  sem_wait(&begin);
  //Ensure no reader process is in exxit section
  sem_wait(&finish);
  //Ensure no reader process is in critical section
  if(reader_enter==reader_exit){
    //Writer can safely enter critical section by still maintaing control of begin semaphore
    sem_post(&finish);
  }
  else{
    //Writer has to wait till all the readers complete in FIFO queue
    writer_waiting=true;  //Update writer_waiting to true to indicate that a writer is waiting
    sem_post(&finish);    //Signal finish
    sem_wait(&writer_wait); //Wait till all the reader process finish.
    writer_waiting=false;   //When it gets chance to execute, set writer_waiting to false and proceed to critical section
  }

  //***************************************
  //CRITICAL SECTION
  //***************************************

  //Process data and print the updated value.
  data++;   //For simplicity, data is only incremented
  std::cout<<"Writer ID:"<<*((int*)pid)<<" wrote to data:"<<data<<std::endl;

  //***************************************
  //EXIT SECTION
  //***************************************

  //Signal begin semaphore to allow other reader or writer processes to enter entry section.
  sem_post(&begin);
}

int main(){
  int num_reader, num_writer;
  std::cout<<"Enter number of READERS"<<std::endl;
  std::cin>>num_reader;                               //No. of reader from input
  assert(num_reader>=0);                              //Assert number of readers is non-negative
  std::cout<<"Enter number of WRITERS"<<std::endl;
  std::cin>>num_writer;                               //No. of writers from input
  assert(num_writer>=0);                              //Assert number of writers is non-negative

  //***************************************
  //Semaphore Initialization
  //***************************************

  sem_init(&begin, 0,1);
  sem_init(&finish,0, 1);
  sem_init(&writer_wait,0, 0);

  //Array of threads initialized
  pthread_t read[num_reader], write[num_writer];

  //read_id and write_id stores reader and writer process IDs.
  int read_id[num_reader], write_id[num_writer];

  //***************************************
  //Initialize reader threads with reader function
  //***************************************

  for(int i=1; i<=num_reader; ++i){
    read_id[i-1]=i;
    pthread_create(&read[i-1], NULL, &reader, (void *)&read_id[i-1]);
  }
  //***************************************
  //Initialize Writer threads with writer function
  //***************************************
  for(int i=1; i<=num_writer; ++i){
    write_id[i-1]=i;
    pthread_create(&write[i-1], NULL, &writer, (void *)&write_id[i-1]);
  }

  //***************************************
  //Join all threads
  //***************************************

  for(int i=1; i<=num_reader; ++i){
      pthread_join(read[i-1], NULL);
  }
  for(int i=1; i<=num_writer; ++i){
    pthread_join(write[i-1], NULL);
  }

  //Destroy all the semaphores

  sem_destroy(&begin);
  sem_destroy(&finish);
  sem_destroy(&writer_wait);

  return 0;
}