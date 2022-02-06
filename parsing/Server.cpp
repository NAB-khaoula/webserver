#include "Server.hpp"

Server::Server()
{
    _root  = "";
    _listen = "";
    _host   = "";
    _client_max_body = "";
    _brace_server = 0;
    _brace_location = 0;
}


void    errors(int index, int &nb_line, std::string line)
{
    static std::string arr[] = 
    {
        "Syntax Error: invalid number of arguments in 'server' directive.",//0
        "Syntax Error: invalid number of arguments in 'location' directive.",//1
        "Syntax Error: directive 'server' has no opening '{' OR unexpected '{'.",//2
        "Syntax Error: unknown directive " + line + ".",//3
        "Syntax Error: host not found in " + line +  " of the 'listen' directive",//4
        "Syntax Error: invalid port in " + line + " of the 'listen' directive",//5
        "Syntax Error: unknown " + line + " of the 'host' directive",//6
        "Syntax Error: invalid number of arguments in 'server_name' directive",//7
        "Syntax Error: 'client_max_body_size' directive invalid value",//8
        "Syntax Error: invalid value " + line +  " in 'error_page' directive",//9
        "Syntax Error: invalid number of arguments in " + line + " directive",//10
        "Syntax Error: directive 'location' has no opening '{'.",//11
        "Syntax Error: invalid number of arguments in 'autoindex' directive",//12
        "Syntax Error: invalid number of arguments in 'index' directive",//13
        "Syntax Error: invalid number of arguments in 'allow_methods' directive",//14
        "Syntax Error: invalid number of arguments in 'fastcgi_pass' directive",//15
        "Syntax Error: invalid number of arguments in 'upload_store' directive",//16
        "Syntax Error: invalid number of arguments in 'upload_enable' directive",//17
        "Syntax Error: invalid value " + line +  " in 'return' directive",//18
        "Syntax Error: invalid number of arguments in " + line + " directive.",//19
        "Syntax Error: duplicate location " + line,//20
        "Syntax Error: duplicate listen " + line,//21
        "Syntax Error: duplicate host " + line,//22
        "Syntax Error: duplicate client_max_body_size " + line,//23
        "Syntax Error: duplicate root " + line,//24
        "Syntax Error: duplicate autoindex " + line,//25
        "Syntax Error: duplicate allow_methods " + line,//26
        "Syntax Error: duplicate return " + line,//27
        "Syntax Error: duplicate upload_store " + line,//28
        "Syntax Error: duplicate upload_enable " + line//29
    };
    std::string str = std::to_string(nb_line);
    throw std::runtime_error("|line " + str + "| " + arr[index]);
}

bool isNumber(std::string s)
{
    for (int i = 0; i < s.length(); i++)
        if (isdigit(s[i]) == false)
            return false;
    return true;
}

std::string& rightTrim(std::string& str)
{
    str.erase(str.find_last_not_of(" \t") + 1);
    return str;
}
std::string& leftTrim(std::string& str)
{
    str.erase(0, str.find_first_not_of(" \t"));
    return str;
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
        while (listen[i] /*&& i < 5*/)
        {
            if (isdigit(listen[i]) == false)
                errors(4, nb_line, listen);
            i++;
            len++;
        }
        _listen = listen;
        
        if (ws.ports.find(_listen) == ws.ports.end())
            ws.ports.insert(std::make_pair(_listen, stoi(_listen)));
        // if (len == 2 || len == 4)
        //     _listen = listen;
        // else
        //     errors(5, nb_line, listen);

    }
    else
        errors(21, nb_line, listen);
}
void    Server::set_host(std::string host, int &nb_line)
{
    if (this->get_host().empty())
    {
        host = rightTrim(host);
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
            if (s_str[i] == ' ' || s_str[i] == '\t')
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
        _map_loc[locat.get_path()] = locat;
    else
    {
        std::cout << "dkhal1337\n";
        errors(20, nb_line, locat.get_path());
    }
}

void    Server::set_brace_server(int brace_server) {_brace_server = brace_server;}
void    Server::set_brace_location(int brace_location) {_brace_location = brace_location;}
void    Server::clear()
{
    _listen = "";
    _host = "";
    _server_names.clear();
    _client_max_body = "";
    _err_pages.clear();
    _root = "";
    _location.clear();
    _map_loc.clear();
    _brace_server = 0;
    _brace_location = 0;
}

//NOTE

void    print_attr(t_WebServ &ws)
{
    std::cout << "All Ports : " << std::endl;
    for (std::map<std::string, int>::iterator it = ws.ports.begin(); it != ws.ports.end(); it++)
    {
        std::cout << it->first << " | " << it->second << std::endl;
    }
    
    for (size_t i = 0; i < ws.servers.size(); i++)
    {
        std::cout << "============= Server" << i + 1  << "===========" << std::endl;
        std::cout << ws.servers[i].get_listen() << std::endl;
        std::cout << ws.servers[i].get_host() << std::endl;
        for (std::map<std::string, std::string>::iterator it = ws.servers[i].get_server_names().begin(); it != ws.servers[i].get_server_names().end(); it++)
        {
            std::cout << it->first << " | " << it->second << std::endl;
        }
        std::cout << ws.servers[i].get_client_max_body() << std::endl;
        for (std::map<int, std::string>::iterator it = ws.servers[i].get_err_pages().begin(); it != ws.servers[i].get_err_pages().end(); it++)
        {
            std::cout << it->first << " | " << it->second << std::endl;
        }
        std::cout << ws.servers[i].get_root() << std::endl;
        std::cout << "============= Locations ===========" << std::endl;
        // for (size_t j = 0; j < vec_serv[i].get_location().size(); j++)
        // {
        //     std::cout << "============= Sub Locations" << j + 1 << "===========" << std::endl;
        //     std::cout << vec_serv[i].get_location()[j].get_path() << std::endl;
        //     std::cout << vec_serv[i].get_location()[j].get_match() << std::endl;
        //     std::cout << vec_serv[i].get_location()[j].get_autoindex() << std::endl;
        //     for (size_t k = 0; k <vec_serv[i].get_location()[j].get_index().size() ; k++)
        //     {
        //         std::cout << vec_serv[i].get_location()[j].get_index()[k] << " \t";
        //     }
		// 	std::cout << std::endl;
        //     for (size_t l = 0; l < vec_serv[i].get_location()[j].get_methods().size(); l++)
        //     {
        //         std::cout << vec_serv[i].get_location()[j].get_methods()[l] << std::endl;
        //     }
        //     std::cout << vec_serv[i].get_location()[j].get_return() << std::endl;
        //     std::cout << vec_serv[i].get_location()[j].get_upload_enble() << std::endl;
        //     std::cout << vec_serv[i].get_location()[j].get_upload() << std::endl;
        //     std::cout << vec_serv[i].get_location()[j].get_cgi() << std::endl;
        // }
        int j = 0;
        for (std::map<std::string, Location>::iterator it = ws.servers[i].get_map_loc().begin(); it != ws.servers[i].get_map_loc().end(); it++)
        {
            std::cout << "============= Sub Locations" << j + 1 << "===========" << std::endl;
            std::cout << it->first << std::endl;
            std::cout << it->second.get_match() << std::endl;
            std::cout << it->second.get_autoindex() << std::endl;
            for (size_t i = 0; i < it->second.get_index().size(); i++)
            {
                std::cout << it->second.get_index()[i] << "|";
            }
            std::cout << "\n";
            for (size_t i = 0; i < it->second.get_methods().size(); i++)
            {
                std::cout << it->second.get_methods()[i] << "|";
            }
            std::cout << "\n";
            for (std::map<int, std::string>::iterator it2 = it->second.get_return().begin(); it2 != it->second.get_return().end(); it2++)
            {
                std::cout << it2->first << "|" << it2->second;
            }
            std::cout << "\n";
            std::cout << it->second.get_upload_enble() << std::endl;
            std::cout << it->second.get_upload() << std::endl;
            std::cout << it->second.get_cgi() << std::endl;
            j++;
        }
        
    }

}

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
    {
        std::cout << "dkhal location\n";
        errors(3, nb_line, key);
    }

}

int check_directive(std::string key)
{
    if (!key.compare("autoindex") || !key.compare("index") ||
        !key.compare("allow_methods") || !key.compare("return") ||
        !key.compare("fastcgi_pass") || !key.compare("upload_enable") ||
        !key.compare("upload_store"))
        return 0;
    return 1;
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
            ws.servers.push_back(*ws.serv);
            delete ws.serv;
            ws.serv = new Server();
            // ws.serv->clear();
        }
        else if (ws.serv->get_brace_location() == 2)
            fill_location(key, value, ws, nb_line);
        else if (!check_directive(key) && ws.serv->get_brace_location() == 1)
            errors(11, nb_line, key);
        else
        {
            std::cout << "dkhal hnahhh"<< std::endl;
            errors(3, nb_line, key);
        }
    }
    else
    {
        std::cout << "dkhal yes\n";
        errors(2, nb_line, "");
    }
}

void    check_braces(std::string &line, t_WebServ &ws, int &nb_line)
{
    std::string str_key;
    std::string str_value;
    int i = 0, j = 0, k = 0;

    if ((k = line.find("#")) != std::string::npos)
        line.erase(k, line.length());
    while (line[i] && line[i] != '{')
        i++;
    if (line[i] == '{')
    {
        str_key = get_key(line, j);
        str_value = get_value(line, j);
        if (str_key.empty() && ws.serv->get_brace_server() == 1)
			ws.serv->set_brace_server(2);
        else if (str_key.empty() && ws.serv->get_brace_location() == 1)
			ws.serv->set_brace_location(2);
        else if (!str_key.compare("server"))
        {
            if (!str_value.empty())
                errors(0, nb_line, str_value);
            ws.serv->set_brace_server(2);
        }
        else if (!str_key.compare("location"))
        {
            if (str_value.empty())
                errors(1, nb_line, str_value);
            ws.serv->set_brace_location(2);
            ws.locat.set_path(str_value, nb_line);
        }
		line.erase(0, i + 1);
    }
    else if (!line[i] && (line.find("server") != std::string::npos || line.find("location") != std::string::npos))
    {
        str_key = get_key(line, j);
        str_value = get_value(line, j);
        if (!str_key.compare("server") && str_value.empty())
        {
            if (ws.serv->get_brace_server())
            {
                std::cout << "emmm1\n";
                errors(2, nb_line, "");
            }
            ws.serv->set_brace_server(1);
            line.clear();
        }
        else if (!str_key.compare("location") && !str_value.empty())
        {
            if (ws.serv->get_brace_location())
            {
                std::cout << "dkhal\n";
                errors(3, nb_line, "");
            }
            // std::cout << str_value << std::endl;
            ws.locat.set_path(str_value, nb_line);
            ws.serv->set_brace_location(1);
            line.clear();
        }
        else if (!str_key.compare("server"))
        {
            std::cout << "emmm2\n";
            errors(0, nb_line, str_key);
        }
        else if (!str_key.compare("location"))
        {
            std::cout << "emmm\n";
            errors(1, nb_line, str_key);
        }
    }
}

void    check_semi(std::string line, int &nb_line)
{
    int i = 0;
    std::string key = get_key(line, i);
    std::string value = get_value(line, i);

    int len = value.length() - 1;
    if (value[len] != ';' && key.compare("}"))
        errors(19, nb_line, key);
}

void     begin_parser(t_WebServ &ws)
{
    std::ifstream               myReadFile;
    std::vector<std::string>    split;
    std::string                 line, str_key, str_value;
    int i = 0, j = 0, k;

    ws.serv = new Server();
    myReadFile.open("./parsing/webserv.conf");
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
    if (ws.serv->get_brace_server())
        errors(5, i, "");
    myReadFile.close();
    delete ws.serv;
}


std::vector<std::string>    ft_splitSpace(std::string str, char c)
{
    std::vector<std::string> words;
    size_t pos = 0;
    
    while ((pos = str.find(c)) != std::string::npos)
    {
        words.push_back(str.substr(0, pos));
        str.erase(0, pos + 1);
    }
    words.push_back(str);
    return words;
}

std::string     get_key(std::string &str, int &i)
{
    std::string ret;
    int j = 0;

    while (str[i] == ' ' || str[i] == '\t')
        i++;
    while (str[j + i] && str[j + i] != ' ' && str[j + i] != '\t' && str[j + i] != '\n' && str[j + i] != '{')
        j++;
    
    ret = str.substr(i, j);
    i += j;
    return ret;
}

std::string get_value(std::string &str, int &i)
{
    std::string ret;
    int j = 0;

    while (str[i] == ' ' || str[i] == '\t')
        i++;
    while (str[j + i] && str[j + i] != '{')
        j++;
    
    ret = str.substr(i, j);
    return ret;
}

Server::~Server(){}