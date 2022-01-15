#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <vector>

class Location
{
private:
    std::string                 _path;
    bool                        _match;
    std::string                 _autoindex;
    std::vector<std::string>    _arr_index;
    std::vector<std::string>    _arr_methods;
    std::string                 _return;
    std::string                 _upload;

public:
    Location();
    
    std::string                  get_path();
    bool                         get_match();
    std::string                  get_autoindex();
    std::vector<std::string>     get_index();
    std::vector<std::string>     get_methods();
    std::string                  get_return();
    std::string                  get_upload();

    void                        set_path(std::string, int &);
    void                        set_match(bool);
    void                        set_autoindex(std::string, int &);
    void                        set_index(std::string, int &);
    void                        set_methods(std::string, int &);
    void                        set_return(std::string);
    void                        set_upload(std::string);
    void                        clear();
    ~Location();
};

std::vector<std::string>    ft_splitSpace(std::string str, char c);
std::vector<std::string>    delete_brace(std::string str, int &);
void                        errors(int index, int &, std::string);
std::string&                rightTrim(std::string& str);
std::string&                leftTrim(std::string& str);
#endif