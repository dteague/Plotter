#ifndef _NORMALIZER_H_
#define _NORMALIZER_H_

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

using namespace std;

class Normer {
 public:  
  Normer();
  Normer(const Normer& other);
  Normer& operator=(const Normer& rhs);
  ~Normer();


  vector<string> input;
  vector<double> skim, xsec;
  string output, type="";
  double lumi;

  TList* FileList;
  vector<double> integral, CumulativeEfficiency, scaleFactor, scaleFactorError;
  bool isData=false;
  int use=3;

  void MergeRootfile( TDirectory*);
  void print();
};

#endif