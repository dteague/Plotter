unordered_map<string, string> Plotter::latexer = { {"GenTau", "#tau"}, {"GenHadTau", "#tau_{h}"}, {"GenMuon", "#mu"}, {"TauJet", "#tau"}, {"Muon", "#mu"}, {"DiMuon", "#mu, #mu"}, {"DiTau", "#tau, #tau"}, {"Tau", "#tau"}, {"DiJet", "jj"}, {"Met", "#slash{E}_{T}"}, {"BJet", "b"}};


///// macro to save space.  Tests if name is in latex map, if not, it just uses the name given
#define turnLatex(x) ((latexer[x] == "") ? x : latexer[x])



//// A lot of regex stuff to extract the x axis label from the title
// of the graph.  If the xaxis label is wrong, change the code here.
string Plotter::newLabel(string stringkey) {
  string particle = "";

  smatch m;
  regex part ("(Di)?(Tau(Jet)?|Muon|Electron|Jet)");

  regex e ("^(.+?)(1|2)?Energy$");
  regex n ("^N(.+)$");
  regex charge ("^(.+?)(1|2)?Charge");
  regex mass ("(.+?)(Not)?Mass");
  regex zeta ("(.+?)(P)?Zeta(1D|Vis)?");
  regex deltar ("(.+?)DeltaR");
  regex MetMt ("^(([^_]*?)(1|2)|[^_]+_(.+?)(1|2))MetMt$");
  regex eta ("^(.+?)(Delta)?(Eta)");
  regex phi ("^(([^_]*?)|[^_]+_(.+?))(Delta)?(Phi)");
  regex cosdphi ("(.+?)(CosDphi)(.*)");
  regex pt ("^(.+?)(Delta)?(Pt)(Div)?.*$");
  regex osls ("^(.+?)OSLS");
  regex zdecay ("^[^_]_(.+)IsZdecay$");


  if(regex_match(stringkey, m, e)) {
    return "E(" + turnLatex(m[1].str()) + ") [GeV]";
  }
  else if(regex_match(stringkey, m, n)) {
    return "N(" + turnLatex(m[1].str()) + ")";
  }
  else if(regex_match(stringkey, m, charge)) {
    return  "charge(" + turnLatex(m[1].str())+ ") [e]";
  }
  else if(regex_match(stringkey, m, mass)) {
    return ((m[2].str() == "Not") ? "Not Reconstructed M(": "M(") + listParticles(m[1].str()) + ") [GeV]";
  }
  else if(regex_match(stringkey, m, zeta)) {
    return m[2].str() + "#zeta" +((m[3].str() != "") ? "_{" + m[3].str() + "}":"") + "(" + listParticles(m[1].str()) + ")";
  }
  else if(regex_match(stringkey, m, deltar)) {
    return "#DeltaR("+ listParticles(m[1].str()) + ")";
  }
  else if(regex_match(stringkey, m, MetMt)) {
    return "M_{T}(" + turnLatex(m[2].str()+m[4].str()) + ") [GeV]";
  }
  else if(regex_match(stringkey, m, eta))  {
    particle += (m[2].str() != "") ? "#Delta" : "";
    particle += "#eta(" + listParticles(m[1].str()) + ")";
    return particle;
  }
  else if(regex_match(stringkey, m, phi))  {
    if(m[4].str() != "") particle = "#Delta";
    particle += "#phi(" + listParticles(m[2].str()+m[3].str()) + ")";
    return particle;
  }
  else if(regex_match(stringkey, m, cosdphi)) {
    return "cos(#Delta#phi(" + listParticles(m[1].str()) + "))";
  }
  else if(regex_match(stringkey, m, pt)) {
    if(m[4].str() != "") particle += "#frac{#Delta p_{T}}{#Sigma p_{T}}(";
    else if(m[2] != "") particle += "#Delta p_{T}(";
    else particle += "p_{T}(";
    particle += listParticles(m[1].str()) + ") [GeV]";
    return particle;
  }
  else if(stringkey.find("Met") != string::npos) return "#slash{E}_{T} [GeV]";
  else if(stringkey.find("MHT") != string::npos) return "#slash{H}_{T} [GeV]";
  else if(stringkey.find("HT") != string::npos) return "H_{T} [GeV]";
  else if(stringkey.find("Meff") != string::npos) return "M_{eff} [GeV]";
  else if(regex_match(stringkey, m, osls)) {
    particle = m[1].str();
    string full = "";
    while(regex_search(particle, m, part)) {
      full += "q_{" + turnLatex(m[0].str()) + "} ";
      particle = m.suffix().str();
    }
    return full;
  } else if(regex_match(stringkey, m, zdecay)) {
    return listParticles(m[1].str()) + "is Z Decay";
  }

  return stringkey;

}

////// helper function for newLabel.  Takes a section of the string
/// and extracts the particles from it, then turns them into latex format.
/// seperated by comma
//// More robust would be vector<string> so could use different formatting
string Plotter::listParticles(string toParse) {
  smatch m;
  regex part ("(Di)?(Tau(Jet)?|Muon|Electron|Jet|Met)");
  bool first = true;
  string final = "";

  while(regex_search(toParse,m,part)) {
    if(first) first = false;
    else final += ", ";
    final += turnLatex(m[0].str());
    toParse = m.suffix().str();
  }
  return final;
}
