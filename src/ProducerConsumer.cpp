//============================================================================
// Name        : ProducerConsumer.cpp
// Author      : Sriee Gowthem Raaj
// Version     :
// Copyright   : Your copyright notice
// Description : Producer Consumer Problem in C++, Ansi-style
//============================================================================
#include<iostream>
#include<cstdlib>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<fstream>
#include<cmath>
#include<ctime>

using namespace std;

//Declaring the semaphores
sem_t full,empty,bufferLock,wr;

//Declaring global variables
static int p,q,inPointer,outPointer,itemProduced;

//Flag to make consumers exit correctly
bool flag_new=false;

//Buffer to store the items
typedef struct{
	int producerNumber;
	int randomNumber;
}producerData;

producerData* buffer;

/**************************************************************************************************
 * Member Function to calculate Prime Number
 * ************************************************************************************************
 */
bool isPrime(int value){
	bool flag = true;
	int temp,upp_val,i;
	double sqrt_num,ceil_value;
	temp =value;
	if(temp==1 || temp==0){
		flag = false;
		return flag;
	}else{
		sqrt_num = sqrt(temp);

		ceil_value = ceil(sqrt_num);

		upp_val = (int) ceil_value;

		for(i=2;i<=upp_val;i++){
			if(temp%i == 0){
				flag = false;
				return flag;
			}
		}
	}
	return flag;
}

/**************************************************************************************************
 * Producer Thread
 *
 * Each Producer Thread will iterate for q times. inPointer variable is used to traverse through the
 * buffer. Random values are created in the range 1 to 10000. As inPointer increments, corresponding
 * produces values will be stored in the buffer.
 *
 **************************************************************************************************
*/
void *producer(void *data){

	//Declaring Variables to be used by Producer Thread
	ofstream ifile;
	try{
		for(int i=1;i<=q;i++){
			sem_wait(&full);
				sem_wait(&bufferLock);
				buffer[inPointer].producerNumber = (long)data;
				buffer[inPointer].randomNumber = (rand()%10000)+1;
				ifile.open("Input-numbers.txt",ios::out | ios::app);
				if (ifile.is_open()){
					ifile << buffer[inPointer].producerNumber <<" " <<buffer[inPointer].randomNumber <<endl;
				}
				ifile.close();
				inPointer = (inPointer +1) % p;
				usleep(1000);			//Sleeping for 1 millisecond
				sem_post(&bufferLock);
			sem_post(&empty);
		}
	}catch(exception& e){
		cout<<"Exception at Producer Thread " <<endl;
	}
}

/**************************************************************************************************
 * Consumer Thread
 *
 * Each thread will look for the number of items consumed. If there are still items to be consumed
 * from the buffer they will extracted from the buffer. The extracted random number are checked whether
 * they are prime or not. Prime and non prime values are stored separately in text files.
 *
 * Flags are used to check whether all the items are consumed. After items are consumed further
 * consumers will break from the loop.
 **************************************************************************************************
*/

void *consumer(void *arg){
	int consumerNumber;
	int producerNumber,producedData;
	ofstream prime_file,non_prime_file;
	try{

		while(itemProduced>0){
		sem_wait(&empty);
			sem_wait(&bufferLock);

				if(flag_new)
				{
					sem_post(&empty);
					sem_post(&bufferLock);
					break;
				}
				consumerNumber=(long)arg;
				consumerNumber = 100000+consumerNumber;

				//To handle sig abort error
				if(!(buffer[outPointer].producerNumber < 0) && !(buffer[outPointer].randomNumber < 0)){
					producerNumber = buffer[outPointer].producerNumber;
					producedData = buffer[outPointer].randomNumber;
				}else{
					break;
				}

				if(isPrime(producedData)){
					prime_file.open("Prime.txt",ios::out | ios::app);
					if(prime_file.is_open()){
						sem_wait(&wr);
						prime_file <<producerNumber <<" " <<consumerNumber <<" " <<producedData <<endl;
						sem_post(&wr);
					}
					prime_file.close();
				}else {
					non_prime_file.open("NonPrime.txt", ios::out | ios::app);
					if(non_prime_file.is_open()){
						sem_wait(&wr);
						non_prime_file <<producerNumber <<" " <<consumerNumber<<" " <<producedData <<endl;
						sem_post(&wr);
					}
					non_prime_file.close();
				}

			outPointer = (outPointer + 1)%p;
			itemProduced--;
			if(itemProduced == 0)
			{
				flag_new = true;
				sem_post(&empty);
			}

			sem_post(&bufferLock);
		sem_post(&full);
		}

	}catch(exception& e){
		cout <<"Exception at Consumer Thread " <<endl;
	}
}

/**************************************************************************************************
 * Main Program
 *
 * Parameters required for Producer Consumer problem are accepted via command line arguments. Entry
 * check validation is done initially. When valid inputs are given then semaphore values and buffer
 * size are initialized. 'm' Producer Threads and 'n' Consumer Threads are produced.
 **************************************************************************************************
*/
int main(int argc, char *args[]){
	//The values required for main thread
	int m,n,initialValueCheck,threadCheck;

	//Seeding for Random Number Generator
	srand(time(NULL));

	//Entry check validation
	if (argc < 2){
		cout << "Argument count = " <<argc <<endl;
		cout << "No arguments entered\n";
		exit(0);
	}
	else if(argc < 5){
		cout << "Argument count = " <<argc <<endl;
		cout << "Please provide the values for m,n,p & q\n";
		exit(0);
	}
	else{
		m = atoi (args[1]);
		n = atoi (args[2]);
		p = atoi (args[3]);
		q = atoi (args[4]);

		if ( m==0 || n==0 || p==0 || q ==0){
			cout <<"Zero value has been entered, Enter non zero values\n";
			exit(0);
		}
		else if(m<0 || n<0 || p<0 || q<0){
			cout <<"Negative values not allowed\n";
			exit(0);
		}
		else{
			cout << "The number of Producers = " << m <<endl;
			cout << "The number of Consumers = " << n <<endl;
			cout << "Maximum buffer limit = " << p <<endl;
			cout << "Number of random numbers a Producer can create = " << q <<endl;
		}
	}

	//Initializing semaphore
	initialValueCheck = sem_init(&empty,0,0);
		if(initialValueCheck!=0) cout <<"Empty semaphore initialization failed" <<endl;

	initialValueCheck=sem_init(&full,0,m);
		if(initialValueCheck!=0) cout <<"Full semaphore initialization failed" <<endl;

	initialValueCheck=sem_init(&wr,0,1);
		if(initialValueCheck!=0) cout <<"Semaphore used for Consumer initialization failed" <<endl;

	initialValueCheck=sem_init(&bufferLock,0,1);
		if(initialValueCheck!=0) cout <<"Semaphore used for Buffer Lock initialization failed" <<endl;

	//Total Items produced
	itemProduced = m*q;

	//Dynamically initializing the size of the buffer to p elements
	buffer = new producerData[p];

	//Declaring Producer & Consumer Thread
	pthread_t producerThread[m];
	pthread_t consumerThread[n];

	//Creating m Producer Threads
	try{
	for(int i=0;i<m;i++){
		threadCheck = pthread_create(&producerThread[i],NULL,producer,(void *)(i+1));
		if(threadCheck)
			cout << "Producer Thread " <<i <<" not created properly" <<endl;
	}
	}catch(exception& e){ //Reaches this section when we are creating threads greater than the Thread stack
		cout<<"Exception occured in Main - Producer Thread section" <<endl;
	}

	//Creating n Consumer Threads
	try{
	for(int j=0;j<n;j++){
		threadCheck = pthread_create(&consumerThread[j],NULL,consumer,(void *)(j+1));
		if(threadCheck)
			cout << "Consumer Thread " <<j <<" not created properly";
	}
	}catch(exception& e){//Reaches this section when we are creating threads greater than the Thread stack
		cout<<"Exception occured in Main - Consumer Thread section" <<endl;
	}

	try{
	//Joining Producer Threads
	for(int i=0;i<m;i++)
		pthread_join(producerThread[i],NULL);

	//Joining Consumer Threads
	for(int k=0;k<n;k++)
		pthread_join(consumerThread[k],NULL);

	}catch(exception& e){
		cout<<"Exception occured in Joining Thread" <<endl;
	}

	//Destroying semaphores used by the program
	sem_destroy(&full);
	sem_destroy(&empty);
	sem_destroy(&wr);
	sem_destroy(&bufferLock);

	cout <<"Reached End of file successfully!!!" <<endl;
	return 0;
}



