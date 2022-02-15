#include "Server.hpp"
#include "../requestResponse/Request.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

std::string    runCgi()
{
    pid_t       pid;
    int         fd[2];
    int         r;
    char        buffer[1024];
    std::string str;
    std::string path_cgi;

    setenv("SCRIPT_FILENAME", "hello.py", true);
    setenv("REQUEST_METHOD", "GET", true);
    setenv("REDIRECT_STATUS", "0", true);
    setenv("GATEWAY_INTERFACE", "CGI/1.1", true);
    setenv("PATH_INFO", "/Users/mbelaman/Desktop/webserver/parsing/", true);
    // path_cgi = "/Users/mbelaman/goinfre/.brew/bin/php-cgi";
    path_cgi = "/usr/bin/python";

    extern char **environ;
    char **args = (char **)malloc(sizeof(char *) * 3);
    args[0] = strdup(path_cgi.c_str());
    args[1] = getenv("SCRIPT_FILENAME");
    args[2] = NULL;
    
    if (pipe(fd) == -1)
        std::cout << "Error" << std::endl;
    if ((pid = fork()) < 0)
        std::cout << "there is an error while calling" << std::endl;
    if (pid == 0)
    {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        if (execve(args[0], (char *const *)args, environ) < 0)
        {
            std::cout << "CGI NOT FOUND!" << std::endl;
            exit(1);
        }
    }
    else
    {
        close(fd[1]);
        while ((r = read(fd[0], buffer, sizeof(buffer))))
            str.append(buffer, r);
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





