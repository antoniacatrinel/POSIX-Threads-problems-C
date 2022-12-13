#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int global = 0;
pthread_mutex_t *m;

typedef struct
{
    int n;
    int prev;
    int next;
    int id;
}arg_t;

void* f(void* arg)
{
    arg_t arr = *(arg_t*)arg;
    while (1)
    {
        pthread_mutex_lock(&m[arr.id]);

        if(global >= 10000)
        {
            pthread_mutex_unlock(&m[arr.next]);
            break;
        }

        global += arr.n;
        if (global % 2 == 0)
        {
            printf("Global %d is even at the end of thread %d, starting next with id %d.\n", global, arr.id, arr.next);
            pthread_mutex_unlock(&m[arr.next]);
        }
        else
        {
            printf("Global %d is odd at the end of thread %d, starting prev with id %d.\n", global, arr.id, arr.prev);
            pthread_mutex_unlock(&m[arr.next]);
        }
    }
    free(arg);
    return NULL;
}

int main(int argc, char** argv)
{
    int nr = argc - 1, i;
    pthread_t* t = malloc((nr/3) * sizeof(pthread_t));
    m = malloc((nr/3) * sizeof(pthread_mutex_t));

    for(i = 0; i < nr; i += 3)
    {
        pthread_mutex_init(&m[i/3], NULL);
        pthread_mutex_lock(&m[i/3]);

        arg_t* a = malloc(sizeof(arg_t));
        a->n = atoi(argv[i+1]);
        a->next = atoi(argv[i+2]);
        a->prev = atoi(argv[i+3]);
        a->id = i/3;
        if (pthread_create(&t[i/3], NULL, f, (void*)a) < 0)
        {
            perror("Unable to create thread!");
            exit(1);
        }
    }

    pthread_mutex_unlock(&m[0]);

    for(i = 0; i < nr; i += 3)
    {
        pthread_join(t[i/3], NULL);
        pthread_mutex_destroy(&m[i/3]);
    }

    printf("Final global is %d.\n", global);

    free(m);
    free(t);
    return 0;
}

