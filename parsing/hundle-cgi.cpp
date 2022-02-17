#include "Server.hpp"
// #include "../requestResponse/Request.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

std::string    runCgi(Response response)
{
    pid_t       pid;
    int         fd[2];
    int         r;
    char        buffer[1024];
    std::string str;
    std::string path_cgi;


    std::string filename = response.get_filePath();
    std::cout << "|" + filename + "|" << std::endl;
    //NOTE - The full path to the CGI script.
    setenv("SCRIPT_FILENAME", filename.c_str(), true);
    //NOTE - This variable is specific to requests made with HTTP.
    setenv("REQUEST_METHOD", "GET", true);
    //NOTE - If cgi.force_redirect is turned on, and you are not running under web servers, you may need to set an environment variable name that PHP will look for to know it is OK to continue execution.
    setenv("REDIRECT_STATUS", "200", true);
    //NOTE - The version of the CGI specification to which this server complies
    setenv("GATEWAY_INTERFACE", "CGI/1.1", true);
    //NOTE - A path to be interpreted by the CGI script.
    setenv("PATH_INFO", "/Users/mbelaman/Desktop/webserver/parsing/", true);
    //NOTE - The length of the query information. It is available only for POST requests.
    setenv("CONTENT_LENGTH", "", true);
    //NOTE - The data type of the content. Used when the client is sending attached content to the server. For example, file upload.
    setenv("CONTENT_TYPE", "", true);
    //NOTE - The URL-encoded information that is sent with GET method request.
    setenv("QUERY_STRING", "", true);
    //NOTE - The server's hostname or IP Address
    setenv("SERVER_NAME", "", true);
    //NOTE - The port on which this request was received; appropriate as the port part of a URI.
    setenv("SERVER_PORT", "", true);
    //NOTE - The name and revision of the information protocol this request came in with.
    setenv("SERVER_PROTOCOL", "", true);
    //NOTE - The name and version of the information server software answering the request (and running the gateway).
    setenv("SERVER_SOFTWARE", "", true);

    // path_cgi = "/Users/mbelaman/goinfre/.brew/bin/php-cgi";
    path_cgi = "/usr/bin/python";

    extern char **environ;
    char **args = new char*[3];
    args[0] = (char *)path_cgi.c_str();
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





