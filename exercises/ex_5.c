#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>

typedef struct
{
    char* file;
    int n;
    char* c;
}arg_t;

void* f(void* a)
{
    arg_t arr = *(arg_t*)a;
    FILE* fd = fopen(arr.file, "r");

    int nth = arr.n, counter = 0;
    char ch;
    while(fscanf(fd, "%c", &ch) != EOF && counter < nth)
    {
        counter++;
    }
    if (counter == nth)
    {
        if (ch == arr.c[0])
            printf("In file %s, character from position %d is the same as given!\n", arr.file, nth);
        else
            printf("In file %s, character from position %d is NOT the same as given!\n", arr.file, nth);

    }
    else
    {
        printf("n is too large (or f is too small)\n");
    }
    fclose(fd);
    free(a);
    return NULL;
}

int main(int argc, char** argv)
{
    int nr = argc - 1, i;
    pthread_t* t = malloc(nr/3 * sizeof(pthread_t));

    for (i = 0; i < nr; i+= 3)
    {
        arg_t* a = malloc(sizeof(arg_t));
        a->file = argv[i + 1];
        a->n = atoi(argv[i + 2]);
        a->c = argv[i + 3];
        pthread_create(&t[i/3], NULL, f, (void*)a);
    }

    for (i = 0; i < nr; i += 3)
    {
        pthread_join(t[i/3], NULL);
    }
    free(t);
    return 0;
}
