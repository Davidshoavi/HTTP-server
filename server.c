#include "segel.h"
#include "request.h"
#include "pthread.h"
#include <string.h>
typedef struct Node{
    int data;
    Node* next;
}Node;

typedef struct Queue{
    Node* head;
    Node* tail;
    int size;
    int maxSize;
}Queue;

typedef struct ThreadIn{
    Queue* queue;
    pthread_mutex_t* queueLock;
    pthread_cond_t* queueCond;
    int* tasksAmount;
    pthread_cond_t* tasksAmountCond;
    // add here things
}ThreadIn;

// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// HW3: Parse the new arguments too


void* doWork(ThreadIn* arg){
int connfd;

while(1){

    pthread_mutex_lock(arg->queueLock);
    while(arg->queue->size == 0){
        pthread_cond_wait(arg->queueCond, arg->queueLock);
    }
    connfd = arg->queue->head->next->data;
    dropHead(arg->queue);
    free(arg->queue->head->next);
    pthread_mutex_unlock(arg->queueLock);

    requestHandle(connfd);
    Close(connfd);

    pthread_mutex_lock(arg->queueLock);
    arg->tasksAmount--;
    pthread_cond_signal(arg->tasksAmountCond);
    pthread_mutex_unlock(arg->queueLock);


}


}


void getargs(int *port, int argc, char *argv[], int *maxSize, int *threadsNum, char **overloadMethod)
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *maxSize = atoi(argv[3]);
    *threadsNum = atoi(argv[2]);
    *overloadMethod = argv[4];
}


void dropHead(Queue* queue){
    queue->size--;
    if(queue->size == 0){
        queue->head->next = NULL;
        queue->tail = queue->head;
    }
    else{
        queue->head->next = queue->head->next->next;
    }
    free(queue->head->next);
}


void addRequest(Queue* requests, int* tasksAmount, int connfd){
    requests->tail->next = (Node*)malloc(sizeof(Node));
    requests->tail->next->data = connfd;
    requests->tail->next->next = NULL;
    requests->tail = requests->tail->next;
    requests->size++;
    tasksAmount++;
}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen;
    char* block = "block";
    char* dt = "dt";
    char* dh = "dh";
    struct sockaddr_in clientaddr;
    Queue* requests = (Queue*)malloc(sizeof(Queue));
    requests->head = (Node*)malloc(sizeof(Node));
    requests->tail = requests->head;
    requests->head->data = -1;
    requests->head->next = NULL;
    requests->size = 0;
    int threadsNum;
    char* overloadMethod;
    
    getargs(&port, argc, argv, &requests->maxSize, &threadsNum, &overloadMethod);

    pthread_t* threads [threadsNum];
    ThreadIn* threadArgs [threadsNum];

    pthread_mutex_t queueLock;
    pthread_mutex_init(&queueLock, NULL);
    pthread_cond_t queueCond;
    pthread_cond_init(&queueCond, NULL);
    int tasksAmount = 0;
    pthread_cond_t tasksAmountCond;
    pthread_cond_init(&tasksAmountCond, NULL);

    for(int i=0; i<threadsNum; i++){
        threadArgs[i] = (ThreadIn*)malloc(sizeof(ThreadIn));
        threadArgs[i]->queue = requests;
        threadArgs[i]->queueLock = &queueLock;
        threadArgs[i]->queueCond = &queueCond;
        threadArgs[i]->tasksAmount = &tasksAmount;
        threadArgs[i]->tasksAmountCond = &tasksAmountCond;
        //add things to threadArgs[i]
        pthread_create(&threads[i], NULL, doWork, threadArgs[i]);
    }

    // 
    // HW3: Create some threads...
    //

    listenfd = Open_listenfd(port);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

        if(strcmp(block, overloadMethod) == 0){ // Block method

            pthread_mutex_lock(&queueLock);
            while(tasksAmount == requests->maxSize){
                pthread_cond_wait(&tasksAmountCond, &queueLock);
            }
            addRequest(requests, &tasksAmount, connfd);
            pthread_cond_broadcast(&queueCond);
            pthread_mutex_unlock(&queueLock);
        }
        else if(strcmp(dt, overloadMethod) == 0){
            Close(connfd);
        }
        else if(strcmp(dh, overloadMethod) == 0){
            pthread_mutex_lock(&queueLock);
            if (tasksAmount == requests->maxSize){
                Close(requests->head->next->data); //closes the connection with the first node
                dropHead(requests);
                tasksAmount--;
            }
            addRequest(requests, &tasksAmount, connfd);
            pthread_cond_broadcast(&queueCond);
            pthread_mutex_unlock(&queueLock);
        }
        else{}



        // 
        // HW3: In general, don't handle the request in the main thread.
        // Save the relevant info in a buffer and have one of the worker threads 
        // do the work. 
        // 
        //requestHandle(connfd);

        //Close(connfd);
    }

}


    


 
