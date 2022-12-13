#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>

int* globals;
int* signals;

typedef struct
{
    int id;
    char* file;
}arg_t;

pthread_mutex_t* m;
pthread_cond_t* cond;

void* x(void* arg)
{
    arg_t a = *(arg_t*)arg;
    pthread_mutex_lock(&m[a.id]);
    char* f = a.file;
    FILE* fd = fopen(f, "r");

    int v = 0, w = 0;
    fscanf(fd, "%d", &v);
    fscanf(fd, "%d", &w);

    while(globals[a.id] + v <= w)
    {
        globals[a.id] += w;
    }

    printf("Thread %d is sending signal with global %d.\n", a.id, globals[a.id]);
    signals[a.id] = 1;
    pthread_cond_signal(&cond[a.id]);
    fclose(fd);
    pthread_mutex_unlock(&m[a.id]);
    return NULL;
}

void* y(void* arg)
{
    arg_t a = *(arg_t*)arg;
    pthread_mutex_lock(&m[a.id]);

    printf("Thread %d is waiting for signal.\n", a.id);
    while(signals[a.id] == 0)
    {
        pthread_cond_wait(&cond[a.id], &m[a.id]);
    }
    printf("Global for thread %d is %d.\n", a.id, globals[a.id]);

    free(arg);
    pthread_mutex_unlock(&m[a.id]);
    return NULL;
}


int main(int argc, char** argv)
{
    int i, n;
    n = argc - 1;
    m = malloc(n * sizeof(pthread_mutex_t));
    cond = malloc(n * sizeof(pthread_cond_t));
    signals = malloc(n * sizeof(int));
    globals = malloc(n * sizeof(int));
    pthread_t* t = malloc(2 * n * sizeof(pthread_t));

    for(i = 0; i < n; i++)
    {
        pthread_mutex_init(&m[i], NULL);
        pthread_cond_init(&cond[i], NULL);
        arg_t* a = malloc(sizeof(arg_t));
        a->id = i;
        a->file = argv[i + 1];
        globals[i] = 0;
        signals[i] = 0;
        if (pthread_create(&t[i], NULL, x, (void*)a) < 0)
        {
            perror("Unable to create thread!");
            exit(1);
        }
        if (pthread_create(&t[i + n], NULL, y, (void*)a) < 0)
        {
            perror("Unable to create thread!");
            exit(1);
        }
    }

    for(i = 0; i < n; i++)
    {
        pthread_join(t[i], NULL);
        pthread_join(t[i + n], NULL);
    }

    for(i = 0; i < n; i++)
    {
        pthread_mutex_destroy(&m[i]);
        pthread_cond_destroy(&cond[i]);
    }

    free(m);
    free(cond);
    free(signals);
    free(globals);
    free(t);
    return 0;
}

