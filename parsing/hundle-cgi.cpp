#include "Server.hpp"
#include "../requestResponse/Request.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

std::string    runCgi()
{
    const char *args[] = {
        "/Users/mbelaman/goinfre/.brew/bin/php-cgi", NULL
    };

    // const char *env[] = {
    //     "SCRIPT_FILENAME=index.php",
    //     "REQUEST_METHOD=GET",
    //     "REDIRECT_STATUS=0",
    //     "PATH_INFO=/Users/mbelaman/Desktop/webserver/parsing/"
    // };
    pid_t       pid;
    int         fd[2];
    int         r;
    char        buffer[1024];
    std::string str;

    setenv("SCRIPT_FILENAME", "index.php", true);
    setenv("REQUEST_METHOD", "GET", true);
    setenv("REDIRECT_STATUS", "0", true);
    setenv("PATH_INFO", "/Users/mbelaman/Desktop/webserver/parsing/", true);

    extern char **environ;
    if (pipe(fd) == -1)
        std::cout << "Error" << std::endl;
    if ((pid = fork()) < 0)
        std::cout << "there is an error while calling" << std::endl;
    if (pid == 0)
    {
        // fd[1] = open("foo.txt", O_RDWR | O_CREAT, 777);
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        if (execve(args[0], (char *const *)args, environ) < 0)
        {
            std::cout << "CGI NOT FOUND!" << std::endl;
            return "";
        }
    }
    else
    {
        close(fd[1]);
        while ((r = read(fd[0], buffer, sizeof(buffer))))
        {
            str.append(buffer, r);
        }
    }
    close(fd[0]);
    return str.substr(str.find("\r\n\r\n"), str.length());
}


int main()
{
    std::string str = runCgi();
    std::cout << str << std::endl;
    return 0;
}





