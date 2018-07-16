#include "histo_style.h"

using namespace std;

HistInfo::HistInfo(string histname_, ConfigFile& config) : histname(histname_) {
  setBinning(config.Value(histname, "binning"));
  sortSmToLg = config.Value(histname, "sortSToL", true);
  logY = config.Value(histname, "logY", false);
  logX = config.Value(histname, "logX", false);
  title = (string)config.Value(histname, "title", "");
  low = config.Value(histname, "lowValue", 10000000);
  xaxistitle = (string)config.Value(histname, "xTitle", "");
  yaxistitle = (string)config.Value(histname, "yTitle", "Events");
}




void HistInfo::setBinning(string line) {
  string current = "";
  int brac_level = 0;
  vector<string> tmpPairHolder;
  
  for(auto it : line) {
    if(it == '[') brac_level++;
    else if(it == ']' ) {
      --brac_level;
      tmpPairHolder.push_back(current);
    } else if(it == ',') {
      if(tmpPairHolder.size() == 0)  {
	tmpPairHolder.push_back(current);
      } else if(tmpPairHolder.size() == 2) {
	bins.push_back(make_pair(stoi(tmpPairHolder.at(0)),stod(tmpPairHolder.at(1))));
	tmpPairHolder.clear();
      } else {
	cout << "Wrong number of elements!" << endl;
	cout << "\"" << line << "\"" << endl;
	exit(1);
      }
    } else {
      current.push_back(it);
      continue;
    }
    current.clear();
  }
  if(brac_level != 0) {
    cout << "line has incorrect number of brackets!" << endl;
    cout << "\"" << line << "\"" << endl;
    exit(1);
  } else  bins.push_back(make_pair(stoi(tmpPairHolder.at(0)),stod(tmpPairHolder.at(1))));
  
}

