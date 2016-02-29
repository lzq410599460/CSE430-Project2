#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>


int PID=0;
int num_children;
int p2k;
pthread_t pro_thread,con_thread;
pthread_mutex_t lock;
sem_t mysem;

void* producer(void* _)
{

printf("\nProducer\n");


while(1 && PID<10)
{

		//I just do some simulation about process
		PID=1+PID;
		num_children=PID*3;
		printf("PID:%d  Children process:%d\n",PID,num_children);
		
	if (num_children=6)  //Assume we regard 6 children process is fork bomb
	{
	pthread_mutex_unlock(&lock);
	p2k=num_children/3;
	//num=0;
	}
//sleep
}



}
void* consumer(void* _)
{printf("\nConsumer\n");
	int count=0;
	
	

	pthread_mutex_lock(&lock);

	
		printf("Process to kill is: %d\n",p2k);
		
	
	//pthread_mutex_unlock(&lock);
	



}

int main()
{
	pthread_mutex_init(&lock,NULL);pthread_mutex_lock(&lock);
	pthread_create(&pro_thread,NULL,producer,NULL);
	pthread_create(&con_thread,NULL,consumer,NULL);

	pthread_exit(NULL);

return 1;


}

