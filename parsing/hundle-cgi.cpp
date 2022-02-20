#include "Server.hpp"
// #include "../requestResponse/Request.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

std::string    runCgi(Response response)
{
    pid_t       pid;
    int         fd[2];
    int         old_fd[2];
    int         fd_post[2];
    int         r;
    char        buffer[1024];
    std::string str;
    std::string path_cgi;

    std::string fullPath = response.get_filePath();
    std::string filename = fullPath.substr(fullPath.find_last_of("/") + 1);
    std::string req_method = response.getClientRequest().getMethod();

    //NOTE - The full path to the CGI script.
    setenv("SCRIPT_FILENAME", fullPath.c_str(), true);
    //NOTE - The name of the CGI script.
    setenv("SCRIPT_NAME", filename.c_str(), 1);
    //NOTE - This variable is specific to requests made with HTTP.
    setenv("REQUEST_METHOD", req_method.c_str(), true);
    //NOTE - If cgi.force_redirect is turned on, and you are not running under web servers, you may need to set an environment variable name that PHP will look for to know it is OK to continue execution.
    setenv("REDIRECT_STATUS", (std::to_string(response.getStatusCode())).c_str(), true);
    //NOTE - The version of the CGI specification to which this server complies
    setenv("GATEWAY_INTERFACE", "CGI/1.1", true);
    //NOTE - A path to be interpreted by the CGI script.
    setenv("PATH_INFO", response.getServer()->get_root().c_str(), true);
    //NOTE - The length of the query information. It is available only for POST requests.
    setenv("CONTENT_LENGTH", (std::to_string(response.getClientRequest().getContentLength())).c_str(), true);
    //NOTE - The data type of the content. Used when the client is sending attached content to the server. For example, file upload.
	if (response.getClientRequest().getContentType().find("multipart/form-data") != std::string::npos)
        setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", true);
    else
        setenv("CONTENT_TYPE", response.getClientRequest().getContentType().c_str(), true);
    std::cerr << getenv("CONTENT_TYPE") << "\n";
    //NOTE - The URL-encoded information that is sent with GET method request.
    setenv("QUERY_STRING", response.getClientRequest().getParam().c_str(), true);
    //NOTE - The server's hostname or IP Address
    setenv("SERVER_NAME", response.getServer()->get_host().c_str(), true);
    //NOTE - The port on which this request was received; appropriate as the port part of a URI.
    setenv("SERVER_PORT", response.getServer()->get_listen().c_str(), true);
    //NOTE - The name and revision of the information protocol this request came in with.
    setenv("SERVER_PROTOCOL", response.getClientRequest().getHttpVersion().c_str(), true);


    extern char **environ;
    char **args = new char*[3];
    args[1] = NULL;
    if (fullPath.find(".php") != std::string::npos)
    {
        path_cgi = response.get_location().get_cgi(); 
        args[0] = (char *)path_cgi.c_str();
    }
    else
    {
        path_cgi = "/usr/bin/python";
        args[0] = (char *)path_cgi.c_str();
        args[1] = getenv("SCRIPT_FILENAME");
    }
    args[2] = NULL;
    
    old_fd[0] = dup(0);
    old_fd[1] = dup(1);
    if (pipe(fd) == -1)
        std::cout << "Error" << std::endl;
    if (pipe(fd_post) == -1)
        std::cout << "Error" << std::endl;
    write(fd_post[1], response.getClientRequest().getQueryString().c_str(), response.getClientRequest().getQueryString().size());
    if ((pid = fork()) < 0)
        std::cout << "there is an error while calling" << std::endl;
    if (pid == 0)
    {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        dup2(fd_post[0], STDIN_FILENO);
        close(fd_post[0]);
        close(fd_post[1]);
        std::cerr << "\n++++++" << args[0] << "++++++" << response.get_filePath() << "+++++++++\n";
        if (execve(args[0], (char *const *)args, environ) < 0)
        {
            std::cout << "CGI NOT FOUND!" << std::endl;
            exit(1);
        }
    }
    else
    {
        close(fd_post[1]);
        close(fd_post[0]);
        close(fd[1]);
        wait(&pid);
        dup2(old_fd[1], STDOUT_FILENO);
        dup2(old_fd[0], STDIN_FILENO);
        while ((r = read(fd[0], buffer, sizeof(buffer))))
            str.append(buffer, r);
        close(fd[0]);
        close(old_fd[1]);
        close(old_fd[0]);
    }
    return str.substr(str.find("\r\n\r\n"), str.length());
}




