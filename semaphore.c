/*
 * CSE234: Computer Architecture and Operating Systems
 * Assignment 3 : Multiple reader writer with synchronization
 * Navya Aggarwal (2018349)
 * November 2, 2019
 */

#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdbool.h>

sem_t mutex,wrt;
int front = -1,rear = -1,rcount = 0, size = 20,count = 0;
int queue[20];

void *reader_func(void *arg);
void *writer_func(void *arg);

void Enqueue(int val)
{
    if ((rear+1)%size == front)
    { 
        printf("Error : Queue overflow.\n"); 
        return; 
    }
    else
    {
        rear = (rear+1)%size;
        queue[rear] = val;
        count+=1;
        if (front == -1)
            front= 0;
    }
} 

void Dequeue()
{
    if (front == -1)
    { 
        printf("Error : Queue underflow.\n"); 
        return; 
    }
    else
    {
        printf("\nDequeued Element - %d\n",queue[front]);
        count-=1;
        if (front == rear)
        {
            rear = front = -1;
        }
        else
            front = (front+1)%size;
    }
}

void Show()
{
    if (front == -1)
    {
        printf("\nQueue Empty.\n");
        return;
    }
    printf("\nQueue Componets.\n");
    if (front == rear)
    {
        printf("\nFront ->    %d    <- Rear\n",queue[rear]);
        return;
    }
    else
    {
        printf("\nFront ->      ");
        int i = front;
        do
        {
            printf("%d      ",queue[i]);
            i = (i+1)%size;
        } while (i != rear);

    printf("%d      <- Rear\n",queue[i]);
    }
}

int main()
{
    int i,b,wrt_count, red_count,high,low; 
    pthread_t rtid[10],wtid[10];

    sem_init(&mutex,0,1);
    sem_init(&wrt,0,1);

    printf("\nNumber of Writers thread (Max 10) : ");
    if ( scanf("%d", &wrt_count) != 1)
    {
        printf("Error: Input type mismatch.\n");
        return 0;
    }
    if (wrt_count <= 0 || wrt_count > 10 )
    {
        printf("Error: Only positive numbers less than 10 expected.\n");
        return 0;
    }
    printf("\nNumber of Readers thread (Max 10) : ");
    if ( scanf("%d", &red_count) != 1)
    {
        printf("Error: Input type mismatch.\n");
        return 0;
    }
    if (red_count <= 0 || red_count > 10 )
    {
        printf("Error: Only positive numbers less than 10 expected.\n");
        return 0;
    }
  
    for(i=0; i<red_count; i++)
        pthread_create(&rtid[i],NULL,reader_func,(void *)i);
    for(i=0; i<wrt_count; i++)
        pthread_create(&wtid[i],NULL,writer_func,(void *)i);

    for(i=0; i<wrt_count; i++)
        pthread_join(wtid[i],NULL);
    for(i=0; i<red_count; i++)
        pthread_join(rtid[i],NULL);
    
    sem_destroy(&mutex);
    sem_destroy(&wrt);
    printf("\nProgram quitted.\n");
    return 0;
}

void *reader_func(void *arg)
{
    int opt = 1;
    while(opt != 3)
    {
        sleep(1);
        int id = ((int)arg);
        sem_wait(&mutex);
        rcount++;
        if (rcount == 1)
        {
            sem_wait(&wrt);
            printf("\n\nReader Thread %d Menu: ", id);
            printf("\n1. Dequeue element.");
            printf("\n2. Print Queue.");
            printf("\n3. Quit.");
            printf("\nEnter your option (1-3) : ");
            if (scanf("%d", &opt) != 1)
            {
                printf("Error: Only integers expected.");
                exit(0);
            }
            switch(opt)
            {
                case 1: Dequeue();
                        break;
                case 2: Show();
                        break;
                case 3: printf("\nReader Thread %d quitted.\n", id);
                        pthread_cancel(pthread_self());
                        break;
                default:printf("\nError: Invalid option input.");
                        //exit(0);
                        break;
            }
        }
        rcount--;
        if(rcount == 0)
            sem_post(&wrt);
        sem_post(&mutex);
    }
}

void *writer_func(void *arg)
{
    int opt = 1;
    while(opt!=2)
    {
        sleep(1);
        int id = ((int)arg);
        sem_wait(&wrt);
        if (count == 0)
            sem_wait(&mutex);
        printf("\n\nWriter Thread %d Menu : ", id);
        printf("\n1. Enqueue.");
        printf("\n2. Quit.");
        printf("\nEnter your option (1 or 2) : ");
        if (scanf("%d", &opt) != 1)
        {
            printf("Error: Only integers expected.");
            exit(0);
        }
        switch(opt)
        {
            case 1: printf("\nEnter number to be added - ");
                    int a;
                    if (scanf("%d", &a) != 1)
                    {
                        printf("Error: Only integers expected.");
                        if (count == 1)
                            sem_post(&mutex);
                        exit(0);
                    }
                    Enqueue(a);
                    Show();
                    if (count == 1)
                            sem_post(&mutex);
                    break;
            case 2: printf("\nWriter Thread %d quitted.\n", id);
                    if (count == 1)
                            sem_post(&mutex);
                    pthread_cancel(pthread_self());
                    break;
            default:printf("\n\nError: Invalid option input.");
                    if (count == 1)
                            sem_post(&mutex);
                    //exit(0);
                    break;
        }
        sem_post(&wrt);
    }
}