#include <TStyle.h>
#include "TMath.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TPaveStats.h"
#include "TCut.h"
#include "TH1.h"
#include "TF1.h"
#include "TLegend.h"

#include "RooFit.h"
#include "RooRealVar.h"
#include "RooCategory.h"
#include "RooAbsPdf.h"
#include "RooResolutionModel.h"
#include "RooFitResult.h"
#include "RooDataSet.h"
#include "RooGlobalFunc.h"
#include "RooProdPdf.h"
#include "RooGaussian.h"
#include "RooAddPdf.h"
#include "RooPlot.h"
#include "RooGaussModel.h"
#include "RooLandau.h"
#include "RooTruthModel.h"
#include "RooLandau.h"
#include "RooProdPdf.h"
#include "RooChebychev.h"
#include "RooExponential.h"
#include "RooDecay.h"
#include "RooPolynomial.h"
#include "RooGenericPdf.h"
#include "RooHistPdf.h"
#include "RooHist.h"
#include "RooBreitWigner.h"
#include "RooVoigtian.h"
#include "RooWorkspace.h"

#include "RooDataHist.h"
#include "RooKeysPdf.h"
#include "RooNDKeysPdf.h"

#include <vector>

using namespace RooFit;

Bool_t dominos = kTRUE  ;
Int_t ncpu = 2;
Int_t binfit=500/5;
Int_t Binsh = 25;
bool dopause = true;


void  signif_bkgsub() {

    //LOAD WORKSPACE, MODELS, VARIABLES, DATA
    TFile *fwsp = new TFile("workspace.root") ;

    RooWorkspace* wrk = (RooWorkspace*) fwsp->Get("w") ;
    //wrk->Print("v");
                            
    RooRealVar* M = wrk->var("M") ;
    RooRealVar* P = wrk->var("P") ;
    
    //RooAbsPdf* model = wrk->pdf("model") ;
    RooAbsPdf* Sig_pdf = wrk->pdf("Sig_pdf") ;
    RooAbsPdf* M_pdf = wrk->pdf("M_pdf") ;
    //RooAbsPdf* P_pdf = wrk->pdf("P_pdf") ;
    RooAbsPdf* M_sig_pdf = wrk->pdf("M_sig_pdf") ;
    RooAbsPdf* M_bkg_pdf = wrk->pdf("M_bkg_pdf") ;
    RooAbsPdf* P_sig_pdf = wrk->pdf("P_sig_pdf") ;
    //RooAbsPdf* P_bkg_pdf = wrk->pdf("P_bkg_pdf") ;

    RooAbsData* data = wrk->data("data") ;
    RooAbsData* data_SB = wrk->data("data_SB") ;

    Double_t min_P = P->getMin();
    Double_t max_P  = P->getMax();

    RooRealVar* Ns = wrk->var("Ns") ;
    RooRealVar* Nb = wrk->var("Nb") ;
    RooRealVar* M_mean = wrk->var("M_mean") ;
    RooRealVar* M_sigma = wrk->var("M_sigma") ;
    RooRealVar* M_c1 = wrk->var("M_c1") ;
    RooRealVar* SBlow = wrk->var("SBlow") ;
    RooRealVar* SBhigh = wrk->var("SBhigh") ;
    
    Int_t Ns_ini = Ns->getVal();
    
    
    //FIT FULL REGION AND PLOT
    wrk->loadSnapshot("sidebands_fit"); //helps convergence of next fit.
    RooFitResult* fit_M = M_pdf->fitTo(*data,Minos(dominos), Save(kTRUE), NumCPU(ncpu), Extended(),Verbose(kFALSE),PrintLevel(-1),PrintEvalErrors(-1));
    fit_M->Print("v");
    
    //Plot results
    TCanvas *c = new TCanvas("c",Form("Signficance and bkg subtraction analysis"),50,50,1150,500);
    c->Divide(2,1);
    c->cd(1);

    RooPlot* M_frame_3 = M->frame(Bins(binfit),Title("Mass fit and local signal significance"));
    data->plotOn(M_frame_3);
    M_pdf->plotOn(M_frame_3,Components(*M_sig_pdf),LineColor(kRed),LineStyle(kDashed));
    M_pdf->plotOn(M_frame_3,Components(*M_bkg_pdf),LineColor(kGray),LineStyle(kDashed));
    M_pdf->plotOn(M_frame_3, VisualizeError(*fit_M, 1,kFALSE),FillColor(kBlue));
    M_pdf->paramOn(M_frame_3, Format("NELU", AutoPrecision(1)), Layout(0.57, 0.99,0.95) );
    data->plotOn(M_frame_3);

    
    //ESTIMATION OF THE SIGNAL SIGNFICANCE BASED ON MASS PEAK ONLY
    //https://en.wikipedia.org/wiki/Wilks%27_theorem
    //https://www.pp.rhul.ac.uk/~cowan/stat/cowan_berkeley_4jan17.pdf
    
    //fit bkg only hypothesis
    M_mean->setConstant(kTRUE); //fix mean
    M_sigma->setConstant(kTRUE);  // fix width
    Ns->setVal(0.0);  //  Zero signal hypothesis
    Ns->setConstant(kTRUE);  // fixed
    // Ns->removeError();
    RooFitResult* fit_M_bkg = M_pdf->fitTo(*data,Minos(dominos), Save(kTRUE), NumCPU(ncpu), Extended(),Verbose(kFALSE),PrintLevel(-1),PrintEvalErrors(-1));
    fit_M_bkg->Print("v");
    
    //Significance from LR:
    Double_t minNLLAll = fit_M->minNll();  // sig&bkg -ln(L_tot)
    Double_t minNLLBkg = fit_M_bkg->minNll(); // bkg -ln(L_tot).
    cout<<" -Log Likelihood values for sig&bkg and bkg only : "<<minNLLAll<<"  "<<minNLLBkg<<endl;
    cout<<" -Log likelihood ratio : "<<minNLLBkg - minNLLAll<<endl; // -ln(L_bkg) - -ln(L_tot) = -ln[L_bkg/L_tot] = -ln(R)
    Double_t sigLR = TMath::Sqrt(2.*(minNLLBkg - minNLLAll)); // sqrt[-2*ln(R)]
    cout<<" Significance from LR : "<<sigLR<<endl;  //Final peak signficance from LR.
    
    TPaveText *ptext_3 = new TPaveText(0.15,0.8,0.4,0.89,"NDC");
    ptext_3->AddText(Form("Signal signif. = %0.1f#sigma", sigLR ));
    ptext_3->AddText("from #sqrt{-2ln(#color[3]{L_{bkg}}/#color[4]{L_{tot}})}");

    ptext_3->SetBorderSize(0); ptext_3->SetFillColor(0); ptext_3->SetTextSize(0.04); ptext_3->SetTextFont(42);
    M_frame_3->addObject(ptext_3);

    //Plot bkg only hypothesis fit.
    M_pdf->plotOn(M_frame_3,LineColor(kGreen));

    c->cd(1); M_frame_3->GetYaxis()->SetTitleOffset(1.5);  M_frame_3->Draw();
    if(dopause) { gPad->WaitPrimitive(); gPad->WaitPrimitive(); } //Double-Click in pad to show next
    
    
    //DO BACKGROUND SUBTRACTION TO SIGNAL MOMENTUM SHAPE (assume unknown momentum model).
    
    //Recover and use full fit results, to be used from now on. (could also use snapshot)
    Ns->setVal( ((RooRealVar*)fit_M->floatParsFinal().find(*Ns))->getVal() );
    Nb->setVal( ((RooRealVar*)fit_M->floatParsFinal().find(*Nb))->getVal() );
    M_mean->setVal( ((RooRealVar*)fit_M->floatParsFinal().find(*M_mean))->getVal() );
    M_sigma->setVal( ((RooRealVar*)fit_M->floatParsFinal().find(*M_sigma))->getVal() );
    M_c1->setVal( ((RooRealVar*)fit_M->floatParsFinal().find(*M_c1))->getVal() );
    
    c->cd(2);
    gStyle->SetOptStat(1101);
    
    //This will be the final bkg subtracted histogram
    TH1F *h1 = new TH1F("Bkg. sub. P","Bkg. sub. Data vs MC momentum",Binsh,min_P,max_P);
    
    //We will compare with generated real model events.
    //RooDataSet* data_sig = Sig_pdf->generate(RooArgSet(*M,*P), Ns_ini, Name("data_sig"), Verbose(kFALSE));
    RooDataSet* data_sig = P_sig_pdf->generate(RooArgSet(*P), Ns_ini,Verbose(kFALSE)); //10*Ns_ini

    TH1F *h2 = (TH1F*) data_sig->createHistogram("MC", *P, Binning(Binsh,min_P,max_P));
    h2->SetTitle("Signal MC pseudodata");
    h2->SetLineColor(kRed);
    h2->SetLineWidth(2);
    h2->SetLineStyle(1);
    h2->Sumw2();
    
    //Remember "data" is our real data. Will create 2 histograms of P: g1 (SR) and g2 (SB).
    string SRcut = Form("M> %f && M < %f",SBlow->getVal(),SBhigh->getVal());
    RooDataSet* data_SR = (RooDataSet*) data->reduce(Cut( SRcut.c_str() )); //data_SB previously defined
    TH1F *g1  = (TH1F*) data_SR->createHistogram("Data SR", *P, Binning(Binsh,min_P,max_P)); //Signal region
    TH1F *g2  = (TH1F*) data_SB->createHistogram("Data SB", *P, Binning(Binsh,min_P,max_P)); //Sidebands
    g1->Sumw2(); g2->Sumw2();
    
    //Obtain background fraction in SR according to the complete fitted model.
    M->setRange("SR",SBlow->getVal(),SBhigh->getVal()) ;
    RooAbsReal* fs_SR_fit = M_sig_pdf->createIntegral(*M, *M, "SR");
    RooAbsReal* fb_SR_fit = M_bkg_pdf->createIntegral(*M, *M, "SR");
    Double_t Ns_SR_fit = Ns->getVal() *  fs_SR_fit->getVal();
    Double_t Nb_SR_fit = Nb->getVal() *  fb_SR_fit->getVal();
    Double_t fb_SR_full = Nb_SR_fit / ( Ns_SR_fit + Nb_SR_fit );
    
    //Subtract sidebands shape (correctly normalized to bkg yield in SR) to SR shape.
    // (from right to left) Factor will normalize g2 area to 1 ; then normalize to events in SR;
    // finally, fb_SR_full will give the correct bkg yield to subtract to g1 (SR histogram).
    Double_t Factor = -1. * fb_SR_full * g1->GetEntries() / g2->GetEntries();
    h1->Add(g1,g2,1.,Factor);

    h1->SetLineColor(kBlue);
    h1->GetXaxis()->SetTitle("Momentum (GeV/c)");
    h1->SetMarkerStyle(20);
    h1->SetMarkerColor(kBlue);

    h1->Draw("p"); //p show marker
    h2->Draw("samesHISTE"); //sames to paint stats, HIST to paint histogram, E to paint errors.
    
    //Let's also compare with data histogram
    g1->SetLineColor(kBlack);
    g1->SetLineWidth(1);
    g1->SetLineStyle(2);
    g1->Draw("samesHISTE");


    gPad->Update();

    TPaveStats *st1 = (TPaveStats*)h1->FindObject("stats");
    TPaveStats *st2 = (TPaveStats*)h2->FindObject("stats");
    TPaveStats *st3 = (TPaveStats*)g1->FindObject("stats");
    st1->SetTextColor(kBlue); st2->SetTextColor(kRed); st3->SetTextColor(kBlack);
    st1->SetX1NDC(0.85); st2->SetX1NDC(0.85); st3->SetX1NDC(0.85);
    st1->SetX2NDC(0.99); st2->SetX2NDC(0.99); st3->SetX2NDC(0.99);
    st1->SetY1NDC(0.86); st2->SetY1NDC(0.86 - 0.15);  st3->SetY1NDC(0.86 - 2*0.15);
    st1->SetY2NDC(0.98); st2->SetY2NDC(0.98 - 0.15);  st3->SetY2NDC(0.98 - 2*0.15);
 
    st1->Draw(); st2->Draw(); st3->Draw();
 
    //Normalize MC to data (this depends on your analysis; here it is only to compare shapes).
    //h2->Scale(h1->Integral()/h2->Integral());

    //Normalize data histogram to MC hist, just for comparison
    g1->Scale(h2->Integral()/g1->Integral());
    
    //Change max of pad, if needed.
    //Double_t binerror = h1->GetBinError(h1->GetMaximumBin());
    //h1->SetMaximum(1.1*h1->GetMaximum() + binerror);

    //Report kolmogorov-smirnov
    TPaveText *ptt = new TPaveText(0.48,0.7,0.72,0.8,"brNDC");
    ptt->SetBorderSize(0); ptt->SetFillColor(0); ptt->SetTextFont(42); ptt->SetTextSize(0.04); ptt->SetTextFont(42);
    Double_t kolmo  = h1->KolmogorovTest(h2);
    Double_t kolmo2 = g1->KolmogorovTest(h2);
    ptt->AddText(Form("KS Data bkg. sub vs MC = %0.2f%%", kolmo*100.));
    ptt->AddText(Form("KS Data SR vs MC = %0.2f%%", kolmo2*100.));
    ptt->Draw();
    
    c->Print("results.png");
}
