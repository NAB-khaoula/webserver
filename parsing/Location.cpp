#include "Location.hpp"
#include "Server.hpp"

Location::Location()
{
    _path = "/";
    _match = false;
    _autoindex = "off";
    _upload = "";
    _upload_enble = "";
    _arr_methods.clear();
    _return.clear();
}

std::string                 Location::get_path() {return  _path;}
bool                        Location::get_match() {return _match;}
std::string                 Location::get_autoindex() {return  _autoindex;}
std::vector<std::string>    Location::get_index() {return  _arr_index;}
std::vector<std::string>    Location::get_methods() {return  _arr_methods;}
std::map<int, std::string>  &Location::get_return() {return  _return;}
std::string                 Location::get_upload() {return  _upload;}
std::string                 Location::get_upload_enble() {return _upload_enble;}
std::string                 Location::get_delete_enble() {return _delete_enble;}
std::string                 Location::get_cgi() {return _cgi;}

void    Location::set_path(std::string path, int &nb_line, t_WebServ &ws)
{

    path = rightTrim(path);
    path = leftTrim(path);
    if (path.empty())
        errors(1, nb_line, path, ws);
    if (path.find(" ") != std::string::npos || path.find("\t") != std::string::npos ||
        path.find(";") != std::string::npos || path.find("*") != std::string::npos)
        errors(1, nb_line, path, ws);
    else
        _path = path;
}

void    Location::set_match(bool    match)
{
    _match = match;
}

void    Location::set_autoindex(std::string autoindex, int &nb_line, t_WebServ &ws)
{
    if (this->get_autoindex() == "off" ||this->get_autoindex().empty())
    {
        autoindex = rightTrim(autoindex);
        if (!autoindex.compare("off") || !autoindex.compare("on"))
            _autoindex = autoindex;
        else
            errors(12, nb_line, autoindex, ws);
    }
    else
        errors(25, nb_line, autoindex, ws);
}

void    Location::set_upload_enble(std::string enable, int &nb_line, t_WebServ &ws)
{
    if (this->get_upload_enble().empty())
    {
        enable = rightTrim(enable);
        if (!enable.compare("off") || !enable.compare("on"))
            _upload_enble = enable;
        else
            errors(17, nb_line, enable, ws);
    }
    else
        errors(29, nb_line, enable, ws);
}

void    Location::set_delete_enble(std::string enable, int &nb_line, t_WebServ &ws)
{
    if (this->get_delete_enble().empty())
    {
        enable = rightTrim(enable);
        if (!enable.compare("off") || !enable.compare("on"))
            _delete_enble = enable;
        else
            errors(34, nb_line, enable, ws);
    }
    else
        errors(35, nb_line, enable, ws);
}


void    Location::set_upload(std::string path_upload, int &nb_line, t_WebServ &ws)
{
    if (this->get_upload().empty())
    {
        path_upload = rightTrim(path_upload);
        if (path_upload.empty())
            errors(16, nb_line, path_upload, ws);
        if (path_upload.find(" ") != std::string::npos || path_upload.find("\t") != std::string::npos)
            errors(16, nb_line, "root", ws);
        else
            _upload = path_upload;
    }
    else 
        errors(28, nb_line, path_upload, ws);
}

void    Location::set_index(std::string value, int &nb_line, t_WebServ &ws)
{
    if (value.empty())
        errors(13, nb_line, value, ws);
    _arr_index = ft_splitSpace(value, ' ');
}

std::vector<std::string> delete_brace(std::string str, int &nb_line, t_WebServ &ws)
{
    if (str.empty())
    {
        std::cout << "empty\n";
        errors(14, nb_line, str, ws);
    }
    str = rightTrim(str);
    std::vector<std::string> vec;
    size_t pos = 0;
    if ((pos = str.find('[')) != std::string::npos)
        str.erase(0, pos + 1);
    else
    {
        std::cout << "not found [\n";
        errors(14, nb_line, str, ws);
    }
    pos = 0;
    if ((pos = str.find(']')) != std::string::npos)
    {
        str.erase(pos, pos + 1);
        if (str[pos + 1])
            errors(14, nb_line, str, ws);
    }
    else
    {
        std::cout << "not found ]\n";
        errors(14, nb_line, str, ws);
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
                errors(14, nb_line, vec[i], ws);
        }
    }
    else
        errors(14, nb_line, "", ws);
    return vec;
}

void    Location::set_methods(std::string value, int &nb_line, t_WebServ &ws)
{
    if (this->get_methods().empty())
        _arr_methods = delete_brace(value, nb_line, ws);
    else
        errors(26, nb_line, value, ws);
}

void    Location::set_return(std::string _ret, int &nb_line, t_WebServ &ws)
{
    if (this->get_return().empty())
    {
        _ret = rightTrim(_ret);
        size_t pos = 0;
        if ((pos = _ret.find(" ")) != std::string::npos)
        {
            std::string f_str = _ret.substr(0, pos);
            int key;
            if (isNumber(f_str))
                key = atoi(f_str.c_str());
            else
                errors(18, nb_line, f_str, ws); 
            int i = 0;
            std::string s_str = _ret.erase(0, pos + 1);
            while (s_str[i])
            {
                if (s_str[i] == ' ' || s_str[i] == '\t')
                    errors(18, nb_line, s_str, ws);
                i++;
            }
            if (_return.find(key) == _return.end())
                _return[key] = s_str;
        }
        else
            errors(18, nb_line, _ret, ws);
    }
    else 
        errors(27, nb_line, _ret, ws);

}

void    Location::set_cgi(std::string cgi, int &nb_line, t_WebServ &ws)
{
    cgi = rightTrim(cgi);
    if (cgi.empty())
        errors(15, nb_line, cgi, ws);
    if (cgi.find(" ") != std::string::npos || cgi.find("\t") != std::string::npos)
        errors(15, nb_line, cgi, ws);
    else
        _cgi = cgi;
}

void    Location::clear()
{
    _path = "";
    _autoindex = "";
    _arr_index.clear();
    _arr_methods.clear();
    _return.clear();
    _upload = "";
    _match = false;
    _upload_enble = "";
    _delete_enble = "";
    _cgi = "";
}


Location::~Location()
{
}