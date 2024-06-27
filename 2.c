#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "semaphore.h"
#include "unistd.h"
#include "string.h"
#include "sys/time.h"
#include "time.h"
typedef struct machine
{
    int start_time;
    int end_time;
} machine;
typedef struct string
{
    char *s;

} string;
typedef struct customer
{
    int number;
    int arrival_time;
    string **toppings;
    int *flavours;
    int number_of_ice;
    int flavour_time;
    int *number_of_toppings;

} customer;
int n;
machine machines[100000];
typedef struct order
{
    int customer_number;
    char **toppings_order;
    int flavour_time;
    int number_of_toppings;
    int arrival_time;
    int order_number;
} order;
typedef struct flavour
{
    char *name;
    int time_flavour;
} flavour;
typedef struct topping
{
    // int topping_number;
    char *name;
    int q_topping;
} topping;
// topping toppings
topping toppings[100000];
int t;
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
int repeat_flag[100000];
int customer_print_remark[100000];
int customer_ing_remark[100000];
int customer_un_remark[100000];
pthread_mutex_t ing_lock;
pthread_mutex_t un_lock;
pthread_mutex_t tp_lock;
int order_count = 0;
order all_orders[100000];
int check_ingredient(int l);
sem_t order_sema[100000];
sem_t machines_sema[100000];
int which_machine[100000];
int completed[100000];
int insufficient[100000];
pthread_mutex_t mutex;
pthread_mutex_t lock_for_all_ingredient_check;
pthread_mutex_t first_time_visit_customer_lock;
pthread_mutex_t first_time_visit_customer_lock1;
int booked_machine[100000];
pthread_mutex_t booked_lock;
int closed_machine[100000];
int time_all_order_retreat[100000];
int first_time_visit_customer[100000];
int ghrwpasi[100000];
int check_all_ingredient(int l)
{
    pthread_mutex_lock(&lock_for_all_ingredient_check);
    char *temp = (char *)malloc(sizeof(char) * 4096);
    // for (int k = 0; k < order_count; k++)
    // {
    // if()
    topping total_customer_requirement[1000];

    for (int i = 0; i < t; i++)
    {
        total_customer_requirement[i].name = (char *)malloc(sizeof(char) * 400);
        strcpy(total_customer_requirement[i].name, toppings[i].name);
        total_customer_requirement[i].q_topping = 0;
    }

    for (int ln = 0; ln < order_count; ln++)
    {
        /* code */
        if (all_orders[ln].customer_number == all_orders[l].customer_number)
        {
            for (int i = 0; i < all_orders[ln].number_of_toppings; i++)
            {
                char *temp = (char *)malloc(sizeof(char) * 400);
                strcpy(temp, all_orders[ln].toppings_order[i]);
                for (int kkkkk = 0; kkkkk < t; kkkkk++)
                {
                    if (strcmp(temp, total_customer_requirement[kkkkk].name) == 0)
                    {
                        total_customer_requirement[kkkkk].q_topping++;
                        // continue;
                        // break;
                    }
                }
            }
        }
    }
    // }
    // for (int i = 0; i < all_orders[l].number_of_toppings; i++)
    // {
    //     char *temp1 = (char *)malloc(sizeof(char) * 4096);
    //     strcpy(temp1, all_orders[l].toppings_order[i]);
    //     for (int j = 0; j < t; j++)
    //     {
    //         if (strcmp(toppings[j].name, temp1) == 0)
    //         {
    //             toppings[j].q_topping--;
    //         }
    //     }
    // }
    for (int i = 0; i < t; i++)
    {
        // printf("gopal%d %s %d %d at %d\n", i, toppings[i].name, toppings[i].q_topping, total_customer_requirement[i].q_topping, time_from_start());
        if (toppings[i].q_topping >= total_customer_requirement[i].q_topping)
        {
            continue;
        }
        else
        {
            // printf("gopal %d %d %s %d\n", all_orders[l].order_number,toppings[i].q_topping,total_customer_requirement[i].name, total_customer_requirement[i].q_topping);
            // printf("%s")
            pthread_mutex_unlock(&lock_for_all_ingredient_check);
            return 0;
        }
    }

    pthread_mutex_unlock(&lock_for_all_ingredient_check);

    return 1;
}
pthread_mutex_t random_mc;

void *machine_handler(void *arg)
{
    int *l = (int *)arg;
    while (machines[*l].start_time > time_from_start())
    {
        sleep(1);
    }
    printf("\033[33;91mMachine %d has started working at %d second(s).\033[0m\n", *l, time_from_start());
    while (1)
    {
        int timmmmmmmm = time_from_start();
        if (timmmmmmmm >= machines[*l].end_time)
        {
            closed_machine[*l] = 1;

            printf("\033[33;91mMachine %d has stopped working at %d second(s).\033[0m\n", *l, timmmmmmmm);
            // pthread_mutex_lock(&mutex);
            int random_flag = 0;
            for (int i = 0; i < n; i++)
            {
                if (closed_machine[i] == 0)
                {
                    random_flag = 1;
                    break;
                }
            }
            if (!random_flag)
            {
                for (int i = 0; i < order_count; i++)
                {
                    if (completed[i] == 0)
                    {
                        sem_post(&order_sema[i]);
                    }
                }
            }
            return NULL;
            // break;
        }
        pthread_mutex_lock(&mutex);

        int index_of_suited_order = -1;
        for (int i = 0; i < order_count; i++)
        {

            if (time_from_start() >= all_orders[i].arrival_time + 1 && time_from_start() + all_orders[i].flavour_time <= machines[*l].end_time && completed[i] == 0)
            {
                // printf("order %d at %d + %d \n", i, time_from_start(), all_orders[i].flavour_time);
                pthread_mutex_lock(&first_time_visit_customer_lock);
                // printf("yha 1 at %d by mahcine %d\n", time_from_start(), *l);

                if (first_time_visit_customer[all_orders[i].customer_number] == 0)
                {
                    // printf("yha at %d by mahcine %d\n", time_from_start(), *l);
                    if (check_all_ingredient(i) == 0)
                    {
                        for (int j = 0; j < order_count; j++)
                        {
                            if (all_orders[i].customer_number == all_orders[j].customer_number)
                            {
                                insufficient[j] = 1;
                                completed[j] = 1;
                                sem_post(&order_sema[j]);
                            }
                        }
                        first_time_visit_customer[all_orders[i].customer_number] = 1;
                        pthread_mutex_unlock(&first_time_visit_customer_lock);
                        continue;
                    }
                    first_time_visit_customer[all_orders[i].customer_number] = 1;
                }
                pthread_mutex_unlock(&first_time_visit_customer_lock);
                index_of_suited_order = i;

                break;
            }
        }
        if (index_of_suited_order == -1)
        {
            pthread_mutex_unlock(&mutex);

            continue;
        }
        completed[index_of_suited_order] = 1;
        pthread_mutex_unlock(&mutex);
        if (index_of_suited_order != -1 && check_ingredient(index_of_suited_order))
        {
            printf("YES at %d\n", time_from_start());
            sem_post(&order_sema[index_of_suited_order]);
            which_machine[index_of_suited_order] = *l;
            sem_wait(&machines_sema[*l]);
        }
        else
        {
            if (index_of_suited_order != -1)
            {
                pthread_mutex_lock(&random_mc);
                insufficient[index_of_suited_order] = 1;
                // printf("YESSSSSSS by %d\n",*l);
                for (int i = 0; i < order_count; i++)
                {
                    if (all_orders[index_of_suited_order].customer_number == all_orders[i].customer_number && completed[i] == 0)
                    {
                        insufficient[i] = 1;
                        completed[i] = 1;
                        sem_post(&order_sema[i]);
                    }
                }
                pthread_mutex_unlock(&random_mc);

                sleep(1);
                sem_post(&order_sema[index_of_suited_order]);
            }
            else
            {
                return NULL;
            }
        }
    }

    return NULL;
}
pthread_mutex_t mutex1;
int check_ingredient(int l)
{
    pthread_mutex_lock(&mutex1);
    char *temp = (char *)malloc(sizeof(char) * 4096);
    // for (int k = 0; k < order_count; k++)
    // {
    // if()
    for (int i = 0; i < all_orders[l].number_of_toppings; i++)
    {
        strcpy(temp, all_orders[l].toppings_order[i]);
        //  if(all_orders[l].toppings_order[i],)
        for (int j = 0; j < t; j++)
        {
            if (strcmp(toppings[j].name, temp) == 0)
            {
                if (toppings[j].q_topping > 0)
                {
                    continue;
                }
                else
                {
                    // printf("Yes at %d\n", l);
                    pthread_mutex_unlock(&mutex1);
                    return 0;
                }
            }
        }
    }
    // }
    for (int i = 0; i < all_orders[l].number_of_toppings; i++)
    {
        char *temp1 = (char *)malloc(sizeof(char) * 4096);
        strcpy(temp1, all_orders[l].toppings_order[i]);
        for (int j = 0; j < t; j++)
        {
            if (strcmp(toppings[j].name, temp1) == 0)
            {
                toppings[j].q_topping--;
            }
        }
    }
    pthread_mutex_unlock(&mutex1);

    return 1;
}
int ghrwpasi[100000];

void *order_handler(void *arg)
{
    int *l = (int *)arg;
    while (all_orders[*l].arrival_time > time_from_start())
    {
        sleep(1);
    }
    // printf("ab ghus rha h\n");
    pthread_mutex_lock(&first_time_visit_customer_lock1);

    if (check_all_ingredient(*l) == 0)
    {
        if (ghrwpasi[all_orders[*l].customer_number] == 0)
        {
            // printf("YES\n");
            printf("Customer %d leaves at %d due to ingredient shortage.\n", all_orders[*l].customer_number, time_from_start());
            ghrwpasi[all_orders[*l].customer_number] = 1;
        }
        sleep(1);
        pthread_mutex_unlock(&first_time_visit_customer_lock1);
        return NULL;
    }
    pthread_mutex_unlock(&first_time_visit_customer_lock1);

    pthread_mutex_lock(&tp_lock);
    if (customer_print_remark[all_orders[*l].customer_number] == 0)
    {
        /* code */

        printf("Customer %d enters at time %d\n", all_orders[*l].customer_number, time_from_start());
        int number_of_ice = 0;
        for (int i = 0; i < order_count; i++)
        {
            if (all_orders[i].customer_number == all_orders[*l].customer_number)
            {
                number_of_ice++;
                /* code */
            }
        }
        printf("\033[33mCustomer  %d orders %d icecreams\033[0m\n", all_orders[*l].customer_number, number_of_ice);
        int icecream_count = 1;
        for (int i = 0; i < order_count; i++)
        {
            if (all_orders[i].customer_number == all_orders[*l].customer_number)
            {
                printf("\033[33mIcecream %d : \033[0m", icecream_count);
                for (int j = 0; j < all_orders[i].number_of_toppings; j++)
                {
                    printf("\033[33m%s \033[0m", all_orders[i].toppings_order[j]);
                }
                printf("\033[0m\n");
                // printf("\n");
                icecream_count++;
            }
        }
        customer_print_remark[all_orders[*l].customer_number] = 1;
    }
    pthread_mutex_unlock(&tp_lock);
    sem_wait(&order_sema[*l]);

    // sleep(1);
    // if (ghrwpasi[*l] == 1)
    // {
    //     return NULL;
    // }
    // printf()
    int random_flag = 0;
    pthread_mutex_lock(&ing_lock);
    if (insufficient[*l] == 1 && customer_ing_remark[all_orders[*l].customer_number] == 0)
    {
        customer_ing_remark[all_orders[*l].customer_number] = 1;
        pthread_mutex_unlock(&ing_lock);
        printf("Customer %d leaves at %d due to ingredient shortage\n", all_orders[*l].customer_number, time_from_start());
        return NULL;
    }
    else if (insufficient[*l] == 1 && customer_ing_remark[all_orders[*l].customer_number] == 1)
    {
        pthread_mutex_unlock(&ing_lock);
        return NULL;
    }
    pthread_mutex_unlock(&ing_lock);

    for (int i = 0; i < n; i++)
    {
        if (closed_machine[i] == 0)
        {
            random_flag = 1;
            break;
        }
    }
    if (!random_flag)
    {
        printf("Customer %d was not serviced due to unavailability of machines at %d\n", all_orders[*l].customer_number, time_from_start());
        return NULL;
    }
    printf("\033[36mMachine %d starts preparing ice cream %d of customer %d at %d second(s)\033[0m\n", which_machine[*l], all_orders[*l].order_number, all_orders[*l].customer_number, time_from_start());
    completed[*l] = 1;
    sleep(all_orders[*l].flavour_time);
    printf("\033[34mMachine %d completes preparing Icecream %d of Customer %d at %d second(s)\033[0m\n", which_machine[*l], all_orders[*l].order_number, all_orders[*l].customer_number, time_from_start());

    sem_post(&machines_sema[which_machine[*l]]);

    return NULL;
}

int main()
{
    int result, r1, r2, r3, r4, r5, r6, r7,r8;
    for (int i = 0; i < 100000; i++)
    {
        closed_machine[i] = 0;
    }
    for (int i = 0; i < 100000; i++)
    {
        customer_ing_remark[i] = 0;
        customer_print_remark[i] = 0;
        first_time_visit_customer[i] = 0;
        ghrwpasi[i] = 0;
        booked_machine[i] = 0; 
        // ghrwpasi[i] = 0;
    }

    result = pthread_mutex_init(&mutex, NULL);
    r1 = pthread_mutex_init(&mutex1, NULL);
    r2 = pthread_mutex_init(&ing_lock, NULL);
    r3 = pthread_mutex_init(&tp_lock, NULL);
    r4 = pthread_mutex_init(&un_lock, NULL);
    r5 = pthread_mutex_init(&first_time_visit_customer_lock, NULL);
    r6 = pthread_mutex_init(&lock_for_all_ingredient_check, NULL);
    r7 = pthread_mutex_init(&first_time_visit_customer_lock1, NULL);
    r8 = pthread_mutex_init(&booked_lock, NULL);

    // r6 =
    for (int i = 0; i < 100000; i++)
    {
        insufficient[i] = 0;
        completed[i] = 0;
        repeat_flag[i] = 0;
    }

    for (int i = 0; i < 10000; i++)
    {
        all_orders[i].number_of_toppings = 0;
        all_orders[i].toppings_order = (char **)malloc(sizeof(char *) * 50);
        for (int j = 0; j < 50; j++)
        {
            all_orders[i].toppings_order[j] = (char *)malloc(sizeof(char) * 4096);
        }
    }
    int k, f;
    scanf("%d %d %d %d", &n, &k, &f, &t);
    for (int i = 0; i < n; i++)
    {
        scanf("%d %d", &machines[i].start_time, &machines[i].end_time);
    }
    flavour flavours[f];
    for (int i = 0; i < f; i++)
    {
        flavours[i].name = (char *)malloc(sizeof(char) * 4096);
        scanf("%s %d", flavours[i].name, &flavours[i].time_flavour);
    }
    for (int i = 0; i < t; i++)
    {
        // toppings[i].topping_number = i;
        toppings[i].name = (char *)malloc(sizeof(char) * 4096);
        scanf("%s %d", toppings[i].name, &toppings[i].q_topping);
        // printf("gopaal -%d\n", toppings[i].q_topping);
        if (toppings[i].q_topping == -1)
        {
            toppings[i].q_topping = 20000;
        }
    }
    customer customers[10000];
    int current_cust;
    // printf("lol\n");
    int flagggg = 0;
    while (1)
    {
        char buffer[4000];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            break;
        }

        int arr, num_orders;
        char *token;
        const char *delimiter = " ";
        token = strtok(buffer, delimiter);
        int k = 0;
        while (token != NULL)
        {
            if (k == 0)
            {
                current_cust = atoi(token);
            }
            else if (k == 1)
            {
                arr = atoi(token);
            }
            else if (k == 2)
            {
                num_orders = atoi(token);
            }
            token = strtok(NULL, delimiter);
            all_orders[order_count].number_of_toppings++;
            k++;
        }
        int kkkkkkk = 1;
        while (num_orders--)
        {
            all_orders[order_count].arrival_time = arr;
            all_orders[order_count].customer_number = current_cust;
            all_orders[order_count].order_number = kkkkkkk++;
            if (num_orders < 0)
            {
                flagggg = 1;
                break;
            }
            char *temp111 = (char *)malloc(sizeof(char) * 4096);
            scanf("%s", temp111);
            int i;
            for (i = 0; i < f; i++)
            {
                if (strcmp(flavours[i].name, temp111) == 0)
                {
                    break;
                }
            }
            all_orders[order_count].flavour_time = flavours[i].time_flavour;
            char input[100000];
            char *token;
            const char *delimiter = " ";
            if (fgets(input, sizeof(input), stdin) != NULL)
            {
                token = strtok(input, delimiter);
                int k = 0;
                while (token != NULL)
                {
                    if (token[strlen(token) - 1] == '\n')
                    {
                        token[strlen(token) - 1] = '\0';
                    }
                    strcpy(all_orders[order_count].toppings_order[k], token);
                    token = strtok(NULL, delimiter);
                    all_orders[order_count].number_of_toppings++;
                    k++;
                }
            }

            order_count++;
        }
        if (flagggg == 1)
        {
            break;
        }
    }
    // printf("ttt - %d\n",order_count);
    pthread_t orders_threads[order_count];
    pthread_t machines_threads[n];
    int arg1[order_count];
    int arg2[n];
    // for (int i = 0; i < t; i++)
    // {
    //     printf("gopal %d\n",toppings[i].q_topping);
    // }

    for (int i = 0; i < order_count; i++)
    {
        arg1[i] = i;
        pthread_create(&orders_threads[i], NULL, (void *)order_handler, (void *)(arg1 + i));
    }
    for (int i = 0; i < n; i++)
    {
        arg2[i] = i;

        pthread_create(&machines_threads[i], NULL, (void *)machine_handler, (void *)(arg2 + i));
    }
    for (int i = 0; i < order_count; i++)
    {
        pthread_join(orders_threads[i], NULL);
    }
    for (int i = 0; i < n; i++)
    {
        pthread_join(machines_threads[i], NULL);
    }
    printf("Parlour Closed\n");

    return 0;
}