#ifndef __CONFIG_FILE_H__
#define __CONFIG_FILE_H__

#include <string>
#include <map>
#include <vector>

#include "Chameleon.h"

class ConfigFile {
  std::map<std::string,Chameleon> content_;
  std::vector<std::string> section_names;
  
  typedef typename std::vector<std::string>::iterator iterator;
  typedef typename std::vector<std::string>::const_iterator const_iterator;
  
public:
  ConfigFile(std::string const& configFile);

  Chameleon const& Value(std::string const& section, std::string const& entry) const;

  Chameleon const& Value(std::string const& section, std::string const& entry, double value);
  Chameleon const& Value(std::string const& section, std::string const& entry, std::string const& value);
  
  inline int size() const {return section_names.size();} 
  const std::string at(int i) const {return section_names.at(i);} 
  inline iterator begin() noexcept { return section_names.begin(); }
  inline const_iterator cbegin() const noexcept { return section_names.cbegin(); }
  inline iterator end() noexcept { return section_names.end(); }
  inline const_iterator cend() const noexcept { return section_names.end(); }


};
#endif
