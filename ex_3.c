#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>

int global = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

typedef struct
{
    char* file;
    int n;
}arg_t;

void* f(void* a)
{
    arg_t arr = *(arg_t*)a;
    FILE* fd = fopen(arr.file, "r");
    int count = arr.n, number = 0, sum = 0;

    while(fscanf(fd, "%d", &number) != EOF)
    {
        if (count && number % 2 == 0)
        {
            sum += number;
            count--;
        }
        if (count == 0)
            break;
    }

    pthread_mutex_lock(&m);
    global += sum;
    pthread_mutex_unlock(&m);
    free(a);
    fclose(fd);
    return NULL;
}

int main(int argc, char** argv)
{
    if (argc < 3 || argc % 2 == 0)
    {
        perror("Incorrect number of arguments!");
        exit(1);
    }

    int i, nr = argc - 1;
    pthread_t* t = malloc((nr/2) * sizeof(pthread_t));
    for (i = 0; i < nr; i += 2)
    {
        arg_t* a = malloc(sizeof(arg_t));
        a->file = argv[i + 1];
        a->n = atoi(argv[i + 2]);
        if (pthread_create(&t[i/2], NULL, f, (void*)a) < 0)
        {
            perror("Unable to create thread!");
            exit(2);
        }
    }
    for (i = 0; i < nr; i += 2)
    {
        pthread_join(t[i/2], NULL);
    }
    printf("Global sum is %d.\n", global);
    pthread_mutex_destroy(&m);
    free(t);
    return 0;
}


