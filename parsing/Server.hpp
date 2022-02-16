#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cstring>
#include "Location.hpp"
#include "../WebServ.hpp"
// #include "../WebServ.hpp"

struct ws;

class Response;
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
    std::map<std::string, Location>       _map_loc;
    int                                   _brace_server;
    int                                   _brace_location;
    bool                                  _df;
public:
    //NOTE - Constructor
    Server();

    //NOTE - Getters
    std::string                                 get_root();
    std::string                                 get_listen();
    std::string                                 get_host();
    std::map<std::string, std::string>          &get_server_names();
    std::map<std::string, std::string>          &get_ports();
    std::string                                 get_client_max_body();
    std::map<int, std::string>                  &get_err_pages();
    std::map<std::string, Location>             &get_map_loc();
    std::vector<Location>                       &get_location();
    int                                         get_brace_server();
    int                                         get_brace_location();
    bool                                        get_df();

    
    //NOTE - Setters
    void                            set_root(std::string, int &);
    void                            set_listen(std::string, struct ws &, int &);
    void                            set_host(std::string, int &);
    void                            set_server_names(std::string, int &);
    void                            set_client_max_body(std::string, int &);
    void                            set_err_pages(std::string, int &);
    void                            set_location(Location &);
    void                            set_map_loc(Location &, int &);
    void                            set_brace_server(int);
    void                            set_brace_location(int);
    void                            set_df(bool);
    void                            clear();
    //NOTE - Destructor
    ~Server();
};

std::string                 get_key(std::string &, int &);
std::string                 get_value(std::string &, int &);
std::vector<std::string>    ft_splitSpace(std::string, char);
std::vector<std::string>    delete_brace(std::string );
void                        check_braces(std::string &, struct ws &, int &);
void                        check_semi(std::string, int &);
void                        errors(int, int &, std::string);
bool                        isNumber(std::string s);
int                         check_directive_loc(std::string);
int                         check_directives(std::string);
std::string                 &rightTrim(std::string& str);
std::string                 &leftTrim(std::string& str);
void                        fill_server(std::string, std::string, std::string &, struct ws &, int &);
void                        fill_location(std::string &, std::string &, struct ws &, int &);
void                        print_attr(struct ws &);
void                        begin_parser(struct ws &);
std::string                 runCgi(Response);
#endif