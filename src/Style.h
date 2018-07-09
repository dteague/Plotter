///////////////////////////
////// STYLE CLASS ////////
//////////////////////////

/*

This is the class that holds the TStyle that is 
used to format the basic idea of the graph.  

Some extra, user defined values are included that are used
by the Plotter to make correct graphs (eg PadRatio).

Main point of this class is to interact with config files for styling the
graphs without having to recompile the code every time.  More can be added to
stop this problem more since there are some style things hardcoded into the 
Plotter class

 */


#ifndef _STYLE_H_
#define _STYLE_H_

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
#include <THStack.h>
#include <TPaveText.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TF1.h>
#include <TStyle.h>
#include <TROOT.h>

#include <vector>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <map>
#include "tokenizer.hpp"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <array>
#include "config.h"
#include "histo_style.h"

using namespace std;

class Style {
 public:

  Style();
  Style(string);
  Style(const Style&);
  Style& operator=(const Style&);
  ~Style();

  const vector<HistInfo*>& getHistograms() {return histograms;}

  
  void read_info(string);
  void setStyle();
  TStyle* getStyle();
  double getPadRatio() {return padratio;}
  double getHeightRatio() {return heightratio;}
  double getRebinLimit() {return rebinlimit;}
  bool getDivideBins() {return dividebins;}
  bool getBinLimit() {return binlimit;}
  bool getDoOverflow() {return dooverflow;}

  int getColor();
  void resetColoring();
  
 private:
  void setupGeneral(ConfigFile&);

  TStyle* styler;

  vector<HistInfo*> histograms;

  int binlimit = 9;
  double padratio = 3, heightratio = 15, rebinlimit = 0.3, dooverflow= 0;
  bool dividebins = false;
  vector<int>  color;
  int color_iter;
};

#endif
