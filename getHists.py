import ROOT

def get_Histos(filename, stackname, hist_stack_name, padName, histname, histlist) :
    rtfile = ROOT.TFile(filename)
    canvas = rtfile.Get(canvas_name)
    canvas.GetListOfPrimitives().Print()
    stackPad = canvas.GetListOfPrimitives().FindObject(padName)
    stack = stackPad.GetListOfPrimitives().FindObject(hist_stack_name)
    print stack
    histos = {}
    for i in hist_list:
        histos[i] = stack.GetHists().FindObject(i+"_"+hist_name)
    return histos

rtfile_name = "Kenneth_file/leadJetPt.root"
canvas_name = "jetPt[0]_canvas"
hist_stack_name = "stack_jetPt[0]"
hist_name = "WZxsec2016_VBSselection_jetPt[0]"
hist_list=['wz-mgmlm',
           'top-ewk',
           'vv',
           'nonprompt',
           'zg',
           'wzjj-ewk'
]

my_canvas_name = "all/hall_leadjet_Pt"

Ken_histo = get_Histos(rtfile_name, canvas_name, hist_stack_name, "stackPad", hist_name, hist_list )
#my_histo = get_Histos(myfile_name, my_canvas_name, "blah")
my_histo = {}
file = ROOT.TFile("output.root")
key = file.GetKey(my_canvas_name)
canvas = key.ReadObj()
stack = canvas.GetListOfPrimitives().FindObject("stack")
for i in stack.GetHists():
    my_histo[i.GetName()] = i
    print i.GetName()



from ROOT import TCanvas, TPad, TFormula, TF1, TPaveLabel, TH1F, TFile
from ROOT import gROOT, gBenchmark

divide_histo = TH1F()
divide_histo.Copy(Ken_histo["wzjj-ewk"])
divide_histo.Divide(my_histo["WZ-EWK"])

c1 = TCanvas( 'c1', 'The FillRandom example', 200, 10, 700, 900 )
Ken_histo["wzjj-ewk"].Draw()
input("Press Enter to continue...")

Ken_histo["wzjj-ewk"].Scale(1./Ken_histo["wzjj-ewk"].Integral())
my_histo["WZ-EWK"].Scale(1./my_histo["WZ-EWK"].Integral())

c1 = TCanvas( 'c1', 'The FillRandom example', 200, 10, 700, 900 )
pad1 = TPad( 'pad1', 'The pad with the function',  0.05, 0.50, 0.95, 0.95 )
pad2 = TPad( 'pad2', 'The pad with the histogram', 0.05, 0.05, 0.95, 0.45 )
pad1.Draw()
pad2.Draw()
pad1.cd()
Ken_histo["wzjj-ewk"].Draw()
my_histo["WZ-EWK"].Draw("same")
pad2.cd()
divide_histo.Draw()
c1.Update()
input("Press Enter to continue...")

#canvas.GetListOfPrimitives()




