#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static int pipe_fd1[2], pipe_fd2[2];

void INIT(void)
{
    if (pipe(pipe_fd1) < 0 || pipe(pipe_fd2) < 0)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
}

void WRITE_TO_PARENT(void)
{
    /* send parent a message through pipe */
    // TO DO
    char message[] = "Child send message to parent!";
    write(pipe_fd1[1], message, sizeof(message));
    printf("Child send message to parent!\n");
}

void READ_FROM_PARENT(void)
{
    /* read message sent by parent from pipe */
    // TO DO
    char message[100];
    read(pipe_fd2[0], message, sizeof(message));
    printf("Child receive message from parent: %s\n", message);
}

void WRITE_TO_CHILD(void)
{
    /* send child a message through pipe */
    // TO DO
    char message[] = "Parent send message to child!";
    write(pipe_fd2[1], message, sizeof(message));
    printf("Parent send message to child!\n");
}

void READ_FROM_CHILD(void)
{
    /* read the message sent by child from pipe */
    // TO DO
    char message[100];
    read(pipe_fd1[0], message, sizeof(message));
    printf("Parent receive message from child: %s\n", message);
}

int main(int argc, char *argv[])
{
    INIT();
    pid_t pid;
    pid = fork();
    // set a timer, process will end after 1 second.
    alarm(10);
    if (pid == 0)
    {
        while (1)
        {
            sleep(rand() % 2 + 1);
            WRITE_TO_CHILD();
            READ_FROM_CHILD();
        }
    }
    else
    {
        while (1)
        {
            sleep(rand() % 2 + 1);
            READ_FROM_PARENT();
            WRITE_TO_PARENT();
        }
    }
    return 0;
}
