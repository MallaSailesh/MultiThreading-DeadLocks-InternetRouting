#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct send_message{
    int destination ;
    char x[1000];
}message;
int network_socket;
void* clienthread(void* args)
{
    message * m = args ;
    // printf("%d %s\n",m->destination,m->x);
    char  client_request[1000]; strcpy(client_request,m->x);
    // Create a stream socket
    network_socket = socket(AF_INET,SOCK_STREAM, 0);
    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8989);
    // Send data to the socket
    int connection_status = connect(network_socket,(struct sockaddr*)&server_address,sizeof(server_address));
    // Check for connection error
    if (connection_status < 0) {
        printf("Error\n");
        return 0;
    }
    printf("Connection Established\n");
    send(network_socket,client_request,sizeof(char)*1000, 0);// flags generally set to zero
    if(strcmp(m->x,"pt") == 0){
        printf("dest\t\tforw\t\tdelay\t\t");
        printf("\n");
        // listen(network_Socket, 1);
        int choice = 0,cnt = 0 ;
        while(1){
            recv(network_socket,&choice,sizeof(choice),0);
            cnt++;
            if(choice == -1) exit(1) ;
            printf("%d\t\t",choice);
            if(cnt == 3) {printf("\n");cnt = 0;}
        }
        printf("%d\n",choice);
    }
    // Close the connection
    close(network_socket);
    pthread_exit(NULL);
    return 0;
}

void tokenise_space(char x[]){
    char y[1000]; strcpy(y,x);
    char * tokens[1000];
    int si = 0;
    tokens[0] = strtok(x," \t");
    for(si=0;;++si) {
        if(tokens[si] == NULL) break;
        else tokens[si+1] = strtok(NULL," \t");
    }
    if(strcmp(tokens[0],"send") == 0){
        pthread_t send_m;
        message m;
        m.destination = atoi(tokens[1]);
        strcpy(m.x, y);
        pthread_create(&send_m,NULL,&clienthread,&m);
        pthread_join(send_m,NULL);
    }
    else if(si == 1 && strcmp(tokens[0],"pt") == 0){
        pthread_t send_m2;
        message m2;
        m2.destination = 0;
        strcpy(m2.x, y);
        pthread_create(&send_m2,NULL,&clienthread,&m2);
        pthread_join(send_m2,NULL);
    }
    else {
        printf("Invalid Input\n");
    }
}

int main(){
    char x[1000];
    x[0] = '\0'; scanf("%[^\n]s",x); 
    tokenise_space(x);
    return 0;
}