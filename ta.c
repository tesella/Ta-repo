#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<semaphore.h>
#include<unistd.h>
#include<sys/time.h>

pthread_t taid;
pthread_t* sid;
sem_t tafree;            //1- it can nap
sem_t student;		// 1- atmost 1 can go to ta
sem_t chairs[3];	     //  1- atmost 1 can occupy that chair
pthread_mutex_t chairval= PTHREAD_MUTEX_INITIALIZER;
int chairnum =0 ;         //no of chairs filled
int ichair = 0;    	// current chair index
        
void* tafunc(void* arg)
{
	int randtime;
	struct timeval t1;
	gettimeofday(&t1,NULL);
	long int min =((t1.tv_sec)+(t1.tv_usec)/1000000)/60;
	long int minchange; 
	while((minchange=(((t1.tv_sec)+((t1.tv_usec)/1000000))/60)!=min+1))
	{	
		gettimeofday(&t1,NULL);
		minchange=((t1.tv_sec)+((t1.tv_usec)/1000000))/60;
		
		sem_wait(&tafree);           //initially sleeping

			while(1)
			{
			
				

pthread_mutex_lock(&chairval);
				if(chairnum == 0)       //no student needs help , keeps sleeping
				{
					printf("\tTA is napping\n");
					pthread_mutex_unlock(&chairval);
					break;
				}

	 			sem_post(&chairs[ichair]);     //ta get the student
				chairnum--;
				printf("\tStudent seeking help.\n\t Remaining Chairs %d\n", 3-chairnum);
				ichair = (ichair + 1) % 3;
				pthread_mutex_unlock(&chairval);
			

				printf("\t TA is currently helping the student.\n");
				sleep(5);
				sem_post(&student);
				usleep(1000);
			}
		}
	}


void *stud(void *sid) 
{
	int randtime;
	struct timeval t1;
	gettimeofday(&t1,NULL);
	long int min =((t1.tv_sec)+(t1.tv_usec)/1000000)/60;
	long int minchange; 
	
	int rtime;
	int sno = (int)sid;
	


while((minchange=(((t1.tv_sec)+((t1.tv_usec)/1000000))/60)!=min+1))
	{	
		gettimeofday(&t1,NULL);
		minchange=((t1.tv_sec)+((t1.tv_usec)/1000000))/60;
		
		printf("\t%dno Student is working.\n", sno);
		rtime = rand()%30+1;
		sleep(rtime);		//Sleep for a bit

		printf("\t%dno Student needs help \n", sno);
		
		pthread_mutex_lock(&chairval);
		int c = chairnum;
		pthread_mutex_unlock(&chairval);

		if(c < 3)		//chairs not full
		{
			if(c == 0)		//wake ta
				sem_post(&tafree);
			else
				printf("\t%dno Student is waiting\n", sno);

			pthread_mutex_lock(&chairval);
			int index = (ichair + chairnum) % 3;
			chairnum++;
			printf("\tStudent arriving.Chairs Remaining: %d\n", 3 - chairnum);
			pthread_mutex_unlock(&chairval);
			

			sem_wait(&chairs[ichair]);		//Student completes
			printf(" \t%d Student is getting help \n",sno);
			sem_wait(&student);		//Student waits to go next.
			printf("\t%d Student resumes work\n",sno);
		}
		else 
			printf("\t%d Student will return at another time. \n", sno);
			

//chairs are full
	}
	exit(0);
}


int main(int argc,char* argv[])
{
	int snum=5;					//number of students, default 5
						
	if(argc>=2)
	{	
		snum = atoi(argv[1]);          //cmd value
	}
	
	sid = (pthread_t*)malloc(sizeof(pthread_t)*snum);  //student threads
	
	if(pthread_create(&taid,NULL,tafunc,NULL))
		{
			printf("\tThread isnt created\n");
			exit(-1);
		}
		else{
			printf("\tTA thread created\n");
			}         //ta thread
	
	for(int i=0;i<snum;i++)
	{
		if(pthread_create(&sid[i],NULL,stud,(void*)((int)i+1)))
		{
			printf("\tThread isnt created\n");
			exit(-1);
		}
		else{
			printf("\t%d student thread created\n",i+1);
			}
	}
	
	pthread_join(taid,NULL);

	for(int i=0;i<snum-1;i++)
	{
		pthread_join(sid[i],NULL);
	}
	
	sem_init(&tafree,0,0);     //at start ta sleeping
	sem_init(&student,0,0);
	
	for(int i=0;i<3;i++)
	{
		sem_init(&chairs[i],0,0);
	}
	
	free(sid);
	
	exit(0);
return 0;
}
