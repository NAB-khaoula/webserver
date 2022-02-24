#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <vector>
#include <map>

class Location
{
	private:
		std::string                 _path;
		bool                        _match;
		std::string                 _autoindex;
		std::vector<std::string>    _arr_index;
		std::vector<std::string>    _arr_methods;
		std::map<int, std::string>  _return;
		std::string                 _upload;
		std::string                 _cgi;
		std::string                 _upload_enble;
		std::string					_delete_enble;
	
	public:
		Location();
		
		std::string                  get_path();
		bool                         get_match();
		std::string                  get_autoindex();
		std::vector<std::string>     get_index();
		std::vector<std::string>     get_methods();
		std::map<int, std::string>   &get_return();
		std::string                  get_upload();
		std::string                  get_upload_enble();
		std::string                  get_delete_enble();
		std::string                  get_cgi();
	
		void                        set_path(std::string, int &, struct ws &);
		void                        set_match(bool);
		void                        set_autoindex(std::string, int &, struct ws &);
		void                        set_index(std::string, int &, struct ws &);
		void                        set_methods(std::string, int &, struct ws &);
		void                        set_return(std::string, int  &, struct ws &);
		void                        set_upload(std::string, int &, struct ws &);
		void                        set_upload_enble(std::string, int &, struct ws &);
		void                        set_delete_enble(std::string, int &, struct ws &);
		void                        set_cgi(std::string, int &, struct ws &);
		void                        clear();
		~Location();
};

std::vector<std::string>    ft_splitSpace(std::string str, char c);
std::vector<std::string>    delete_brace(std::string str, int &, struct ws &);
void                        errors(int index, int &, std::string, struct ws &);
std::string&                rightTrim(std::string& str);
std::string&                leftTrim(std::string& str);
bool                        isNumber(std::string s);
#endif