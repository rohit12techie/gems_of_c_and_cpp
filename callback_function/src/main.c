#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void on_value_divided_by_ten(int val)
{
    printf("This value is divided by 10 (%d).\n", val);
}

int main()
{
    printf("In Main ---\n");

    int x = 10;
    pthread_t update_thread;

    // Creating a callback function - which will call on_value_divided_by_ten.
    create_call_back_thread(&update_thread, on_value_divided_by_ten);

    while(1)
    {
        usleep(100);
        printf("Some other job is running\n");
    }

    pthread_join(update_thread, NULL);
    return 0;
}