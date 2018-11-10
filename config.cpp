
#include	"config.h"
#include	<stdlib.h>
#include	<iostream>
#include	<fstream>
#include	<string>


// trim leading white-spaces
static std::string& ltrim(std::string& s) {
	size_t startpos = s.find_first_not_of(" \t\r\n\v\f");
	if (std::string::npos != startpos) {
	   s = s.substr(startpos);
	}
	return s;
}

// trim trailing white-spaces
static std::string& rtrim (std::string& s) {
	size_t endpos = s.find_last_not_of(" \t\r\n\v\f");
	if (std::string::npos != endpos) {
	   s = s.substr(0, endpos + 1);
	}
	return s;
}

	config::config (const std::string filename) {
std::string env	= getenv ("HOME");

	this	-> fileName	= env. append (filename);
	fprintf (stderr, "config is stored as %s\n", fileName. c_str ());
	configMap. clear ();
	parse (this -> fileName);
}

std::string config::getValue (const std::string&keyName) {
	for (std::map<std::string, std::string>
                                     ::iterator it = configMap. begin ();
                     it != configMap. end (); it ++)
           if (it -> first == keyName) {
	      return it -> second;
	   }
	return "";
}

void	config::update (const std::string &keyName, const std::string &val) {
	for (std::map<std::string, std::string>
                                     ::iterator it = configMap. begin ();
	      it != configMap. end (); it ++) {
           if (it -> first == keyName) {
	      configMap [keyName] = val;
	      update ();
	      return;
	   }
	}
	configMap. insert (mapElement (keyName, val));
	update ();
}


void config::parse (const std::string& filename) {
std::ifstream ifstrm;

	ifstrm. open (filename);
	if (!ifstrm) {
	   fprintf (stderr, "Opening %s lukte niet\n",  filename. c_str ());
	   return;
	}

	fprintf (stderr, "Opening %s lukte\n", filename. c_str ());
	for (std::string line; std::getline (ifstrm, line);) {
//	if a comment
	   if (!line.empty() && (line[0] == ';' || line[0] == '#')) { }
// allow both ; and # comments at the start of a line
	   else
	   if (!line. empty ()) {
/* Not a comment, must be a name[=:]value pair */
	      size_t end = line. find_first_of ("=:");
	      if (end != std::string::npos) {
	         std::string name = line. substr(0, end);
	         std::string value = line.substr(end + 2);
	         ltrim (rtrim (name));
	         ltrim (rtrim(value));
		 configMap. insert (mapElement (name, value));
	         fprintf (stderr, "inserted (%s, %s)\n",
	                              name. c_str (), value. c_str ());
	      }
	      else { }
                // no key value delimiter
	   }
	} // for
	ifstrm. close ();
}

	config::~config (void) { }

void	config::update	(void) {
std::ofstream ofstrm;

	ofstrm. open (fileName);
	if (!ofstrm) {
	   fprintf (stderr, "trying to open %s failed\n", fileName. c_str ());
	   return;
	}

	ofstrm << "# configuration for dab-server\n";
	for (std::map<std::string, std::string>
                                     ::iterator it = configMap. begin ();
                     it != configMap. end (); it ++)
	   ofstrm << it -> first << "=:" << it -> second << "\n";
	ofstrm. close ();
}


