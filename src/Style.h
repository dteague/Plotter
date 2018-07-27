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
#include "TPad.h"
#include "TLatex.h"
#include "TLine.h"
#include "TBox.h"
#include "TImage.h"
#include <TColor.h>

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
  Style(string, double);
  ~Style();

  const vector<HistInfo*>& getHistograms() {return histograms;}
  
  void read_info(string);
  void setStyle();
  TStyle* getStyle();
  void fixOverlay();
  void setTDRStyle();
  void tdrGrid(bool);
  int getColor();
  void resetColoring();
  void CMS_lumi( TPad* pad, int iPosX=10 );
  const char* getDir() { return directory.c_str();}
  double getMaxRatio() { return maxRatio;}
  
  bool doStackSignal() {return stackSignal;}
  
 private:
  void setupGeneral(ConfigFile&);

  TStyle *tdrStyle;
  double lumi;

  vector<HistInfo*> histograms;

  int color_iter;

  ///////CMS text//////
  
  TString cmsText     = "CMS";
  float cmsTextFont   = 61;  // default is helvetic-bold
  float extraTextFont = 52;  // default is helvetica-italics
  float lumiTextSize     = 0.6;
  float lumiTextOffset   = 0.2;
  float cmsTextSize      = 0.75;
  float cmsTextOffset    = 0.1;  // only used in outOfFrame version
  float relPosX    = 0.045;
  float relPosY    = 0.035;
  float relExtraDY = 1.2;
  float extraOverCmsTextSize  = 0.76;  // ratio of "CMS" and extra text size


  ////////////////////////////////////////////
  /////// Values set in config ///////////////
  ////////////////////////////////////////////

  string extraText;
  string lumiText;
  bool drawLogo;
  bool writeExtraText;
  vector<int>  color;
  bool stackSignal;
  string directory;
  double maxRatio;
};

#endif
