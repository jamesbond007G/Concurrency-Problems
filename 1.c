#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "semaphore.h"
#include "unistd.h"
#include "string.h"
#include "sys/time.h"
#include "time.h"
int counter = 0;
int which_machine[1000000];
int completed[100000];
int booked[100000];
typedef struct input
{
    int index;
    int t_arr_index;
    int tol_index;
    int coffee_time;
    char *coffee_name;
} input;
typedef struct cofee
{
    int c_time;
    char *c;
} cofee;
sem_t customers_sema[100000];
sem_t machines_sema[100000];
sem_t lock;
sem_t lock1;

// custom function from chatgpt/ tanishq
int global_timer = -1e4;
int time_from_start()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    if (global_timer == -1e4)
    {
        global_timer = tv.tv_sec;
        return 0;
    }
    return tv.tv_sec - global_timer;
}

int time1[1000000];
pthread_mutex_t mutex;
int machine_book[10000];
// pthread_mutex_init(&mutex, NULL);
void *func_customer(void *arg)
{

    while (((input *)arg)->t_arr_index > time_from_start())
    {
        sleep(1);
    }
    printf("customer %d came at time %d seconds(s).\n", (((input *)arg)->index) - 1, time_from_start());
    printf("\033[33mcustomer %d orders a %s.\033[0m\n", ((input *)arg)->index - 1, ((input *)arg)->coffee_name);
    sleep(1);
    struct timespec abs_time;
    clock_gettime(CLOCK_REALTIME, &abs_time);
    abs_time.tv_sec += (((input *)arg)->tol_index);
    int result = sem_timedwait(&customers_sema[(((input *)arg)->index) - 1], &abs_time);
    time1[(((input *)arg)->index) - 1] = 1e4;

    if (result == -1)
    {
        // pthread_mutex_lock(&mutex);
        // pthread_mutex_unlock(&mutex);
        printf("customer %d leaves at %d\n", (((input *)arg)->index) - 1, time_from_start());
        time1[((input *)arg)->index - 1] = 1e9;
        completed[(((input *)arg)->index) - 1] = 1;
        sem_post(&machines_sema[which_machine[(((input *)arg)->index) - 1]]);
        // printf("okg %d at %d\n", completed[5], time_from_start());
        return NULL;
    }
    printf("\033[36mBarista  %d begins preparing the order of Customer  %d at %d seconds(s). \033[0m\n", which_machine[(((input *)arg)->index) - 1], (((input *)arg)->index) - 1, time_from_start());
    int temp1 = time_from_start();
    if (temp1 + ((input *)arg)->coffee_time > ((input *)arg)->t_arr_index + ((input *)arg)->tol_index)
    {
        if ((((input *)arg)->t_arr_index + ((input *)arg)->tol_index - temp1 + 1) >= 0)
        {
            sleep(((input *)arg)->t_arr_index + ((input *)arg)->tol_index - temp1 + 1);
            printf("\033[31mCustomer %d leaves without their order at %d.\033[0m\n", (((input *)arg)->index) - 1, ((input *)arg)->t_arr_index + ((input *)arg)->tol_index + 1);
            completed[(((input *)arg)->index) - 1] = 1;

            if ((temp1 + ((input *)arg)->coffee_time - (((input *)arg)->t_arr_index + ((input *)arg)->tol_index + 1)) > 0)
            {
                sleep(temp1 + ((input *)arg)->coffee_time - (((input *)arg)->t_arr_index + ((input *)arg)->tol_index + 1));
            }
            completed[((input *)arg)->index - 1] = 1;
            sem_post(&machines_sema[which_machine[(((input *)arg)->index) - 1]]);
            printf("Cofee gets wasted at %d\n", time_from_start());
        }
    }
    else
    {
        sleep((((input *)arg)->coffee_time));
        completed[(((input *)arg)->index) - 1] = 1;
        sem_post(&machines_sema[which_machine[(((input *)arg)->index) - 1]]);
        printf("\033[34mBarista %d succesfully completes the order of customer %d at %d second(s).\033[0m\n", which_machine[(((input *)arg)->index) - 1], (((input *)arg)->index) - 1,time_from_start());
        printf("\033[32mCustomer %d leaves with their order at %d second(s).\033[0m\n", (((input *)arg)->index) - 1, time_from_start());
    }
    return NULL;
}
int n;
// int random_
int b;
pthread_mutex_t prev_machine_lock; 
void *func_machine(void *arg)
{
    int f1 = 0;
    int f2 = 0;
    int *l = (int *)arg;

    while (1)
    {
        // printf("gopal %d %d \n",*l,time_from_start());
        // sleep(0.01);
        usleep(50000);
        int executing_customer = 1e4;
        int current_min_time = 1e4;
        int flag1 = 0;
        for (int i = 0; i < n; i++)
        {
            if (time1[i] < 1e4)
            {
                flag1 = 1;
                break;
            }
        }
        if (flag1 == 0)
        {
            // sem_post(&lock);
            break;
        }
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < n; i++)
        {
            if (time1[i] < current_min_time)
            {
                current_min_time = time1[i];
                executing_customer = i;
            }
        }
        if (booked[executing_customer] == 1)
        {
            continue;
        }
        time1[executing_customer] = 1e4;
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&prev_machine_lock);
        int prev_machine_flag = 0;
        for (int i = 0; i < *l; i++)
        {
            if (machine_book[i] == 0)
            {
                prev_machine_flag = 1;
                break;
            }
        }
        if (prev_machine_flag == 1)
        {
        pthread_mutex_unlock(&prev_machine_lock);
            continue;
        }
        machine_book[*l] = 1;
        pthread_mutex_unlock(&prev_machine_lock);

        sleep(1);

        int *k = (int *)arg;

        booked[executing_customer] = 1;
        // printf("customer %d trying to get barista %d at %d\n", executing_customer, *k, time_from_start());
        sem_post(&customers_sema[executing_customer]);
        which_machine[executing_customer] = *k;
        sem_wait(&machines_sema[*k]);
        sem_post(&lock);
    }
    int *k = (int *)arg;
    return NULL;
}

int main()
{
    int k;
    sem_init(&lock, 0, 1);
    sem_init(&lock1, 0, 1);
    for (int i = 0; i < 10000; i++)
    {
        machine_book[i] = 0;
    }

    scanf("%d %d %d", &b, &k, &n);
    cofee coffee_types[k];
    // cin >> b >> k >> n;
    for (int i = 0; i < 100000; i++)
    {
        completed[i] = 0;
    }
    for (int i = 0; i < 100000; i++)
    {
        booked[i] = 0;
    }
    completed[10000] = 1;

    for (int i = 0; i < k; i++)
    {
        coffee_types[i].c = (char *)malloc(sizeof(char) * 4000);
        scanf("%s %d", coffee_types[i].c, &coffee_types[i].c_time);
    }
    pthread_t customers[n];
    pthread_t machines[b];
    // sema
    pthread_t time;
    input inputs[n];

    for (int i = 0; i < n; i++)
    {
        int index, t_arr_index, tol_index;
        char *string1 = (char *)malloc(sizeof(char) * 4096);
        scanf("%d %s %d %d", &index, string1, &t_arr_index, &tol_index);
        for (int j = 0; j < k; j++)
        {
            if (strcmp(string1, coffee_types[j].c) == 0)
            {
                inputs[i].coffee_time = coffee_types[j].c_time;
                break;
            }
        }
        inputs[i].coffee_name = (char *)malloc(sizeof(char) * 4000);
        strcpy(inputs[i].coffee_name, string1);
        inputs[i].index = index;
        inputs[i].t_arr_index = t_arr_index;
        inputs[i].tol_index = tol_index;
        sem_init(&customers_sema[i], 0, 0);
        time1[i] = t_arr_index;
    }
    for (int i = 0; i < n; i++)
    {
        pthread_create(&customers[i], NULL, (void *)func_customer, (void *)(inputs + i));
    }
    int arg[b];
    for (int i = 0; i < b; i++)
    {

        arg[i] = i;
        sem_init(&machines_sema[i], 0, 0);
    }
    // for (int i = 0; i < count; i++)
    // {
    //     /* code */
    // }

    for (int i = 0; i < b; i++)
    {
        pthread_create(&machines[i], NULL, (void *)func_machine, (void *)(arg + i));
    }
    for (int i = 0; i < n; i++)
    {
        pthread_join(customers[i], NULL);
    }
    for (int i = 0; i < b; i++)
    {
        pthread_join(machines[i], NULL);
    }

    return 0;
}