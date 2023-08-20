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
#define BLUE    "\x1B[34m"
#define RESET   "\x1B[0m"

int track,n,m ,o,chefs_cnt,ovens_cnt,pizza_ok,fp;
int ingr_amt[1000];
sem_t curr_sem[1000];
sem_t curr_sem2[1000];
struct timespec time_req,time_req2,begin,now,valid;

typedef struct chefs{
    int id ;
    int working ;
    int arrival_time ;
    int exit_time ;
}chefs;

typedef struct customers{
    int id ;
    int entry_time ;
    int cnt ; // to keep track of pizza_cnt ordered 
    int order_id ;
    int check ; 
    int pizzas[1000];
    int pizzas_st[1000];
}customers ;

typedef struct pizzas{
    int id ;
    int time_taken  ;
    int ingredients ;
    int ing_id[1000];
    int pre_cust;
    int pre_ord ;
}pizza;

typedef struct ovens{
    int id;
    int working ;
}ovens;

pizza pizza_info[1000];// assuming atmax 1000 max pizza varieties 
chefs chef_status[1000];// at max 1000 chefs work
ovens oven_status[1000];// assuming at max 1000 ovens 
pthread_t oven[1000];
pthread_t pizza_1[10000];// assuming atmax 10,000 pizzas ordered per day 

void * thread1(void * arg){
    chefs * x = arg ;
    sleep(x->arrival_time);
    chefs_cnt++;
    x->working = 0;
    printf("\x1B[34mChef %d arrives at time %d\x1B[0m\n",x->id,x->arrival_time);
    sleep(x->exit_time - x->arrival_time);
    chefs_cnt--;
    x->working = -1;
    printf("\x1B[34mChef %d exits at time %d\x1B[0m\n",x->id,x->exit_time);
}

void * thread3(void * arg){
    pizza *x = arg;
    int val1 = (x->pre_cust);
    int val2 = (x->pre_ord);
    int c = 0;
    while(c == 0){
        int cnt3 = 0;
        for(int i = 0;i<n;++i){
            clock_gettime(CLOCK_REALTIME, &valid) ;
            if(chef_status[i].working == 0 && (valid.tv_sec + (x->time_taken)) <= (begin.tv_sec + chef_status[i].exit_time )){
                int s ;
                clock_gettime(CLOCK_REALTIME, &time_req) ;
                time_req.tv_sec += 1;
                while (((s = sem_timedwait(&curr_sem[chef_status[i].id],&time_req)) == -1) && errno == EINTR);
                if(s == 0){
                    c = 1;
                    printf("Pizza %d in order %d assigned to chef %d\x1B[0m\n",x->id,val2,chef_status[i].id);
                    printf("\x1B[34mChef %d is preparing pizza %d from order %d\x1B[0m\n",chef_status[i].id , x->id,val2);
                    chef_status[i].working = 1;
                    sleep(3);
                    int c1 = 0;
                    while(c1 == 0){
                        for(int j = 0; j < o;++j){
                            if(oven_status[j].working == 0){
                                int s1 ; 
                                clock_gettime(CLOCK_REALTIME,&time_req2);
                                time_req2.tv_sec += 1;
                                while (((s1 = sem_timedwait(&curr_sem2[oven_status[j].id],&time_req2)) == -1) && errno == EINTR);
                                if(s1 == 0){
                                    c1 = 1;
                                    clock_gettime(CLOCK_REALTIME, &now) ;
                                    fp  = (now.tv_sec)  -(begin.tv_sec);
                                    printf("\x1B[34mChef %d has put the pizza %d for order %d in oven %d at time %d\x1B[0m\n",chef_status[i].id,x->id,val2,oven_status[j].id ,fp);
                                    oven_status[j].working = 1;
                                    sleep(x->time_taken-3);
                                    printf("\x1B[34mChef %d has picked up the pizza %d for order %d from the oven %d at time %d\x1B[0m\n",chef_status[i].id,x->id,val2,oven_status[j].id,fp+(x->time_taken-3));
                                    oven_status[j].working = 0;
                                    sem_post(&curr_sem2[oven_status[j].id]);
                                    break;
                                }
                            }
                        }
                        sleep(1);
                    }
                    printf("\x1B[33mCustomer %d picks up their pizza %d\x1B[0m\n",val1,x->id);
                    chef_status[i].working = 0;
                    sem_post(&curr_sem[chef_status[i].id]);
                    break;
                }
            }
            else if(chef_status[i].working == -1) cnt3++;
        }
        if(cnt3 == n) break;
        sleep(1);
    }
}

void * thread2(void * arg){
    customers *x = arg;
    sleep(x->entry_time);
    printf("\x1B[33mCustomer %d arrives at time %d\x1B[0m\n",x->id,x->entry_time);
    track++;// to keep track of order id 
    x->order_id = track;
    printf("\x1B[31mOrder %d placed by customer %d has pizzas {%d",x->order_id,x->id,x->pizzas[0]);
    for(int i = 1;i<(x->cnt);i++) printf(",%d",x->pizzas[i]);
    printf("}\x1B[0m\n");
    printf("\x1B[31mOrder %d placed by the customer %d awaits processing\x1B[0m\n",x->order_id,x->id);

    if(x->check == 0){
        printf("\x1B[33mCustomer %d rejected\x1B[0m\n",x->id);
        printf("\x1B[33mCustomer %d exits the drive-thru zone\x1B[0m\n",x->id);
    }
    else {
        printf("\x1B[31mOrder %d placed by customer %d is being processed\x1B[0m\n",x->order_id,x->id);
        for(int i = 0;i < (x->cnt);++i){
            if(x->pizzas_st[i] != -1) {
                for(int j = 0;j<m;++j){
                    if(pizza_info[j].id == (x->pizzas[i])) {
                        pizza_info[j].pre_cust = (x->id);
                        pizza_info[j].pre_ord = (x->order_id);
                        pthread_create(&pizza_1[x->pizzas_st[i]],NULL,thread3,&pizza_info[j]);
                        break;
                    }
                }
            }
        }
        for(int i = 0;i < (x->cnt);++i){
            if(x->pizzas_st[i] != -1)  pthread_join(pizza_1[x->pizzas_st[i]],NULL);
        }
        printf("\x1B[33mCustomer %d exits the drive-thru zone\x1B[0m\n",x->id);
    }
}

int main()
{
    int ing,c,k;  scanf("%d %d %d %d %d %d",&n,&m,&ing,&c,&o,&k);
    pthread_t chef[n];
    pthread_t customer[c];
    customers customer_info[c];
    for(int i = 0;i<n ;++i) sem_init(&curr_sem[i],0,1); 
    for(int i = 0;i<o;++i) sem_init(&curr_sem2[i],0,1);
    for(int i = 0;i<m;++i){
        int id , time , no_ing ; scanf("%d %d %d",&id,&time,&no_ing);
        pizza_info[i].id = id ;
        pizza_info[i].time_taken = time ;
        pizza_info[i].ingredients = no_ing ;
        for(int j = 0;j<no_ing;++j){
            int ingr_id ; scanf("%d",&ingr_id);
            pizza_info[i].ing_id[j] = ingr_id;
        }
    }

    for(int  i= 0;i<ing;++i) scanf("%d",&ingr_amt[i]);

    for(int i = 0;i<n ;++i){
        int arr , exit ; scanf("%d %d",&arr,&exit);
        chef_status[i].arrival_time = arr ;
        chef_status[i].exit_time = exit ;
        chef_status[i].id  = i+1;
        chef_status[i].working = -1;
    }

    for(int i = 0;i<c ;++i){
        int arr , cnt,check = 0 ; scanf("%d %d",&arr,&cnt);
        customer_info[i].entry_time = arr;
        customer_info[i].cnt = cnt ;
        customer_info[i].id = i+1;
        for(int j = 0;j<cnt ;++j) {
            int pizza_id ; scanf("%d",&pizza_id);
            customer_info[i].pizzas[j] = pizza_id;
            for(int l = 0;l<m;l++) {
                if(pizza_info[l].id == pizza_id){
                    int check1 = 1;
                    for(int li = 0;li<(pizza_info[l].ingredients);li++){
                        if(ingr_amt[pizza_info[l].ing_id[li]-1] <= 0 ) check1 = 0;
                    }
                    if(check1){
                        for(int li = 0;li<(pizza_info[l].ingredients);li++) ingr_amt[pizza_info[l].ing_id[li]-1]--;
                        customer_info[i].pizzas_st[j] = (pizza_ok++); 
                        check++;
                    }
                    else customer_info[i].pizzas_st[j] = -1;
                    break;
                }
            }
        } 
        customer_info[i].check = check;
    }
    
    for(int i=0;i<o;++i) {
        oven_status[i].id =i+1;
        oven_status[i].working = 0;
    }
    clock_gettime(CLOCK_REALTIME, &begin) ;
    printf("Simulation Started\n");
    for(int i = 0;i<n ;++i){
        pthread_create(&chef[i],NULL,thread1,&chef_status[i]);
    }
    for(int i = 0;i<c ;++i){
        pthread_create(&customer[i],NULL,thread2,&customer_info[i]);
    }
    for(int i = 0;i<n ;++i){
        pthread_join(chef[i],NULL);
    }
    for(int i = 0;i<c ;++i){
        pthread_join(customer[i],NULL);
    }
    printf("Simulation Ended\n");
}

// 3 3 4 3 5 3
// 1 20 3 1 2 3
// 2 30 2 2 3   
// 3 30 1 4
// 10 5 3 0
// 0 60 20 60 30 120
// 1 1 1
// 2 2 1 2
// 4 1 3