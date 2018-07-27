#include "Logfile.h"

using namespace std;

template <typename T>
string Logfile::to_string_with_precision(const T a_value, const int n)
{
  ostringstream out;
  out << fixed << setprecision(n) << a_value;
  return out.str();
}


void Logfile::addEventData(string dir, vector<pair<double, double>> info) {
  logfile << dir << " & ";
  pair<double, double> last = info.back();
  info.pop_back();
  for(auto data : info ) {
    logfile << to_string_with_precision(data.first, 2) << " $\\pm$ ";
    logfile << to_string_with_precision(data.second, 2) << " & ";
    
  }
  logfile << to_string_with_precision(last.first, 2) << " $\\pm$ ";
  logfile << to_string_with_precision(last.second, 2) << " \\\\ \\hline" << endl;
}


//// set up header for writing table.  Input plotnames in vector
void Logfile::setHeader(vector<string> plotnames) {

  int totalfiles = plotnames.size();

  logfile << "\\begin{tabular}{ | l |";
  for(int i = 0; i < totalfiles; i++) {
    logfile << " c |";
  }
  logfile << " }" << endl << "\\hline" << endl << "Process";

  for(auto logfile_name:  plotnames) {
    replace(logfile_name, "#", "$\\");
    replace(logfile_name, "_", "-");
    if(logfile_name.find("$") != string::npos) logfile_name.push_back('$');
    logfile << " & " << logfile_name;
  }
    
  logfile << "\\\\ \\hline" << endl;
}


//// end table
void Logfile::setTrailer() {
  logfile << "\\end{tabular}" << endl;
  logfile.close(); 
}



bool Logfile::replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

