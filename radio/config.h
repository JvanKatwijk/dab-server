#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <string>
#include <map>


class config {
public:
	config	(const std::string filename);
	~config	(void);

	std::string getValue	(const std::string& keyname);
	void	update		(const std::string &keyName,
	                         const std::string &keyvalue);

private:
	void	parse		(const std::string& filename);
	void	update		(void);
	typedef std::pair <std::string, std::string> mapElement;
	std::map<std::string, std::string> configMap;
	std::string	fileName;
};

#endif 

