#include "Location.hpp"

Location::Location(){}
std::string                 Location::get_path() {return  _path;}
bool                        Location::get_match() {return _match;}
std::string                 Location::get_autoindex() {return  _autoindex;}
std::vector<std::string>    Location::get_index() {return  _arr_index;}
std::vector<std::string>    Location::get_methods() {return  _arr_methods;}
std::string                 Location::get_return() {return  _return;}
std::string                 Location::get_upload() {return  _upload;}

void    Location::set_path(std::string path, int &nb_line)
{
    int pos = 0;

    path = rightTrim(path);
    if((pos = path.find("=")) != std::string::npos)
    {
        _match = true;
        path.erase(0, pos + 1);
    }
    path = leftTrim(path);
    if (path.find(" ") != std::string::npos || path.find("\t") != std::string::npos)
        errors(12, nb_line, path);
    else
    {
        _path = path;
        // std::cout << _path << std::endl;
    }
}

void    Location::set_match(bool    match)
{
    _match = match;
}

void    Location::set_autoindex(std::string autoindex, int &nb_line)
{
    autoindex = rightTrim(autoindex);
    if (!autoindex.compare("off") || !autoindex.compare("on"))
    {
        // std::cout << autoindex << std::endl;
        _autoindex = autoindex;
    }
    else
        errors(11, nb_line, autoindex);
}
void    Location::set_index(std::string value, int &nb_line)
{
    if (value.empty())
        errors(12, nb_line, value);
    _arr_index = ft_splitSpace(value, ' ');
    // for (size_t i = 0; i < _arr_index.size(); i++)
    // {
    //     std::cout << _arr_index[i] << " ";
    // }
    // std::cout << "\n";
   
}

std::vector<std::string> delete_brace(std::string str, int &nb_line)
{
    if (str.empty())
    {
        std::cout << "empty\n";
        errors(13, nb_line, str);
    }
    str = rightTrim(str);
    std::vector<std::string> vec;
    size_t pos = 0;
    if ((pos = str.find('[')) != std::string::npos)
        str.erase(0, pos + 1);
    else
    {
        std::cout << "not found [\n";
        errors(13, nb_line, str);
    }
    pos = 0;
    if ((pos = str.find(']')) != std::string::npos)
    {
        str.erase(pos, pos + 1);
        if (str[pos + 1])
            errors(13, nb_line, str);
    }
    else
    {
        std::cout << "not found ]\n";
        errors(13, nb_line, str);
    }
    vec = ft_splitSpace(str, ',');
    if (vec.size() >= 1 && vec.size() <= 3)
    {
        for (size_t i = 0; i < vec.size(); i++)
        {
            vec[i] = leftTrim(rightTrim(vec[i]));
            if (!vec[i].compare("GET") || !vec[i].compare("POST") || !vec[i].compare("DELETE"))
                continue ;
            else
                errors(13, nb_line, vec[i]);
        }
    }
    else
        errors(13, nb_line, "");
    return vec;
}

void    Location::set_methods(std::string value, int &nb_line)
{
    _arr_methods = delete_brace(value, nb_line);
    
}
void    Location::set_return(std::string _ret){_return = _ret;}
void    Location::set_upload(std::string upload){_upload = upload;}

void    Location::clear()
{
    _path = "";
    _autoindex = "";
    _arr_index.clear();
    _arr_methods.clear();
    _return = "";
    _upload = "";
    _match = false;
}

Location::~Location()
{
}