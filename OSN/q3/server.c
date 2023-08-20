#include<stdio.h>
#include<stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct send_message{
    int destination ;
    char x[1000];
}message;

int n,m;
int serverSocket, newSocket;
int a[1000],b[1000],d[1000];
int parent[1000],distance[1000];

void * func1(void *args){ // send 
    message *m1 = args;
    int parent_a[n],check = 0;
    for(int i = 0;i<n;++i) parent_a[i] = parent[i];
    if((m1->destination) >= n) printf("No route from node %d to node %d\n",0,(m1->destination));
    else {
        for(int j = (m1->destination);j!=0;j = parent_a[j]){
            if(j == -1) { check = 1; break; }
        }
        if(check == 1) printf("No route from node %d to node %d\n",0,(m1->destination));
        else {
            int capacity[n],cnt1 = 0;
            for(int j = (m1->destination);j!=0;j = parent_a[j]){
                capacity[cnt1++] = j;
            }
            for(int j = cnt1-1;j>0;j--){
                printf("Data recieved at node:%d; Source:%d; Destination:%d; Forwaded Destination:%d; Message:%s\n",capacity[j],0,(m1->destination),capacity[j+1],(m1->x));
                sleep(1);
            }
            if(m1->destination != 0)
            printf("Data recieved at node:%d; Source:%d; Destination:%d; Message:%s\n",capacity[0],0,(m1->destination),(m1->x));
        }
    }
}
void * func2(void * args){ // pt 
    message *m1 = args;
    int parent_a[n],value = -1;
    for(int i = 0;i<n;++i) parent_a[i] = parent[i];
    for(int i = 0 ;i<m;++i){
        if(a[i] == 0 || b[i] == 0){
            int to_find = (a[i] == 0)?b[i]:a[i];
            value = to_find;
            send(newSocket,&value,sizeof(value), 0);
            // printf("%d ",value);
            sleep(0.2);
            for(int j = value;j!=0;j = parent_a[j]){
                if(parent_a[j] == 0) value = j;
            }
            // printf("%d ",value);
            send(newSocket,&value,sizeof(value), 0);
            sleep(0.2);
            value = distance[to_find];
            send(newSocket,&value,sizeof(value), 0);
            sleep(0.2);
            // printf("%d ",value);
            // printf("\n");
        } 
    }
    value = -1;
    send(newSocket,&value,sizeof(value),0);
}

int main(){
    scanf("%d %d",&n,&m);
    int graph[n][n];
    for(int i = 0;i<n;++i){
        for(int j = 0;j<n;++j) graph[i][j] = -1;
    }
    for(int i = 0 ;i<m;++i) {
        scanf("%d %d %d",&a[i],&b[i],&d[i]);
        graph[a[i]][b[i]] = d[i];
    }
    for(int i = 1;i<n;++i) {distance[i] = 1e9+5;parent[i]  = -1;}
    distance[0] = 0;
    int visited[n]; for(int i = 0 ; i<n;++i) visited[i] = 0;
    for(int i = 0;i<n;++i){
        int u = -1;
        for(int j = 0;j<n;++j){
            if(!visited[j] && (u == -1 || distance[j] < distance[u])) u = j;
        }
        visited[u] = 1;
        for(int j = 0;j<n;++j){
            if(graph[u][j] != -1 && !visited[j] && (distance[j] > (distance[u]+graph[u][j]) )){
                distance[j]  = distance[u]+graph[u][j];
                parent[j]  = u;
            }
        }
    }
    // for(int i = 0 ;i<n;++i) printf("%d ",parent[i]);
    // printf("\n");
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8989);

    // Bind the socket to the address and port number.
    bind(serverSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr));

    // Listen on the socket,with 50 max connection requests queued
    if (listen(serverSocket, 50) == 0) printf("Listening\n");
    else    printf("Error\n");

    pthread_t threads1[51];
    pthread_t threads2[51];
    int i = 0;

    while(1){
        addr_size = sizeof(serverStorage);
        // Extract the first  connection in the queue
        newSocket = accept(serverSocket,(struct sockaddr*)&serverStorage,&addr_size);
        char choice[1000] ;
        recv(newSocket,&choice,sizeof(char)*1000, 0);
        // printf("%s\n",choice);
        char * tokens[1000];
        int si = 0;
        tokens[0] = strtok(choice," \t");
        for(si=0;;++si) {
            if(tokens[si] == NULL) break;
            else tokens[si+1] = strtok(NULL," \t");
        }
        if(strcmp(tokens[0],"send") == 0){
            message m;
            m.destination = atoi(tokens[1]);
            strcpy(m.x, tokens[2]);
            for(int i=3;i<si;++i) {strcat(m.x," ");strcat(m.x,tokens[i]);}
            pthread_create(&threads1[i++],NULL,&func1,&m);
        }
        else if(si == 1 && strcmp(tokens[0],"pt") == 0){
            message m;
            m.destination = 0;
            strcpy(m.x, tokens[0]);
            pthread_create(&threads2[i++],NULL,&func2,&m);
        }
        if (i >= 50) {
            // Update i
            i = 0;
            while (i < 50) {
                // Suspend execution of the calling thread until the target thread terminates
                pthread_join(threads1[i++],NULL);
                pthread_join(threads2[i++],NULL);
            }
            // Update i
            i = 0;
        }
    }
    return 0;
}