#include "Plotter.h"
#include <iostream>     
#include <string>
#include <sstream>
#include <typeinfo>
using namespace std;


const double EPSILON_VALUE = 0.0001;


template <typename T>
string to_string_with_precision(const T a_value, const int n = 6);

/// Main function.  Takes TDirectory with the file you are writing to.  Also takes in logfile
//  Gets all of the graphs from files in the plotter class and puts them in one graph.
void Plotter::CreateStack( TDirectory *target) {

  //// Sets up style here.  Does everytime, just in case.  Probably don't need
  gStyle = styler->getStyle();
  styler->setTDRStyle();
  
  for(auto histos: styler->getHistograms()) {
    setupHistogram();
    TH1D* refHisto = 0;
    TH1D* fillHisto = 0;
    const char* histname = histos->getHistname();

    referenceFile->cd(styler->getDir());
    referenceFile->GetObject(histname, refHisto);
    cout << histname << endl;
    double lowbin = (histos->getLow() < 100000) ? histos->getLow() : refHisto->GetXaxis()->GetXmin();
    auto bins_test = getBinning(histos->getBinning(), lowbin ,refHisto->GetXaxis()->GetXmax() );
    int nBins = numberBinning(histos->getBinning());
    nBins = nBins > 0 ? nBins : (refHisto->GetXaxis()->GetXmax() - lowbin)/histos->getBinning().at(0).second;

    TH1D* errorHist = new TH1D("errorHist", "errorHist", nBins, bins_test);
    TList* dataHistos = createWorkingList(&dataFiles, histname, nBins, bins_test, errorHist);
    TList* bgHistos = createWorkingList(&bgFiles, histname, nBins, bins_test, errorHist);
    TList* sigHistos = createWorkingList(&sigFiles, histname, nBins, bins_test, errorHist);

    setSignal(sigHistos);
    THStack* stack = getStack(bgHistos, histname, histos->useSortSmToLg());

    TLegend* leg = createLeg(stack->GetHists(), sigHistos);
    target->cd();
    TCanvas *c = new TCanvas(histname, histname);//403,50,600,600);
    if(styler->doStackSignal()) {
      for(auto sigh : *sigHistos) stack->Add((TH1D*)sigh);
    }

    TGraphErrors* errorstack = createError(errorHist);
    
    stack->Draw();
    stack->SetTitle(histos->getTitle());
    stack->GetXaxis()->SetTitle(histos->getXaxisTitle());
    stack->GetYaxis()->SetTitle(histos->getYaxisTitle());
    double maximum = stack->GetMaximum();
    stack->SetMaximum(maximum*styler->getMaxRatio());
    
    if(!styler->doStackSignal()) {
      for(auto sigh : *sigHistos) sigh->Draw("same");
    }

    errorstack->Draw("2");
    leg->Draw();
    c->cd();
    styler->CMS_lumi((TPad*)gPad);
    c->Write(c->GetName());
    c->Close();
    delete [] bins_test;
    delete stack;
    delete leg;
    delete errorHist;
    delete errorstack;
  }

}  

TList* Plotter::createWorkingList(TList* fileList, const char* name, int nBins, double* rebinning, TH1D* errorHist) {
  TH1D* tmphist = 0;
  TList* rebinnedHistos = new TList();
  for(auto file : *fileList) {
    ((TFile*)file)->cd(styler->getDir());
    ((TFile*)file)->GetObject(name, tmphist);
    rebinnedHistos->Add((TH1D*)tmphist->Rebin(nBins, file->GetTitle(), rebinning));
  }

  for(auto files : *rebinnedHistos) {
    TH1D* workfile = (TH1D*)files;
    int Nbins = workfile->GetXaxis()->GetNbins();
    workfile->SetBinContent(Nbins,workfile->GetBinContent(Nbins)+workfile->GetBinContent(Nbins+1));
    errorHist->Add(workfile);
  }
  return rebinnedHistos;
}


THStack* Plotter::getStack(TList* histos, const char* histoName, bool sortSmToLg) {
  TH1D* tmp = 0;
  THStack* stack = new THStack("stack", "stack");
  
  for(auto hwork : *histos) {
    int color = styler->getColor();
    tmp = (TH1D*)hwork;
    for(int j = 1; j < tmp->GetXaxis()->GetNbins()+1; j++) {
      tmp->SetBinError(j, 0);
    }
    
    tmp->SetLineColor(TColor::GetColorDark(color));
    tmp->SetLineWidth(2);
    tmp->SetFillColor(color);
      
    stack->Add(tmp);
  }

  stack  = sortStack(stack, sortSmToLg);
  return stack;
}

void Plotter::setSignal(TList* signal) {
  for(auto obj : *signal) {
    int color = styler->getColor();
    TH1D* hist = (TH1D*)obj;
    for(int j = 1; j < hist->GetXaxis()->GetNbins()+1; j++) {
      hist->SetBinError(j,0);
    }
    hist->SetLineColor(color);
    hist->SetLineWidth(4);
    hist->SetLineStyle(7);
  }
}


///// Function takes an old THStack and sorts the stack based on
//// integral of the graph, smallest to largest.
/// deletes old stack so pointer nonsense doesn't happen
THStack* Plotter::sortStack(THStack* old, bool doSortSToL) {
  if(old == NULL || old->GetNhists() == 0) return old;
  string name = old->GetName();
  THStack* newstack = new THStack(name.c_str(),name.c_str());

  TList* list = (TList*)old->GetHists();

  while(list->GetSize() > 1) {

    TH1* interest = nullptr;
    for(auto tmp : *list) {
      if(interest == nullptr) interest = (TH1*)tmp;
      else if(doSortSToL && interest->Integral() > ((TH1*)tmp)->Integral()) interest = (TH1*)tmp;
      else if(!doSortSToL && interest->Integral() < ((TH1*)tmp)->Integral()) interest = (TH1*)tmp;
    }
    newstack->Add(interest);
    list->Remove(interest);
  }
  TH1* last = (TH1*)list->First();
  if(last) newstack->Add(last);

  delete old;
  return newstack;
}




////make legend position adjustable
//// Puts all of the histograms in the legend.  Set so backgrounds show up
/// as colored blocks, but signal and data as T's in the format they are on the graph
TLegend* Plotter::createLeg(const TList* bgl, const TList* sigl) {
  double width = 0.05;

  TLegend* leg = new TLegend(0.79, 0.92-(getSize()+1)*width ,0.94,0.92);

  //  if(dataFiles.GetSize() != 0) leg->AddEntry(data, "Data", "lep");

  for(auto histos : *bgl) {
    leg->AddEntry(histos, histos->GetName(), "f");
  }

  for(auto histos: *sigl) {
    leg->AddEntry(histos, histos->GetName(), "f");
  }

  TH1* mcErrorleg = new TH1I("mcErrorleg", "BG stat. uncer.", 100, 0.0, 4.0);
  mcErrorleg->SetLineWidth(1);
  mcErrorleg->SetFillColor(1);
  mcErrorleg->SetFillStyle(3013);
  mcErrorleg->SetLineColor(1);
  leg->AddEntry(mcErrorleg,"BG stat. uncer.","f");
  leg->SetFillStyle(0);

  delete mcErrorleg;
  return leg;
}



//// get the number of files total in the plotter
int Plotter::getSize() {
  return dataFiles.GetSize() + bgFiles.GetSize() + sigFiles.GetSize();
}


//// Get the filenames (have options for different lists).
/// puts the names into a vector.  Mainly used for the logfile class
vector<string> Plotter::getFilenames(string option) {
  vector<string> filenames;

  TList* worklist;
  if(option == "data") worklist = &dataFiles;
  else if(option == "background") worklist = &bgFiles;
  else if(option == "signal") worklist = &sigFiles;
  else if(option == "all") {

    filenames = getFilenames("data");
    vector<string> tmp = getFilenames("background");
    filenames.insert(filenames.end(), tmp.begin(), tmp.end());
    tmp = getFilenames("signal");
    filenames.insert(filenames.end(), tmp.begin(), tmp.end());
    return filenames;

  } else {
    cout << "Error in filename option, returning empty vector" << endl;
    return filenames;
  }

  for(auto files : *worklist) {
    filenames.push_back(files->GetTitle());
  }
  return filenames;

}

//// set style given to the style of the plotter class
void Plotter::setStyle(Style* style) {
  this->styler = style;
}


/// Add Normalized file to plotter
void Plotter::addFile(Normer& norm) {
  string filename = norm.output;
  if(norm.use == 0) {
    cout << filename << ": Not all files found" << endl << endl;
    return;
  }

  while(filename.find("#") != string::npos) {
    filename.erase(filename.find("#"), 1);
  }

  TFile* normedFile = NULL;
  if(norm.use == 1) {
    norm.print();
    norm.FileList = new TList();
    for(auto name: norm.input) {
      norm.FileList->Add(TFile::Open(name.c_str()));
    }

    normedFile = new TFile(filename.c_str(), "RECREATE");
    norm.MergeRootfile(normedFile);
  } else if(norm.use == 2) {
    cout << filename << " is already Normalized" << endl << endl;;
    normedFile = new TFile(filename.c_str());
  }
  string legName = (norm.output).substr(0, (norm.output).length()-5);
  normedFile->SetTitle(legName.c_str());

  if(referenceFile == nullptr) referenceFile = normedFile;
  
  if(norm.type == "data") dataFiles.Add(normedFile);
  else if(norm.type == "bg") bgFiles.Add(normedFile);
  else if(norm.type == "sig") sigFiles.Add(normedFile);

}



double* Plotter::getBinning(vector<pair<int, double>> binning, double start, double end) {
  int numberBins = numberBinning(binning);
  if(numberBins < 0) {
    if(binning.at(0).second == -1) return nullptr;
    else {
      double binsize = binning.at(0).second;
      numberBins = (end - start)/binsize;
      binning.clear();
      binning.push_back(make_pair(numberBins, binsize));
    }
  } 


  double* returnBins = new double[numberBins+1];
  double current = start;
  returnBins[0] = start;
  int binN = 1;

  for(auto bins : binning) {
    for(int n = 0; n < bins.first; n++) {
      current += bins.second;
      returnBins[binN] = current;
      binN++;
    }
  }

  return returnBins;
}

int Plotter::numberBinning(vector<pair<int, double>> &binning) {
  int count = 0;
  for(auto bins : binning) {
    count += bins.first;
  }
  return count;
}



void Plotter::setupHistogram() {
  styler->resetColoring();

}

vector<string> Plotter::getDirectories() {
  TList* tmplist = referenceFile->GetListOfKeys();
  vector<string> dirs;
  for(auto key : *tmplist) {
    TObject* obj = ((TKey*)key)->ReadObj();
    if ( obj->IsA()->InheritsFrom( TDirectory::Class() ) ) {
      if(strcmp(obj->GetTitle(),"Eff") == 0) continue;
      dirs.push_back(obj->GetTitle());
    }
  }
  return dirs;
}

vector<pair<double,double>> Plotter::eventInfo(string path) {
  vector<pair<double,double>> info = eventInfoHelper(path, dataFiles);
  vector<pair<double,double>> tmp = eventInfoHelper(path, bgFiles);
  info.insert(info.end(), tmp.begin(), tmp.end());
  tmp = eventInfoHelper(path, sigFiles);
  info.insert(info.end(), tmp.begin(), tmp.end());
  return info;
}

vector<pair<double, double>> Plotter::eventInfoHelper(string path, TList& list) {
  string filename = "Met";
  vector<pair<double, double>> info;
  for(auto file : list) {
    ((TFile*)file)->cd(path.c_str());
    TH1D* tmp = 0;
    gDirectory->GetObject((filename.c_str()), tmp);
    if(tmp == nullptr) break;
    double error = 0;
    double total = tmp->IntegralAndError(0,tmp->GetXaxis()->GetNbins()+1, error);
    info.push_back(make_pair(total, error));
  }
  for(auto p : info) cout << p.first << " " << p.second << endl;
  return info;
}

TGraphErrors* Plotter::createError(const TH1* error) {
  int Nbins =  error->GetXaxis()->GetNbins();
  Double_t* mcX = new Double_t[Nbins];
  Double_t* mcY = new Double_t[Nbins];
  Double_t* mcErrorX = new Double_t[Nbins];
  Double_t* mcErrorY = new Double_t[Nbins];

  for(int bin=0; bin < error->GetXaxis()->GetNbins(); bin++) {
    mcY[bin] = error->GetBinContent(bin+1);
    mcErrorY[bin] = error->GetBinError(bin+1);
    mcX[bin] = error->GetBinCenter(bin+1);
    mcErrorX[bin] = error->GetBinWidth(bin+1) * 0.5;
  }
  TGraphErrors *mcError = new TGraphErrors(error->GetXaxis()->GetNbins(),mcX,mcY,mcErrorX,mcErrorY);

  mcError->SetFillColor(1);
  mcError->SetFillStyle(3013);
  mcError->SetMarkerStyle(20);
  mcError->SetMarkerStyle(0);

  delete[] mcX;
  delete[] mcY;
  delete[] mcErrorX;
  delete[] mcErrorY;
  return mcError;
}

