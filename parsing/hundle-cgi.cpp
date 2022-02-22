#include "Server.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>


// #include  <unistd.h>
// #include <stdio.h>


// #define PATH_MAX 4096
// int main() {
//    char cwd[PATH_MAX];
//    if (getcwd(cwd, sizeof(cwd)) != NULL) {
//        printf("Current working dir: %s\n", cwd);
//    } else {
//        perror("getcwd() error");
//        return 1;
//    }
//    return 0;
// }

std::string    runCgi(Response &response)
{
    pid_t       pid;
    int         fd[2];
    int         old_fd[2];
    int         fd_post[2];
    int         r;
    char        buffer[1024];
    std::string str;
    std::cout << "|" << response.get_location().get_cgi() << std::endl;
    std::string fullPath = response.get_filePath();
    std::string filename = fullPath.substr(fullPath.find_last_of("/") + 1);
    std::string req_method = response.getClientRequest().getMethod();
    std::string path_cgi_php = response.get_location().get_cgi();
    if (!path_cgi_php.empty())
    {
        path_cgi_php = response.get_location().get_cgi();
        std::cout << "3amar " << path_cgi_php << std::endl;
    }
    else
    {
        std::cout << "khawi " << path_cgi_php << std::endl;
        throw std::runtime_error("Path CGI not found");
    }
    std::string path_cgi_py = "/usr/bin/python";


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
    setenv("CONTENT_TYPE", response.getClientRequest().getContentType().c_str(), true);
    //NOTE - The URL-encoded information that is sent with GET method request.
    setenv("QUERY_STRING", response.getClientRequest().getParam().c_str(), true);
    //NOTE - The server's hostname or IP Address
    setenv("SERVER_NAME", response.getServer()->get_host().c_str(), true);
    //NOTE - The port on which this request was received; appropriate as the port part of a URI.
    setenv("SERVER_PORT", response.getServer()->get_listen().c_str(), true);
    //NOTE - The name and revision of the information protocol this request came in with.
    setenv("SERVER_PROTOCOL", response.getClientRequest().getHttpVersion().c_str(), true);
    //NOTE - Returns the set cookies in the form of key & value pair.
    setenv("HTTP_COOKIE", response.getClientRequest().getParam().c_str(), true);
    
    extern char **environ;
    char **args = new char*[3];
    args[1] = NULL;
    if (fullPath.find(".php") != std::string::npos)
    {
        args[0] = (char *)path_cgi_php.c_str();
        args[1] = (char *)fullPath.c_str();
    }
    else
    {
        args[0] = (char *)path_cgi_py.c_str();
        args[1] = getenv("SCRIPT_FILENAME");
    }
    args[2] = NULL;
    
    old_fd[0] = dup(0);
    old_fd[1] = dup(1);

    if (pipe(fd) == -1)
        throw std::runtime_error("Pipe Failed!");
    if (pipe(fd_post) == -1)
        throw std::runtime_error("Pipe Failed!");

    if ((pid = fork()) < 0)
        throw std::runtime_error("There is an error while calling");
    if (pid == 0)
    {
        dup2(fd[1], STDOUT_FILENO);
        dup2(fd_post[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        close(fd_post[0]);
        close(fd_post[1]);
        if (execve(args[0], (char *const *)args, environ) < 0)
        {
            throw std::runtime_error("CGI NOT FOUND!");
            exit(1);
        }
    }
    else
    {
        if (req_method == "POST")
            write(fd_post[1], response.getClientRequest().getQueryString().c_str(), response.getClientRequest().getQueryString().size());
        close(fd[1]);
        close(fd_post[0]);
        close(fd_post[1]);
        while ((r = read(fd[0], buffer, sizeof(buffer))))
            str.append(buffer, r);
        close(fd[0]);
        wait(&pid);
        dup2(old_fd[1], STDOUT_FILENO);
        dup2(old_fd[0], STDIN_FILENO);
        close(old_fd[1]);
        close(old_fd[0]);
    }
    response.setCgiHeaders(str.substr(0, str.find("\r\n\r\n")));
    return str.substr(str.find("\r\n\r\n") + 4, str.length());
}




