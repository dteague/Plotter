/////////////////////////////////
//////// PLOTTER CLASS //////////
/////////////////////////////////

/*

Beef of the Plotter code; this takes in all of the
normalized histograms, puts them into a canvas and formats
them all together to look nice.  Most of the functions are for
aesthetics, so to change the look of the graphs, change the Plotter
Functions.  Is a little cumbersome and could use more work into
putting more ability into the style config files/options.

Current state is still work in progress, but does make nice graphs
right now.  may need to revisit when submitting paper and comments
about the graph are more important.

 */



#ifndef _PLOTTER_H_
#define _PLOTTER_H_

#include <TTree.h>
#include <TH1.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TClass.h>
#include <TKey.h>
#include <TGraphAsymmErrors.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TText.h>
#include <TLatex.h>
#include <THStack.h>
#include <TPaveText.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TF1.h>
#include <TStyle.h>
#include <TROOT.h>
#include <cmath>
#include <vector>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unordered_map>
#include "tokenizer.hpp"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <array>
#include <sstream>
#include <iomanip>
#include <regex>


#include "Normalizer.h"
#include "Style.h"
#include "Logfile.h"
#include "histo_style.h"

enum Bottom {SigLeft, SigRight, SigBoth, SigBin, Ratio};



using namespace std;

class Plotter {
 public:
  void CreateStack( TDirectory*); ///fix plot stuff
  void addFile(Normer&);

  int getSize();
  vector<string> getFilenames(string option="all");
  void setStyle(Style*);
  void setBottomType(Bottom input) {bottomType = input;}
  void setSignificanceSSqrtB() {ssqrtsb = false;}
  void setNoBottom() {onlyTop = true;}
  void getPresetBinning(string);
  void setSignal(TList*);
  TLegend* createLeg(const TList* bgl, const TList* sigl);

  TList* createWorkingList(TList* fileList, const char* name, int nBins, double* rebinning);
  THStack* getStack(TList* histos, const char* histoName);
  
  
  double* getBinning(vector<pair<int, double>> binning, double start, double end);
  int numberBinning(vector<pair<int, double>> &binning);

 private:
  TList dataFiles, bgFiles, sigFiles;

  Style* styler;

 bool ssqrtsb = true, onlyTop = false;
  Bottom bottomType = Ratio;
  static unordered_map<string, string> latexer;

  void setupHistogram();
  
  THStack* sortStack(THStack*, bool);


  unordered_map<string, vector<pair<int, double> > > explicitBins;


  TFile* referenceFile = nullptr;
  
  enum {DATA_FILES, BACKG_FILES, SIGNAL_FILES, FILE_ENUM_SIZE};
};

#endif
