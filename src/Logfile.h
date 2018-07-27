////////////////////////
////  LOGFILE CLASS ////
////////////////////////

/*

Class is a container for the logfile
Essentially a container for the iostream
A little overkill, but this system will allow for more
complex logfiles to be made easily.  Also makes the 
code a little cleaner.  Who doesn't love clean code?

*/


#ifndef _LOGFILE_H_
#define _LOGFILE_H_

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

class Logfile {
 public:
  /// Constructor + Destructor
  //// if other is added, put copy constructor and operator=
  Logfile() { logfile.open("log.txt", ios::out);  }
  Logfile(string logname) { logfile.open(logname, ios::out); }


  /////Helper functions
  //// does the actual writing for the class
  void setHeader(vector<string>);
  void addEventData(string, vector<pair<double,double>>);
  void setTrailer();
  bool replace(std::string& str, const std::string& from, const std::string& to);
  template <typename T>
  string to_string_with_precision(const T a_value, const int n);
  
 private:
  ofstream logfile;
};

#endif
