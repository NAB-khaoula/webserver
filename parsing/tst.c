
#include<stdio.h>
#include <unistd.h>
#include <fcntl.h>

int runCgi()
{
    int fds1[2];
    int fds2[2];
    char *args[] = {"/Users/mbelaman/goinfre/.brew/bin/php-cgi", NULL};
    char *env[] = {"SCRIPT_FILENAME=/Users/mbelaman/Desktop/webserver/parsing/index.php", "REQUEST_METHOD=GET","REDIRECT_STATUS=?", NULL};

    pipe(fds1);
    pipe(fds2);
    int pid = fork();
    if (!pid)
    {
        // dup2(fds1[0], 0);
        // // write pipe
        // close(fds2[0]);
        // dup2(fds2[1], 1);
        // close(fds1[1]);
        // close(fds1[0]);
        execve(args[0], args, env);
        return(0);
    }
    else
    {
        // close(fds1[0]);
        // close(fds1[1]);
        // close(fds2[1]);
        waitpid(pid, 0, 0);
    }
    return (fds2[0]);
}
int main()
{
    // char buff[1024];
    int fd = runCgi();
    // read(fd, buff, 1024);
    // printf("|%s|\n", buff);
}