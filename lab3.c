#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#define NUM_THREADS 5
#define N 10000000

void get_info();
void *blank(void *arg);
void *adder(void *arg);

int main(int argc, char *argv[])
{
    int i;
    int policy;
    time_t startS, endS;
    long startMS, endMS;
    long diff_in_sec;
    struct sched_param param;
    pthread_attr_t attr;
    pthread_t curr;
    struct timeval tv;

    pthread_attr_init(&attr);



    printf("Before adjustments to scheduling policy:\n");
    curr = pthread_self();

    get_info(&curr);
    pthread_t threads[NUM_THREADS];
    int *arr = (int*)calloc(sizeof(int), NUM_THREADS);
    pthread_getschedparam(pthread_self(), &policy, &param);

    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    param.sched_priority = 99;
    pthread_attr_setschedparam(&attr, &param);

    pthread_create(&threads[0], &attr, blank , NULL);
    get_info(&threads[0]);
    pthread_join(threads[0], NULL);

    gettimeofday(&tv, 0);
    startMS = tv.tv_usec;
    startS = tv.tv_sec;

    for(i = 0; i < NUM_THREADS; i++)
    {
        arr[i] = i+1;
        pthread_create(&threads[i], &attr, adder, (void*)&arr[i]);
    }

    for(i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&tv, 0);
    endMS = tv.tv_usec;
    endS = tv.tv_sec;

    diff_in_sec = endS - startS;

    printf("\n\nElapsed time FOR THREADED TEST=%ld microseconds\n\n", (endMS - startMS) + (diff_in_sec * 1000000));

    gettimeofday(&tv, 0);
    startMS = tv.tv_usec;
    startS = tv.tv_sec;

    for(i = 0; i < NUM_THREADS; i++)
    {
        adder(&arr[i]);
    }

    gettimeofday(&tv, 0);
    endMS = tv.tv_usec;
    endS = tv.tv_sec;

    diff_in_sec = endS - startS;

    printf("\n\nElapsed time for SEQUENTIAL TEST=%ld microseconds\n\n", (endMS - startMS) + (diff_in_sec * 1000000));


    free(arr);
    return 0;
}

void *blank(void *arg)
{
    return NULL;
}

void *adder(void *arg)
{

    printf("%d: begin\n", *((int*)arg));

    int i, sum;
    for(i = 1; i < N; i++)
    {
        sum += i;
    }
    printf("%d: Counter: %d\n", *((int*)arg), i);
    printf("%d: done\n", *((int*)arg));
    return NULL;
}

void get_info(pthread_t *t)
{
    pthread_attr_t attr;
    int detach;
    int scope;
    int inherit;
    int policy;
    size_t guard;
    void *base;
    size_t stack_size;
    struct sched_param param;

    pthread_attr_init(&attr);

    pthread_getschedparam(*t, &policy, &param);

    switch (policy)
    {
        case SCHED_OTHER:
            printf ("Pthread Policy is SCHED_OTHER\n");
            break;
        case SCHED_RR:
            printf ("Pthread Policy is SCHED_PR\n");
            break;
        case SCHED_FIFO:
            printf ("Pthread Policy is SCHED_FIFO\n");
            break;
        case -1:
            perror ("sched_getscheduler returns error");
            break;
        default:
            fprintf (stderr, "Unknown policy!\n");
    }

    pthread_attr_getdetachstate(&attr, &detach);

    switch (detach)
    {
        case PTHREAD_CREATE_JOINABLE:
            printf ("\tDetach state\t\t= PTHREAD_CREATE_JOINABLE\n");
            break;
        case PTHREAD_CREATE_DETACHED:
            printf ("\tDetach state\t\t= PTHREAD_CREATE_DETACHED\n");
            break;
        case -1:
            perror ("\tattr_getdeatchstate returns error");
            break;
        default:
            fprintf (stderr, "\tUnknown detach!\n");
    }

    pthread_attr_getscope(&attr, &scope);

    switch (scope)
    {
        case PTHREAD_SCOPE_SYSTEM:
            printf ("\tScope\t\t\t\t= PTHREAD_SCOPE_SYSTEM\n");
            break;
        case PTHREAD_SCOPE_PROCESS:
            printf ("\tScope\t\t\t\t= PTHREAD_SCOPE_PROCESS\n");
            break;
        case -1:
            perror ("\tattr_getscope returns error");
            break;
        default:
            fprintf (stderr, "\tUnknown scope!\n");
    }

    pthread_attr_getinheritsched(&attr, &inherit);

    switch (inherit)
    {
        case PTHREAD_INHERIT_SCHED:
            printf ("\tInherit schedule\t= PTHREAD_INHERIT_SCHED\n");
            break;
        case PTHREAD_EXPLICIT_SCHED:
            printf ("\tInherit schedule\t= PTHREAD_EXPLICIT_SCHED\n");
            break;
        case -1:
            perror ("\tattr_getinheritsched returns error");
            break;
        default:
            fprintf (stderr, "\tUnknown inherit scheduler!\n");
    }

    printf("\tScheduling Priority\t= %d\n", param.sched_priority);
    pthread_attr_getguardsize(&attr, &guard);
    printf("\tGuard size\t\t\t= %d size\n", (int)guard);
    pthread_attr_getstackaddr(&attr, &base);
    printf("\tStack Address\t\t= %p\n", base);
    pthread_attr_getstacksize(&attr, &stack_size);
    printf("\tStack Size\t\t\t= %lu bytes\n", stack_size);
}
