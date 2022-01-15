#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cstring>
#include "Location.hpp"

class Parser;

class Server
{
private:
    std::string                           _listen;
    std::string                           _host;
    std::map<std::string, std::string>    _server_names;
    std::string                           _client_max_body;
    std::map<int, std::string>            _err_pages;
    std::string                           _root;
    std::vector<Location>                 _location;
    int                                   _brace_server;
    int                                   _brace_location;
public:
    //NOTE - Constructor
    Server();

    //NOTE - Getters
    std::string                                 get_root();
    std::string                                 get_listen();
    std::string                                 get_host();
    std::map<std::string, std::string>          &get_server_names();
    std::string                                 get_client_max_body();
    std::map<int, std::string>                  &get_err_pages();
    std::vector<Location>                       &get_location();
    int                                         get_brace_server();
    int                                         get_brace_location();

    
    //NOTE - Setters
    void                            set_root(std::string, int &);
    void                            set_listen(std::string, int &);
    void                            set_host(std::string, int &);
    void                            set_server_names(std::string, int &);
    void                            set_client_max_body(std::string, int &);
    void                            set_err_pages(std::string, int &);
    void                            set_location(Location &);
    void                            set_brace_server(int);
    void                            set_brace_location(int);
    void                            clear();
    //NOTE - Destructor
    ~Server();
};

std::string                 get_key(std::string &, int &);
std::string                 get_value(std::string &, int &);
std::vector<std::string>    ft_splitSpace(std::string str, char c);
std::vector<std::string>    delete_brace(std::string str);
void                        fill_server(std::string, std::string, std::vector<Server> &, Server &, Location &, int &);
void                        fill_location(std::string &, std::string &, Location &, Server &, int &);
void                        print_attr(std::vector<Server> &vec_serv);
std::vector<Server>         begin_parser();
#endif