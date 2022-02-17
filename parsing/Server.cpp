#include "Server.hpp"

Server::Server()
{
    _root  = "";
    _listen = "";
    _host   = "";
    _client_max_body = "";
    _brace_server = 0;
    _brace_location = 0;
    _df = false;
}

std::string                             Server::get_root(){return _root;}
std::string                             Server::get_listen(){return _listen;}
std::string                             Server::get_host(){return _host;}
std::map<std::string, std::string>      &Server::get_server_names(){return  _server_names;}
std::string                             Server::get_client_max_body(){return _client_max_body;}
std::map<int, std::string>              &Server::get_err_pages(){return _err_pages;}
std::vector<Location>                   &Server::get_location() {return _location;}
std::map<std::string, Location>         &Server::get_map_loc() {return _map_loc;}
int                                     Server::get_brace_server() {return _brace_server;}
int                                     Server::get_brace_location() {return _brace_location;}
bool                                    Server::get_df() {return _df;}

void    Server::set_df(bool b) {_df = b;}

void    Server::set_root(std::string root, int &nb_line)
{
    if (this->get_root().empty())
    {
        root = rightTrim(root);
        if (root.find(" ") != std::string::npos || root.find("\t") != std::string::npos)
            errors(10, nb_line, "root");
        else
            _root = root;
    }
    else
        errors(24, nb_line, root);
}

void    Server::set_listen(std::string listen, t_WebServ &ws, int &nb_line)
{
    if (this->get_listen().empty())
    {
        listen = rightTrim(listen);
        int i = 0, len = 0;
        if (listen == "")
            errors(5, nb_line, listen);
        while (listen[i])
        {
            if (isdigit(listen[i]) == false)
                errors(4, nb_line, listen);
            if (listen.length() > 9)
                errors(5, nb_line, listen);
            i++;
            len++;
        }
        _listen = listen;
        if (ws.ports.find(_listen) == ws.ports.end())
        {
            _df = true;
            ws.ports.insert(std::make_pair(_listen, stoi(_listen)));
        }
        else
            _df = false;
    }
    else
        errors(21, nb_line, listen);
}
void    Server::set_host(std::string host, int &nb_line)
{
    if (this->get_host().empty())
    {
        host = rightTrim(host);
        if (!host.compare("localhost"))
            _host = "localhost";
        else
        {
            std::vector<std::string> vec = ft_splitSpace(host, '.');
            if (vec.size() == 4)
            {
                for (size_t i = 0; i < vec.size(); i++)
                {
                    if (vec[i].empty() || isNumber(vec[i]) == false)
                        errors(6, nb_line, host);
                    int nb = atoi(vec[i].c_str());
                    if (nb >= 0 && nb <= 255)
                    {
                        if (i != vec.size() - 1)
                            _host += std::to_string(nb) + ".";
                        else
                            _host += std::to_string(nb);
                    }
                    else
                    {
                        std::string str = std::to_string(nb);
                        errors(6, nb_line, str);
                    }
                }
            }
            else
                errors(6, nb_line, host);
        }
    }
    else
        errors(22, nb_line, host);
}
void    Server::set_server_names(std::string value, int &nb_line)
{
    if (value.empty())
        errors(7, nb_line, "server_name");
    std::vector<std::string> vec = ft_splitSpace(value, ' ');
    for (size_t i = 0; i < vec.size(); i++)
    {
        if (_server_names.find(vec[i]) == _server_names.end())
            _server_names.insert(std::make_pair(vec[i], vec[i]));
    }

}
void    Server::set_client_max_body(std::string client_max_body, int &nb_line)
{
    if (this->get_client_max_body().empty())
    {
            client_max_body = rightTrim(client_max_body);
        int i = 0;
        int check = 0;
        while (client_max_body[i])
        {
            if (isdigit(client_max_body[i]))
                i++;
            else if ((client_max_body[i] == 'm' || client_max_body[i] == 'M' || client_max_body[i] == 'k' || client_max_body[i] == 'K' || client_max_body[i] == 'g' || client_max_body[i] == 'G') &&
                    !check && isdigit(client_max_body[i - 1]) && client_max_body[i + 1] == '\0')
            {
                check++;
                i++;
            }
            else
                errors(8, nb_line, client_max_body);
        }
        if (!check)
            errors(8, nb_line, client_max_body);
        else
        {
            i = 0;
            if ((i = client_max_body.find("m")) != std::string::npos || (i = client_max_body.find("M")) != std::string::npos)
            {
                client_max_body = client_max_body.substr(0, i);
                size_t nb = (long long)atoi(client_max_body.c_str()) * 1048576;
                _client_max_body = std::to_string(nb);
            }
            else if ((i = client_max_body.find("k")) != std::string::npos || (i = client_max_body.find("K")) != std::string::npos)
            {
                client_max_body = client_max_body.substr(0, i);
                size_t nb = (long long)atoi(client_max_body.c_str()) * 1024;
                _client_max_body = std::to_string(nb);
            }
            else
            {
                client_max_body = client_max_body.substr(0, i);
                size_t nb = (long long)atoi(client_max_body.c_str()) * 1073741824;
                _client_max_body = std::to_string(nb);
            }
        }   
    }
    else
        errors(23, nb_line, client_max_body);
}
void    Server::set_err_pages(std::string value, int &nb_line)
{
    value = rightTrim(value);
    size_t pos = 0;
    if ((pos = value.find(" ")) != std::string::npos)
    {
        std::string f_str = value.substr(0, pos);
        int key;
        if (isNumber(f_str))
            key = atoi(f_str.c_str());
        else
            errors(9, nb_line, f_str); 
        int i = 0;
        std::string s_str = value.erase(0, pos + 1);
        while (s_str[i])
        {
            if ((s_str[i] == ' ' || s_str[i] == '\t'))
                errors(9, nb_line, s_str);
            i++;
        }
        if (_err_pages.find(key) == _err_pages.end())
            _err_pages[key] = s_str;
    }
    else
        errors(9, nb_line, value);
}

void    Server::set_location(Location &locat)
{
    _location.push_back(locat);
}

void    Server::set_map_loc(Location &locat, int &nb_line)
{
    if (_map_loc.find(locat.get_path()) == _map_loc.end())
    {
        locat.set_match(true);
        _map_loc[locat.get_path()] = locat;
    }
    else
        errors(20, nb_line, locat.get_path());
}

void    Server::set_brace_server(int brace_server) {_brace_server = brace_server;}
void    Server::set_brace_location(int brace_location) {_brace_location = brace_location;}

void fill_location(std::string &key, std::string &value, t_WebServ &ws, int &nb_line)
{
    if (!key.compare("autoindex"))
        ws.locat.set_autoindex(value, nb_line);
    else if (!key.compare("index"))
        ws.locat.set_index(value, nb_line);
    else if (key == "allow_methods")
        ws.locat.set_methods(value, nb_line);
    else if (!key.compare("return"))
        ws.locat.set_return(value, nb_line);
    else if (!key.compare("fastcgi_pass"))
        ws.locat.set_cgi(value, nb_line);
    else if (!key.compare("upload_enable"))
        ws.locat.set_upload_enble(value, nb_line);
    else if (!key.compare("upload_store"))
        ws.locat.set_upload(value, nb_line);
    else
        errors(3, nb_line, key);

}

void    fill_server(std::string key, std::string value, std::string &line, t_WebServ &ws, int &nb_line)
{
    if (ws.serv->get_brace_server() == 2)
    {
        if (!key.compare("listen"))
            ws.serv->set_listen(value, ws, nb_line);
        else if (!key.compare("host"))
            ws.serv->set_host(value, nb_line);
        else if (!key.compare("server_name"))
            ws.serv->set_server_names(value, nb_line);
        else if (!key.compare("client_max_body_size"))
            ws.serv->set_client_max_body(value, nb_line);
        else if (!key.compare("error_page"))
            ws.serv->set_err_pages(value, nb_line);
        else if (!key.compare("root"))
            ws.serv->set_root(value, nb_line);
        else if (line.find("}") != std::string::npos && ws.serv->get_brace_location() == 2)
        {
            ws.serv->set_brace_location(0);
            ws.serv->set_location(ws.locat);
            ws.serv->set_map_loc(ws.locat, nb_line);
            ws.locat.clear();
        }
        else if (line.find("}") != std::string::npos && ws.serv->get_brace_server() == 2)
        {
            ws.serv->set_brace_server(0);
            if (ws.serv->get_listen().empty() || ws.serv->get_host().empty() || ws.serv->get_root().empty())
                errors(30, nb_line, "");
            ws.servers.push_back(*ws.serv);
            delete ws.serv;
            ws.serv = new Server();
        }
        else if (ws.serv->get_brace_location() == 2)
            fill_location(key, value, ws, nb_line);
        else if (!check_directive_loc(key) && ws.serv->get_brace_location() == 1)
            errors(11, nb_line, key);
        else
            errors(3, nb_line, key);
    }
    else if ((!check_directives(key) || !check_directive_loc(key)) && ws.serv->get_brace_location() != 2)
        errors(33, nb_line, "");
    else
        errors(2, nb_line, "");
}

void     begin_parser(t_WebServ &ws, char **av)
{
    std::ifstream               myReadFile;
    std::vector<std::string>    split;
    std::string                 line, str_key, str_value;
    int i = 0, j = 0, k;

    ws.serv = new Server();
    myReadFile.open("./src/webserv.conf");
    if (myReadFile.is_open())
    {
        while (std::getline(myReadFile, line))
        {
            i++;
            leftTrim(rightTrim(line));
            check_braces(line, ws, i);
            if (line.empty())
                continue;
            check_semi(line, i);
            split = ft_splitSpace(line, ';');
            k = 0;
            while (k < split.size())
            {
                j = 0;
                str_key  = get_key(split.at(k), j);
                str_value = get_value(split.at(k), j);
                k++;
                if (!str_key.empty() || !str_value.empty())
                    fill_server(str_key, str_value, line, ws, i);
            }
        }
        myReadFile.close();
    }
    else
        throw std::runtime_error("\033[1;31mFile Error: \033[0m\033[1;37mError Opening File Config.\033[0m");
    if (ws.serv->get_brace_server())
        errors(32, i, "");
    delete ws.serv;
}

Server::~Server(){}