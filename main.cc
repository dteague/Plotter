#include <vector>
#include <TH1.h>
#include <TFile.h>
#include <TDirectory.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <array>
#include <TClass.h>
#include <TKey.h>
#include <TGraphAsymmErrors.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <map>
#include "tokenizer.hpp"
#include <fstream>

using namespace std;

struct Plot {
  vector<string> input;
  string output;
  vector<double> skim, xsec;
  bool use;

  void print() {
    cout << " =========== " << output << " =========== " << endl;
    for(int i = 0; i < input.size(); ++i) {
      cout << input.at(i) << " " << skim.at(i) << " " << xsec.at(i) << ((use) ? " Used" : " Skip" )<<  endl;
    }
    cout << endl;
  }
};


vector<string> dirvec;
vector<array<double, 3>> histinfo;
vector<string> histnamevec;
map<string, Plot*> plots;

double lumi;


  float effSkim;
  vector<string> inRootFiles;
  vector<string> inDirectories;
  vector<string> inProcess;
  vector<float> inScaleFactor;
  vector<float> inScaleFactorError;
  vector<float> x_section;
  vector<float> theCumulativeEfficiency;
  vector<float> RelativeEffVector;
  vector<float> RelativeEffErrorVector;
  vector<float> TotalEffVector;
  vector<float> TotalEffErrorVector;
  vector<float> theEventsAnalyzed;
  vector<float> theEventsPassing;
  vector<string> theHistNameStrings;
  vector<double> maxHistContents;
  vector<TH1F*> HistList;
  vector<TH1F*> HistList2;
//  TList *HistList = new TList();
//  TList *HistList2 = new TList();
  int nHistList;
  int nHistList2;
  int nHistos;
  string IsData;
  float MaxEventsAnalyzed;
  string outputRootFileForNormalizedHistos;
  string outputRootFileForProbabilityHistos;

  string lumi_string;
  TFile *theCurrentFile;




void setupFiles(const vector<string>&, vector<TFile*>&);
inline int rounder(double val);
void calculateEfficienciesAndErrors();
void read_info(string);
int getModTime(const char*);
bool shouldAdd(string, string);
void createLogFile(string);

int main(int argc, char* argv[]) {
  vector<string> files;
  read_info("config.txt");

  for(map<string, Plot*>::iterator it = plots.begin(); it != plots.end(); ++it) {
    it->second->print();
  }


  for(int i = 1; i < argc; i++) {
    files.push_back(argv[i]);
  }
  // vector<TFile*> filevec;
  // setupFiles(files, filevec);
  // for(int i = 0; i < histnamevec.size(); ++i) {
  //   cout << histnamevec.at(i) << " " << histinfo.at(i)[0] << " " << histinfo.at(i)[1] << " " << histinfo.at(i)[2] << endl;

  // }
}


void setupFiles(const vector<string>& rootfiles, vector<TFile*>& filevec) {
  bool firstTime = true;
  bool firstdir = true;
  for(vector<string>::const_iterator it= rootfiles.begin(); it != rootfiles.end(); ++it) {
    string name = *it;
    TFile* tmpfile = new TFile(name.c_str());
    filevec.push_back(tmpfile);
    TIter tmpit(tmpfile->GetListOfKeys());

    while(TObject* obj = tmpit()) {
      if(firstTime) {
	dirvec.push_back(obj->GetName());
	if(firstdir) {
	  cout << tmpfile->cd(obj->GetName()) << endl;
	  TIter tmphist(gDirectory->GetListOfKeys());

	  while(TKey* key = (TKey*)tmphist()) {
	    TH1F* hist = (TH1F*)key->ReadObj();
	    //	    if(! hist->InheritsFrom(TH1F::Class())) continue;
	    histnamevec.push_back(hist->GetName());
	    cout << (double)((TH1*)hist)->GetXaxis()->GetNbins() << endl;
	    histinfo.push_back( {(double)((TH1*)hist)->GetXaxis()->GetNbins(), ((TH1*)hist)->GetXaxis()->GetXmin(), \
		  ((TH1*)hist)->GetXaxis()->GetXmax()});
	  }
	}
	firstdir = false;
      }      
    }
    firstTime = false;
  }
}

void read_info(string filename) {
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  ifstream info_file(filename);
  boost::char_separator<char> sep(", \t");

  if(!info_file) {
    std::cout << "could not open file " << filename <<std::endl;
    exit(1);
  }

  vector<string> stemp;
  string group, line;
  while(getline(info_file, line)) {
    tokenizer tokens(line, sep);
    stemp.clear();
    for(tokenizer::iterator iter = tokens.begin();iter != tokens.end(); iter++) {
      if( ((*iter)[0] == '/' && (*iter)[0] == '/') || ((*iter)[0] == '#') ) break;
      stemp.push_back(*iter);

    }
    if(stemp.size() == 0) continue;
    else if(stemp.size() == 2) {
      lumi = stod(stemp[1]);
    } else if(stemp.size() == 4) {
      if(plots.find(stemp[1]) == plots.end()) { 
	Plot* tmpplot = new Plot;
	tmpplot->input.push_back(stemp[0]);
	tmpplot->output = stemp[1];
	tmpplot->xsec.push_back(stod(stemp[2]));
	tmpplot->skim.push_back(stod(stemp[3]));
	tmpplot->use = shouldAdd(stemp[0], stemp[1]);
	plots[stemp[1]] = tmpplot;
      } else {
	plots[stemp[1]]->input.push_back(stemp[0]);
	plots[stemp[1]]->xsec.push_back(stod(stemp[2]));
	plots[stemp[1]]->skim.push_back(stod(stemp[3]));
	plots[stemp[1]]->use += shouldAdd(stemp[0], stemp[1]);
      }
    }
  }
  info_file.close();
}

bool shouldAdd(string infile, string globalFile) {
  struct stat buffer;
  if(stat(infile.c_str(), &buffer) != 0) return false;
  ////need to change so doesn't make plot if fatal error (eg file doesn't exist!
  else if(stat(globalFile.c_str(), &buffer) != 0) return true;
  else if(getModTime(globalFile.c_str()) > getModTime(infile.c_str())) return false;
  else return true;

}

int getModTime(const char *path) {
  struct stat attr;
  stat(path, &attr);
  char date[100] = {0};
  strftime(date, 100, "%s", localtime(&attr.st_mtime));
  return atoi(date);

}


void calculateEfficienciesAndErrors() {

  float numerator = 1.0;
  float denominator = 1.0;
  float theRelativeEfficiency = 1.0;
  float efferror = 0.0;
  float cumulativeEfficiency = 1.0;
  float efferror2 = 0.0;

  // Calculate relative and cumulative cut efficiencies
  for (int i=0;i<theCumulativeEfficiency.size(); i++) {
    numerator = rounder(MaxEventsAnalyzed * theCumulativeEfficiency.at(i));
    denominator = (i > 0) ? rounder(MaxEventsAnalyzed * theCumulativeEfficiency.at(i-1)) : rounder(MaxEventsAnalyzed);

    if(numerator > 0) {
      theRelativeEfficiency = numerator / denominator;
      efferror = sqrt(theRelativeEfficiency*(1.-theRelativeEfficiency)/denominator);
    } else {
      theRelativeEfficiency = 0.0;
      efferror = (denominator != 0.0) ? 1.0 / denominator : 0.0;
    }

    /* binomial uncertainties do not work when the efficiency gets close to 1 or 0 (e.g. efficiency cannot
       be 99 +- 2 because the efficiency cannot be e.g. 101) ... in these cases, use bayesian */
    if(((theRelativeEfficiency + efferror) > 1.) || ((theRelativeEfficiency - efferror) < 0.)){
      TH1F theNumHisto("theNumHisto","theNumHisto",1,0,1);
      theNumHisto.SetBinContent(1,numerator);
      theNumHisto.Sumw2();
      TH1F theDenHisto("theDenHisto","",1,0,1);
      theDenHisto.SetBinContent(1,denominator);
      theDenHisto.Sumw2();
      TGraphAsymmErrors bayesEff;
      bayesEff.BayesDivide(&theNumHisto, &theDenHisto,"b");
      efferror = (bayesEff.GetErrorYhigh(0) > bayesEff.GetErrorYlow(0)) ? bayesEff.GetErrorYhigh(0) : bayesEff.GetErrorYlow(0);
    }

    if(theRelativeEfficiency == 1.0) efferror = 0;

    // recalculate efficiencies and errors incorporating scale factors
    theRelativeEfficiency *= inScaleFactor.at(i);
    if(numerator > 0) {
      efferror = sqrt(pow(efferror*inScaleFactor.at(i),2.0) + pow(inScaleFactorError.at(i)*theRelativeEfficiency,2.0));
    } else {
      efferror = (denominator != 0) ? inScaleFactor.at(i) / denominator : 0.0;
    }

    numerator = rounder(MaxEventsAnalyzed * theCumulativeEfficiency.at(i));
    denominator = rounder(MaxEventsAnalyzed);

    if(numerator > 0) {
      cumulativeEfficiency = numerator / denominator;
      efferror2 = sqrt(cumulativeEfficiency*(1.-cumulativeEfficiency)/denominator);
    } else {
      cumulativeEfficiency = 0.0;
      efferror2 = (denominator != 0.0) ? 1.0 / denominator : 0.0;
    }

    /* binomial uncertainties do not work when the efficiency gets close to 1 or 0 (e.g. efficiency cannot
       be 99 +- 2 because the efficiency cannot be e.g. 101) ... in these cases, use bayesian */
    if(((cumulativeEfficiency + efferror2) > 1.) || ((cumulativeEfficiency - efferror2) < 0.)){
      TH1F theNumHisto("theNumHisto","theNumHisto",1,0,1);
      theNumHisto.SetBinContent(1,numerator);
      theNumHisto.Sumw2();
      TH1F theDenHisto("theDenHisto","",1,0,1);
      theDenHisto.SetBinContent(1,denominator);
      theDenHisto.Sumw2();
      TGraphAsymmErrors bayesEff;
      bayesEff.BayesDivide(&theNumHisto,&theDenHisto,"b");
      efferror2 = (bayesEff.GetErrorYhigh(0) > bayesEff.GetErrorYlow(0)) ? bayesEff.GetErrorYhigh(0) : bayesEff.GetErrorYlow(0);
    }

    if(cumulativeEfficiency == 1.0) efferror2 = 0;
    // recalculate efficiencies and errors incorporating scale factors
    for(int numberOfSFs = 0; numberOfSFs <= i; numberOfSFs++) {
      if(numerator > 0) {
	efferror2 = sqrt(pow(efferror2*inScaleFactor.at(numberOfSFs),2.0) + pow(inScaleFactorError.at(numberOfSFs)*cumulativeEfficiency,2.0));
	cumulativeEfficiency *= inScaleFactor.at(numberOfSFs);
      } else {
	cumulativeEfficiency *= (denominator != 0.) ? inScaleFactor.at(numberOfSFs) : 0.;
	efferror2 = (denominator != 0.) ? inScaleFactor.at(numberOfSFs) / denominator : 0.0;
      }
    }
      
    RelativeEffVector.push_back(theRelativeEfficiency);
    RelativeEffErrorVector.push_back(efferror);
    TotalEffVector.push_back(cumulativeEfficiency);
    TotalEffErrorVector.push_back(efferror2);
  }
}

inline int rounder(double val) {
  return (int)(val + 0.5);
}

void NormalizeHistos() {

  if(( nHistList % inRootFiles.size() == 0 )) {
    for(int i=0; i<nHistos; i++) {
      string histname = HistList.at(i)->GetName();
      string histoEffyName = "hEffy_" + inProcess.at(0);
      TH1F *h = (TH1F*)HistList.at(i);
      h->Sumw2();
      if(IsData != "0") continue;

      if(h->Integral() > 0) h->Scale(1.0 / theEventsPassing.at(0) * x_section.at(0) * lumi * effSkim);
      TH1F *hEffy = new TH1F(histoEffyName.c_str(),histoEffyName.c_str(),h->GetXaxis()->GetNbins(),h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax());
      for(int ibin=0; ibin<=(hEffy->GetXaxis()->GetNbins() + 1); ibin++) {
	hEffy->SetBinContent(ibin,RelativeEffVector.at(0));
	hEffy->SetBinError(ibin,0);
      }
      h->Multiply(h,hEffy,1,1,"");
      for(int ibin=0; ibin<=(h->GetXaxis()->GetNbins() + 1); ibin++) {
	h->SetBinError(ibin, sqrt(pow(h->GetBinError(ibin),2.0) + h->GetBinContent(ibin)) ); // propagating MC statistical uncertainty & poisson uncertainty on the r$
      }
      delete hEffy;
    }

    h->SetName(histname.c_str());
    string YtitleString = "N / " + lumi_string + " pb^{-1}";
    // h->GetYaxis()->SetTitle(YtitleString.c_str());
    // h->GetYaxis()->SetTitleSize(0.06);
    // h->GetYaxis()->SetTitleFont(62);
    // h->GetYaxis()->CenterTitle();
    // h->GetYaxis()->SetLabelSize(0.05);
    // h->GetYaxis()->SetLabelFont(62);
    // h->GetXaxis()->SetTitle(theHistNameStrings.at(i).c_str());
    // h->GetXaxis()->SetTitleSize(0.06);
    // h->GetXaxis()->SetTitleFont(62);
    // h->GetXaxis()->CenterTitle();
    // h->GetXaxis()->SetLabelSize(0.05);
    // h->GetXaxis()->SetLabelFont(62);
    TFile *hfile = (TFile*)gROOT->FindObject(outputRootFileForNormalizedHistos.c_str());
    if (hfile) {hfile->Close();}
    hfile = new TFile(outputRootFileForNormalizedHistos.c_str(),"UPDATE");
    h->Write();
    hfile->Close();
    delete h;
    if(inRootFiles.size() > 1) {
      for(int j=1;j<inRootFiles.size();j++) {
	string hist2name = HistList.at(i+(j*nHistos))->GetName();
	if(theHistNameStrings.at(i) == theHistNameStrings.at(i+(j*nHistos))) {
	  string histoEffyName2 = "hhEffy_" + inProcess.at(j);
	  TH1F *hh = (TH1F*)HistList.at(i+(j*nHistos));
	  hh->Sumw2();
	  if(IsData == "0") {
	    if(hh->Integral(0,(hh->GetXaxis()->GetNbins()+1)) > 0) {hh->Scale(1.0 / theEventsPassing.at(j) * x_section.at(j) * lumi * effSkim);}
	    for(int jfile=0;jfile<=j;jfile++) {
	      TH1F *hhEffy = new TH1F(histoEffyName2.c_str(),histoEffyName2.c_str(),
				      hh->GetXaxis()->GetNbins(),hh->GetXaxis()->GetXmin(),hh->GetXaxis()->GetXmax());
	      for(int ibin=0; ibin<=(hhEffy->GetXaxis()->GetNbins() + 1); ibin++) {
		hhEffy->SetBinContent(ibin,RelativeEffVector.at(jfile));
		hhEffy->SetBinError(ibin,0);
	      }
	      hh->Multiply(hh,hhEffy,1,1,"");
	      delete hhEffy;
	    }
	    for(int ibin=0; ibin<=(hh->GetXaxis()->GetNbins() + 1); ibin++) {
	      hh->SetBinError(ibin, sqrt(pow(hh->GetBinError(ibin),2.0) + hh->GetBinContent(ibin)) );
	    }
	  }
	  hh->SetName(hist2name.c_str());
	  string YtitleString = "N / " + lumi_string + " pb^{-1}";
	  // hh->GetYaxis()->SetTitle(YtitleString.c_str());
	  // hh->GetYaxis()->SetTitleSize(0.06);
	  // hh->GetYaxis()->SetTitleFont(62);
	  // hh->GetYaxis()->CenterTitle();
	  // hh->GetYaxis()->SetLabelSize(0.05);
	  // hh->GetYaxis()->SetLabelFont(62);
	  // hh->GetXaxis()->SetTitle(theHistNameStrings.at(i+(j*nHistos)).c_str());
	  // hh->GetXaxis()->SetTitleSize(0.06);
	  // hh->GetXaxis()->SetTitleFont(62);
	  // hh->GetXaxis()->CenterTitle();
	  // hh->GetXaxis()->SetLabelSize(0.05);
	  // hh->GetXaxis()->SetLabelFont(62);
	  TFile *hfile = (TFile*)gROOT->FindObject(outputRootFileForNormalizedHistos.c_str());
	  if (hfile) {hfile->Close();}
	  hfile = new TFile(outputRootFileForNormalizedHistos.c_str(),"UPDATE");
	  hh->Write();
	  hfile->Close();
	  delete hh;
	}
      }
    }
  }

}

void CreateProbHistos() {

  if(( nHistList2 % inRootFiles.size() != 0 )) return;

  for(int i=0; i<nHistos; i++) {
    string histname = HistList2.at(i)->GetName();
    TH1F *h = (TH1F*)HistList2.at(i);
    h->Sumw2();
    if(h->Integral(0,(h->GetXaxis()->GetNbins()+1)) > 0) {h->Scale(1.0 / h->Integral(0,(h->GetXaxis()->GetNbins()+1)));}
    h->SetName(histname.c_str());
    h->GetYaxis()->SetTitle("a.u.");
    h->GetYaxis()->SetTitleSize(0.06);
    h->GetYaxis()->SetTitleFont(62);
    h->GetYaxis()->CenterTitle();
    h->GetYaxis()->SetLabelSize(0.05);
    h->GetYaxis()->SetLabelFont(62);
    h->GetXaxis()->SetTitle(theHistNameStrings.at(i).c_str());
    h->GetXaxis()->SetTitleSize(0.06);
    h->GetXaxis()->SetTitleFont(62);
    h->GetXaxis()->CenterTitle();
    h->GetXaxis()->SetLabelSize(0.05);
    h->GetXaxis()->SetLabelFont(62);
    TFile *hfile = (TFile*)gROOT->FindObject(outputRootFileForProbabilityHistos.c_str());
    if (hfile) {hfile->Close();}
    hfile = new TFile(outputRootFileForProbabilityHistos.c_str(),"UPDATE");
    h->Write();
    hfile->Close();
    delete h;
    if(inRootFiles.size() <= 1) continue;

    for(int j=1;j<inRootFiles.size();j++) {
      string hist2name = HistList2.at(i+(j*nHistos))->GetName();
      if(theHistNameStrings.at(i) != theHistNameStrings.at(i+(j*nHistos))) continue;

      TH1F *hh = (TH1F*)HistList2.at(i+(j*nHistos));
      hh->Sumw2();
      if(hh->Integral(0,(hh->GetXaxis()->GetNbins()+1)) > 0) hh->Scale(1.0 / hh->Integral(0,(hh->GetXaxis()->GetNbins()+1)));
      hh->SetName(hist2name.c_str());
      hh->GetYaxis()->SetTitle("a.u.");
      hh->GetYaxis()->SetTitleSize(0.06);
      hh->GetYaxis()->SetTitleFont(62);
      hh->GetYaxis()->CenterTitle();
      hh->GetYaxis()->SetLabelSize(0.05);
      hh->GetYaxis()->SetLabelFont(62);
      hh->GetXaxis()->SetTitle(theHistNameStrings.at(i+(j*nHistos)).c_str());
      hh->GetXaxis()->SetTitleSize(0.06);
      hh->GetXaxis()->SetTitleFont(62);
      hh->GetXaxis()->CenterTitle();
      hh->GetXaxis()->SetLabelSize(0.05);
      hh->GetXaxis()->SetLabelFont(62);
      TFile *hfile = (TFile*)gROOT->FindObject(outputRootFileForProbabilityHistos.c_str());
      if (hfile) {hfile->Close();}
      hfile = new TFile(outputRootFileForProbabilityHistos.c_str(),"UPDATE");
      hh->Write();
      hfile->Close();
      delete hh;
    }
  }
}

//---function called once just before obtaining user inputs. It clears the vectors.
void createLogFile(string outputLogFile) {

  // create an output log file that will contain the cut flow efficiency table
  ofstream outFile;
  outFile.open(outputLogFile.c_str(), ios::out);

  // if output log file cannot be opened, exit the code
  if (!outFile) {
    cerr << "Can't open output file " << outputLogFile << endl;
    exit(1);
  } else  outFile << endl << "The following input was used: " << endl << endl << endl;


  // loop over root files
  for(int j=0;j<inRootFiles.size();j++) {
    // printout information to log file
    outFile << "Name of Root File #" << (j+1) << " : " << inRootFiles.at(j) << endl;
    outFile << "Name of Directory #" << (j+1) << " : " << inDirectories.at(j) << endl;
    outFile << "Name of Process #" << (j+1) << "   : " << inProcess.at(j) << endl << endl;
  }

  //  outFile << "Detailed Efficiencies " << "\n";
  outFile << "Table of Efficiencies\n\n";
  outFile << "Efficiencies below are calculated with respect to the number of events after skimming\n\n";
  outFile << "-------------------------------------------------------------------------------------------------------------------------------\n";
  outFile << "         Name                         Events               Relative (%)                     Cumulative (%)\n";
  outFile << "-------------------------------------------------------------------------------------------------------------------------------\n";

  if(theCumulativeEfficiency.size() <= 0) return;
  float effy = 1;
  float deffyOvereffy = 0;
  for (int i=0;i<theCumulativeEfficiency.size(); i++) {
    effy = effy * RelativeEffVector.at(i);
    deffyOvereffy = sqrt(pow(RelativeEffErrorVector.at(i)/RelativeEffVector.at(i),2.0) + pow(deffyOvereffy,2.0));
    // output for cut flow efficiency table - efficiencies and errors
    outFile     <<setw(24)<<inProcess.at(i)
		<<setw(20)<<round(MaxEventsAnalyzed * theCumulativeEfficiency.at(i))
		<<setw(15)<<setprecision(4)<<RelativeEffVector.at(i)*100.0<<setw(4)<<" +- "<<setw(10)<<setprecision(4)<<(RelativeEffErrorVector.at(i)*100.0)
		<<setw(20)<<setprecision(4)<<TotalEffVector.at(i)*100.0<<setw(4)<<" +- "<<setw(10)<<setprecision(4)<<(TotalEffErrorVector.at(i) * 100.0)
		<<endl;
  }

  outFile << "-------------------------------------------------------------------------------------------------------------------------------\n";

  outFile.close();

}


