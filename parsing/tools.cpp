#include "Server.hpp"

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
                errors(2, nb_line, "");
            ws.serv->set_brace_server(1);
            line.clear();
        }
        else if (!str_key.compare("location") && !str_value.empty())
        {
            if (ws.serv->get_brace_location())
                errors(3, nb_line, "");
            ws.locat.set_path(str_value, nb_line);
            ws.serv->set_brace_location(1);
            line.clear();
        }
        else if (!str_key.compare("server"))
            errors(0, nb_line, str_key);
        else if (!str_key.compare("location"))
            errors(1, nb_line, str_key);
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

int check_directive(std::string key)
{
    if (!key.compare("autoindex") || !key.compare("index") ||
        !key.compare("allow_methods") || !key.compare("return") ||
        !key.compare("fastcgi_pass") || !key.compare("upload_enable") ||
        !key.compare("upload_store"))
        return 0;
    return 1;
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
        std::cout << "default server : " << ws.servers[i].get_df() << std::endl;
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
