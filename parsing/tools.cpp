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
        else
            errors(31, nb_line, str_value);
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
            else if (ws.serv->get_brace_location() || ws.serv->get_brace_server() != 2)
                errors(33, nb_line, "");
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

int check_directive_loc(std::string key)
{
    if (!key.compare("autoindex") || !key.compare("index") ||
        !key.compare("allow_methods") || !key.compare("return") ||
        !key.compare("fastcgi_pass") || !key.compare("upload_enable") ||
        !key.compare("upload_store"))
        return 0;
    return 1;
}

int check_directives(std::string key)
{
    if (!key.compare("listen") || !key.compare("host") ||
        !key.compare("server_name") || !key.compare("client_max_body_size") ||
        !key.compare("error_page") || !key.compare("root"))
        return 0;
    return 1;
}

void    errors(int index, int &nb_line, std::string line)
{
    static std::string arr[] = 
    {
        "\033[1;31mSyntax Error: \033[0m\033[1;37minvalid number of arguments in 'server' directive.\033[0m",//0
        "\033[1;31mSyntax Error: \033[0m\033[1;37minvalid number of arguments in 'location' directive\033[0m",//1
        "\033[1;31mSyntax Error: \033[0m\033[1;37mdirective 'server' has no opening '{' OR unexpected '{'\033[0m",//2
        "\033[1;31mSyntax Error: \033[0m\033[1;37munknown directive " + line + "\033[0m",//3
        "\033[1;31mSyntax Error: \033[0m\033[1;37mhost not found in " + line +  " of the 'listen' directive\033[0m",//4
        "\033[1;31mSyntax Error: \033[0m\033[1;37minvalid port in " + line + " of the 'listen' directive\033[0m",//5
        "\033[1;31mSyntax Error: \033[0m\033[1;37munknown " + line + " of the 'host' directive\033[0m",//6
        "\033[1;31mSyntax Error: \033[0m\033[1;37minvalid number of arguments in 'server_name' directive\033[0m",//7
        "\033[1;31mSyntax Error: \033[0m\033[1;37m'client_max_body_size' directive invalid value\033[0m",//8
        "\033[1;31mSyntax Error: \033[0m\033[1;37minvalid value " + line +  " in 'error_page' directive\033[0m",//9
        "\033[1;31mSyntax Error: \033[0m\033[1;37minvalid number of arguments in " + line + " directive\033[0m",//10
        "\033[1;31mSyntax Error: \033[0m\033[1;37mdirective 'location' has no opening '{'.\033[0m",//11
        "\033[1;31mSyntax Error: \033[0m\033[1;37minvalid number of arguments in 'autoindex' directive\033[0m",//12
        "\033[1;31mSyntax Error: \033[0m\033[1;37minvalid number of arguments in 'index' directive\033[0m",//13
        "\033[1;31mSyntax Error: \033[0m\033[1;37minvalid number of arguments in 'allow_methods' directive\033[0m",//14
        "\033[1;31mSyntax Error: \033[0m\033[1;37minvalid number of arguments in 'fastcgi_pass' directive\033[0m",//15
        "\033[1;31mSyntax Error: \033[0m\033[1;37minvalid number of arguments in 'upload_store' directive\033[0m",//16
        "\033[1;31mSyntax Error: \033[0m\033[1;37minvalid number of arguments in 'upload_enable' directive\033[0m",//17
        "\033[1;31mSyntax Error: \033[0m\033[1;37minvalid value " + line +  " in 'return' directive\033[0m",//18
        "\033[1;31mSyntax Error: \033[0m\033[1;37minvalid number of arguments in " + line + " directive.\033[0m",//19
        "\033[1;31mSyntax Error: \033[0m\033[1;37mduplicate location " + line + "\033[0m",//20
        "\033[1;31mSyntax Error: \033[0m\033[1;37mduplicate listen " + line + "\033[0m",//21
        "\033[1;31mSyntax Error: \033[0m\033[1;37mduplicate host " + line + "\033[0m",//22
        "\033[1;31mSyntax Error: \033[0m\033[1;37mduplicate client_max_body_size " + line + "\033[0m",//23
        "\033[1;31mSyntax Error: \033[0m\033[1;37mduplicate root " + line + "\033[0m",//24
        "\033[1;31mSyntax Error: \033[0m\033[1;37mduplicate autoindex " + line + "\033[0m",//25
        "\033[1;31mSyntax Error: \033[0m\033[1;37mduplicate allow_methods " + line + "\033[0m",//26
        "\033[1;31mSyntax Error: \033[0m\033[1;37mduplicate return " + line + "\033[0m",//27
        "\033[1;31mSyntax Error: \033[0m\033[1;37mduplicate upload_store " + line + "\033[0m",//28
        "\033[1;31mSyntax Error: \033[0m\033[1;37mduplicate upload_enable " + line + "\033[0m",//29
        "\033[1;31mSyntax Error: \033[0m\033[1;37mimportant in block server contain |'listen'|'host'|'root'| " + line + "\033[0m",//30
        "\033[1;31mSyntax Error: \033[0m\033[0m\033[1;37munexpected '{'\033[0m",//31
        "\033[1;31mSyntax Error: \033[0m\033[0m\033[1;37munexpected end of file, expecting '}'\033[0m",//32
        "\033[1;31mSyntax Error: \033[0m\033[0m\033[1;37mdirective is not allowed here\033[0m",//33
    };
    std::string str = std::to_string(nb_line);
    throw std::runtime_error("\033[1;31m|line " + str + "| \033[0m" + arr[index]);
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
