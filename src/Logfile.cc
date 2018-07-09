#include "Logfile.h"

using namespace std;

// template <typename T>
// string to_string_with_precision(const T a_value, const int n)
// {
//   ostringstream out;
//   out << fixed << setprecision(n) << a_value;
//   return out.str();
// }



  // //// Loop to write cutflow to logfile
  // TH1* events;
  // current_sourcedir->GetObject("Events", events);

  // if(events) {
  //   vector<string> logEff;
  //   string totalval = "";
  //   logEff.push_back(current_sourcedir->GetName());

  //   for(int i=0; i < FILE_ENUM_SIZE; i++) {
  //     TFile* nextsource = (TFile*)FileList[i]->First();
  //     while ( nextsource ) {
  // 	nextsource->cd(path);
  // 	gDirectory->GetObject("Events", events);
  // 	totalval = to_string_with_precision(events->GetBinContent(2), 1);
  // 	if(i != 0) totalval += " $\\pm$ " + to_string_with_precision(events->GetBinError(2), 1);
  // 	logEff.push_back(totalval);
  // 	nextsource = (TFile*)FileList[i]->After( nextsource );
  //     }
  //   }
  //   logfile.addLine(logEff);
  //   delete events;
  // }
  // events = nullptr;




//// set up header for writing table.  Input plotnames in vector
void Logfile::setHeader(vector<string> plotnames) {

  int totalfiles = plotnames.size();

  logfile << "\\begin{tabular}{ | l |";
  for(int i = 0; i < totalfiles; i++) {
    logfile << " c |";
  }
  logfile << " }" << endl << "\\hline" << endl << "Process";

  for(auto sample_name:  plotnames) {
    string logfile_name = sample_name.substr(0, sample_name.length()-5);
    replace(logfile_name, "#", "\\");
    replace(logfile_name, "_", "-");
    logfile << " & " << logfile_name;
  }
    
  logfile << "\\\\ \\hline" << endl;
}

//// write actual cutflow.  Takes values and puts a & between each value
void Logfile::addLine(vector<string> values) {
  vector<string>::iterator it = values.begin();
  logfile << *it;
  for(++it; it != values.end(); it++) {
    logfile << " & " << *it;
  }
  logfile << " \\\\ \\hline" << endl;
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

