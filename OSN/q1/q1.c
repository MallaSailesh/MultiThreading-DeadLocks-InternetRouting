#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include<time.h>
#include<errno.h>

#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define WHITE   "\x1B[37m"
#define RESET   "\x1B[0m"

struct timespec pt_time ,start_time,end_time,begin,now;
typedef struct student_det{
    int id ; 
    int arrive_time ;
    int wash_time;
    int patience_time ;
} student_det;

int compare(const void * x , const void * y ){
    const student_det *s1 = x ;
    const student_det *s2 = y ;
    return s1->arrive_time - s2->arrive_time;
}

sem_t curr_sem ;
int cnt , total_time,fp;

void * thread(void * arg){
    student_det *x = arg;
    clock_gettime(CLOCK_REALTIME, &now) ;
    fp = (now.tv_sec)-(begin.tv_sec);
    printf("%d: Student %d arrives\n",fp,x->id);
    int s ;
    clock_gettime(CLOCK_REALTIME, &pt_time) ;
    pt_time.tv_sec += (x->patience_time) ;
    clock_gettime(CLOCK_REALTIME, &start_time) ;
    while ( ((s = sem_timedwait(&curr_sem, &pt_time)) == -1) && errno == EINTR)  continue;
    if(s == 0){ // success 
        clock_gettime(CLOCK_REALTIME, &end_time) ;
        total_time += (end_time.tv_sec) - (start_time.tv_sec);
        clock_gettime(CLOCK_REALTIME, &now) ;
        fp = (now.tv_sec)-(begin.tv_sec);
        printf("\x1B[32m%d: Student %d starts washing\x1B[0m\n",fp,x->id);
        sleep(x->wash_time);
        clock_gettime(CLOCK_REALTIME, &now) ;
        fp = (now.tv_sec)-(begin.tv_sec);
        printf("\x1B[33m%d: Student %d leaves after washing\x1B[0m\n",fp,x->id);
        sem_post(&curr_sem);
    }
    else{
        total_time += (x->patience_time);
        // printf("%d ",s);
        clock_gettime(CLOCK_REALTIME, &now) ;
        fp = (now.tv_sec)-(begin.tv_sec);
        if(errno == ETIMEDOUT) {printf("\x1B[31m%d: Student %d leaves without washing\x1B[0m\n",fp,x->id); cnt++;}
        else perror("sem_timedwait\n");
    }
}

int main()
{
    int n , m ; scanf("%d %d\n",&n,&m);
    sem_init(&curr_sem,0,m);
    int arrival[n],wash[n],patience[n];
    for(int i = 0;i<n;++i) scanf("%d %d %d",&arrival[i],&wash[i],&patience[i]);
    pthread_t student[n];
    student_det student_info[n];
    int initial_time = 0 ; // for 0 seconds

    for(int i = 0 ; i < n ; ++i){
        // student_info[i] = malloc(sizeof(student_det));
        student_info[i].id = i+1;
        student_info[i].arrive_time = arrival[i];
        student_info[i].wash_time = wash[i];
        student_info[i].patience_time = patience[i];
    }

    qsort(student_info,n,sizeof(student_det),compare);
    clock_gettime(CLOCK_REALTIME, &begin) ;
    for(int i = 0; i<n ; ++i){
        int val = student_info[i].arrive_time - initial_time;
        sleep(val);
        pthread_create(&student[i],NULL,thread,&student_info[i]);
        initial_time = student_info[i].arrive_time;
    }
    for(int i = 0 ; i < n ; ++i){
        pthread_join(student[i],NULL);
    }
    sem_destroy(&curr_sem);
    printf("%d\n",cnt);
    // int tot_time  = total_time;
    printf("%d\n",total_time);
    double calc = cnt*100/n;
    if(calc >= 25) printf("Yes\n");
    else printf("No\n");
    return 0;
}
// 5 2
// 6 3 5
// 3 4 3
// 6 5 2
// 2 9 6
// 8 5 2