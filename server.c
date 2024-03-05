#include "segel.h"
#include "request.h"
typedef struct Node{
    int data;
    struct node* next;
}Node;

typedef struct Queue{
    Node* head;
    Node* tail;
    int size;
    int maxSize;
}Queue;

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
void getargs(int *port, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
}


int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;
    Queue* requests = (Queue*)malloc(sizeof(Queue));
    requests->head = (Node*)malloc(sizeof(Node));
    requests->tail = requests->head;
    requests->head->data = -1;
    requests->head->next = NULL;
    requests->size = 0;
    requests->maxSize = atoi(argv[2]); //which argv??
    
    getargs(&port, argc, argv);



    // 
    // HW3: Create some threads...
    //

    listenfd = Open_listenfd(port);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

        // 
        // HW3: In general, don't handle the request in the main thread.
        // Save the relevant info in a buffer and have one of the worker threads 
        // do the work. 
        // 
        requestHandle(connfd);

        Close(connfd);
    }

}


    


 
