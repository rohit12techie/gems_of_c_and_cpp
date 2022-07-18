#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>

typedef void (*fptr_callback)(int);

typedef struct callback_meta
{
    fptr_callback callback;
} callback_meta;

void *callback_thread(void *cbm)
{
    int l_data = 0;
    callback_meta* cb_meta = (callback_meta*)cbm;

    while (1)
    {
        printf("CallBack Job Running!! \n");
        usleep(10);
        l_data++;
        if(l_data % 10 == 0)
        {
            //Notify callback
            cb_meta->callback(l_data);
        }
        if(l_data == INT_MAX)
        {
            l_data = 0;
        }
    }
}

extern void create_call_back_thread(pthread_t *pthread_id, fptr_callback callback_fun)
{
    printf("In- create_call_back_thread \n");

    callback_meta *cbm = malloc(sizeof(callback_meta));
    cbm->callback = callback_fun;

    pthread_create(pthread_id, NULL, callback_thread, (void *)cbm);
    
    printf("Out- create_call_back_thread \n");
}