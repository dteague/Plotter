#include "Style.h"


Style::Style() {
  styler = new TStyle("Styler", "Style");

}

Style::Style(string infile) {
  styler = new TStyle("Styler", "Style");
  
  ConfigFile config(infile);

  setupGeneral(config);
  for(auto histname : config) {
    if(histname == "General") continue;
    else {
      HistInfo* hist = new HistInfo(histname, config);
      histograms.push_back(hist);
    }
  }
}



Style::~Style() {
  //  delete styler;
}


void Style::setupGeneral(ConfigFile& config) {
  string color_line = config.Value("General", "Colors");
  string current = "";
  for(auto it : color_line) {
    if(it == '[' || isspace(it)) continue;
    else if(it == ']' || it == ',') {
      color.push_back(stoi(current));
      current = "";
    } else current.push_back(it);
  }
}



TStyle* Style::getStyle() {
  return styler;
}

int Style::getColor() {
  return color.at(color_iter++);
}

void Style::resetColoring() {
  color_iter=0;
}


