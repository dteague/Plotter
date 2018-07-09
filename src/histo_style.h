#ifndef HISTO_STYLE_H
#define HISTO_STYLE_H

#include "config.h"
#include <vector>
#include <iostream>

using namespace std;

class HistInfo {

 public:
  HistInfo(string, ConfigFile&);
  vector<pair<int,double>>& getBinning() {return bins;}
  bool useSortSmToLg() {return sortSmToLg;}
  bool useLogY() {return logY;}
  bool useLogX() {return logX;}
  const char* getHistname() {return histname.c_str();}
  const char* getTitle() {return title.c_str();}
  
 private:
  
  void setBinning(string);


  vector<pair<int,double> > bins;
  string histname, title;
  bool sortSmToLg;
  bool logY, logX;
  
  
};

#endif
