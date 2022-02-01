#include "Server.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

std::string    runCgi()
{
    const char *args[] = {
        "/Users/mbelaman/goinfre/.brew/bin/php-cgi", NULL
    };

    const char *env[] = {
        "SCRIPT_FILENAME=/Users/mbelaman/Desktop/webserver/parsing/index.php", 
        "REQUEST_METHOD=GET", 
        "SERVER_PROTOCOL=\"HTTP/1.1\"", 
        "PATH_INFO=/Users/mbelaman/Desktop/webserver",
        "REDIRECT_STATUS=0"
    };
    pid_t       pid;
    int         fd[2];
    char        buffer[1024];
    std::string str;

    if (pipe(fd) == -1)
        std::cout << "Error" << std::endl;
    if ((pid = fork()) < 0)
        std::cout << "there is an error while calling" << std::endl;
    if (pid == 0)
    {
        // fd[1] = open("foo.txt", O_RDWR | O_CREAT, 777);
        dup2(fd[1], STDOUT_FILENO);
        if (execve(args[0], (char *const *)args, (char *const *)env) < 0)
        {
            std::cout << "CGI NOT FOUND!" << std::endl;
            return "";
        }
    }
    else
    {
        close(fd[1]);
        int r;
        while ((r = read(fd[0], buffer, sizeof(buffer))))
        {
            str.append(buffer, r);
        }
    }
    close(fd[0]);
    return str;
}


int main()
{
    std::string str = runCgi();
    std::cout << str << std::endl;
    return 0;
}





