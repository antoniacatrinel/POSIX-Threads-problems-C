#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>

int digits = 0, letters = 0;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

void* f(void* arg)
{
    char* file = (char*)arg;
    FILE* fd = fopen(file, "r");
    char c;
    int d = 0, l = 0;
    while((c = fgetc(fd)) != EOF)
    {
        if (c >= '0' && c <= '9')
        {
            d++;
        }
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        {
            l++;
        }
    }
    pthread_mutex_lock(&m1);
    digits += d;
    pthread_mutex_unlock(&m1);
    pthread_mutex_lock(&m2);
    letters += l;
    pthread_mutex_unlock(&m2);
    fclose(fd);
    return NULL;
}

int main(int argc, char** argv)
{
    int n = argc - 1, i;
    pthread_t* t = malloc(n * sizeof(pthread_t));
    for (i = 1; i < argc; i++)
    {
        if (pthread_create(&t[i - 1], NULL, f,(void*)argv[i]) < 0)
        {
            perror("Unable to create thread!");
        }
    }

    for (i = 1; i < argc; i++)
    {
        pthread_join(t[i - 1], NULL);
    }

    printf("Total number of digits is %d.\n", digits);
    printf("Total number of letters is %d.\n", letters);
    pthread_mutex_destroy(&m1);
    pthread_mutex_destroy(&m2);
    free(t);
    return 0;
}
