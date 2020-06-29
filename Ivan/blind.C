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
#include "RooBernstein.h"

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
void suppresswarnings();
void resetwarnings();

Bool_t dominos = kTRUE; //kTRUE;
Int_t ncpu = 2;
Int_t binfit=500/5;
bool dopause = true;

void  blind() {

    RooWorkspace *w = new RooWorkspace("w","workspace") ;

    
    // CREATE MASS AND MOMENTUM MODELS
    w->factory("Gaussian::M_sig_pdf( M[5.05,5.55], M_mean[0], M_sigma[0] )");
    w->factory("Polynomial::M_bkg_pdf( M, M_c1[0] )");
    w->factory("SUM::M_pdf( Ns[0]*M_sig_pdf, Nb[0]*M_bkg_pdf )");

    w->factory("Landau::P_sig_pdf( P[0,45], P_mean_s[0], P_sigma_s[0] )");
    w->factory("Landau::P_bkg_pdf( P[0,45], P_mean_b[0], P_sigma_b[0] )");
    w->factory("SUM::P_pdf( Ns*P_sig_pdf, Nb*P_bkg_pdf )");
    
    //Retrieve variables and define sets
    RooRealVar* M = w->var("M") ;
    RooRealVar* P = w->var("P") ;
    RooArgSet* variables = new RooArgSet(*M,*P);
    
    Double_t min_M = M->getMin();
    Double_t max_M = M->getMax();

    RooRealVar* Ns = w->var("Ns") ;
    RooRealVar* Nb = w->var("Nb") ;
    RooRealVar* M_mean = w->var("M_mean") ;
    RooRealVar* M_sigma = w->var("M_sigma") ;
    RooRealVar* M_c1 = w->var("M_c1") ;
    RooRealVar* P_mean_s = w->var("P_mean_s") ;
    RooRealVar* P_mean_b = w->var("P_mean_b") ;
    RooRealVar* P_sigma_s = w->var("P_sigma_s") ;
    RooRealVar* P_sigma_b = w->var("P_sigma_b") ;
    w->factory("SBlow[5.210]"); RooRealVar* SBlow = w->var("SBlow") ;
    w->factory("SBhigh[5.350]"); RooRealVar* SBhigh = w->var("SBhigh") ;

    RooArgSet* params = new RooArgSet(*Ns,*Nb);
    params->add(RooArgSet(*M_mean,*M_sigma,*M_c1));
    params->add(RooArgSet(*SBlow,*SBhigh));
    params->add(RooArgSet(*P_mean_s, *P_sigma_s, *P_mean_b, *P_sigma_b));
    
    //import parameter values from text file.
    params->readFromFile("params_default.txt","READ","B0x");
    params->Print("v") ;

    w->defineSet("parameters", *params);
    w->defineSet("observables", *variables);

    
    //PLOT MODELS:
    Double_t normS = Ns->getVal() / ( Ns->getVal() + Nb->getVal() );
    Double_t normB = Nb->getVal() / ( Ns->getVal() + Nb->getVal() );
    
    RooPlot* M_fr = M->frame(Title("Mass Model"));
    w->pdf("M_sig_pdf")->plotOn(M_fr,LineColor(kRed),LineStyle(kDashed),Normalization(normS,RooAbsReal::Relative));
    w->pdf("M_bkg_pdf")->plotOn(M_fr,LineColor(kGray),LineStyle(kDashed),Normalization(normB,RooAbsReal::Relative));
    w->pdf("M_pdf")->plotOn(M_fr);

    RooPlot* P_fr = P->frame(Title("Momentum Model"));
    w->pdf("P_bkg_pdf")->plotOn(P_fr,LineColor(kGray),LineStyle(kDashed),Normalization(normB,RooAbsReal::Relative));
    w->pdf("P_sig_pdf")->plotOn(P_fr,LineColor(kRed),LineStyle(kDashed),Normalization(normS,RooAbsReal::Relative));
    w->pdf("P_pdf")->plotOn(P_fr);

    TCanvas *c0 = new TCanvas("c0",Form("Models"),100,50,1100,600);
    c0->Divide(2,2);

    c0->cd(1);  M_fr->GetYaxis()->SetTitleOffset(1.5);  M_fr->Draw();
    c0->cd(2);  P_fr->GetYaxis()->SetTitleOffset(1.5);  P_fr->Draw();
    if(dopause) { gPad->WaitPrimitive(); gPad->WaitPrimitive();} //Click in pad to show

    
    //BUILD MULTIDIMENSIONAL MODEL
    w->factory("PROD::Sig_pdf( M_sig_pdf, P_sig_pdf )");
    w->factory("PROD::Bkg_pdf( M_bkg_pdf, P_bkg_pdf )");
    w->factory("SUM::model( Ns*Sig_pdf, Nb*Bkg_pdf )");

    
    //PLOT PROJECTIONS OF 2D MODEL.
    RooAbsPdf* model = w->pdf("model");
    RooAbsPdf* Sig_pdf = w->pdf("Sig_pdf");
    RooAbsPdf* Bkg_pdf = w->pdf("Bkg_pdf");
    RooPlot* M_fr_pr = M->frame(Title("Mass from 2D Model"));
    model->plotOn(M_fr_pr, Components(*Sig_pdf), Project(*P), LineColor(kRed),LineStyle(kDashed));
    model->plotOn(M_fr_pr, Components(*Bkg_pdf), Project(*P), LineColor(kGray), LineStyle(kDashed));
    model->plotOn(M_fr_pr);

    RooPlot* P_fr_pr = P->frame(Title("Momentum from 2D Model"));
    model->plotOn(P_fr_pr,Components(*Sig_pdf), Project(*M),LineColor(kRed),LineStyle(kDashed));
    model->plotOn(P_fr_pr,Components(*Bkg_pdf), Project(*M),LineColor(kGray),LineStyle(kDashed));
    model->plotOn(P_fr_pr);

    c0->cd(3);  M_fr_pr->GetYaxis()->SetTitleOffset(1.5);  M_fr_pr->Draw();
    c0->cd(4);  P_fr_pr->GetYaxis()->SetTitleOffset(1.5);  P_fr_pr->Draw();
    if(dopause) { gPad->WaitPrimitive(); gPad->WaitPrimitive();} //Double-Click in pad to show
    
    c0->Print("models.png");

    
    //CREATE 2D-PSEUDODATA WITH WITH MC ACCEPT-REJECT METHOD:
    //Number of events are not specified in extended models (specify NumEvents(N) for not exteded).
    RooDataSet* data = w->pdf("model")->generate(RooArgSet(*M,*P),Name("data"),Verbose(kFALSE),Extended());
    //w->import(*data, Rename("data"));

    
    //PLOT PSEUDODATA:
    RooAbsPdf* M_pdf = w->pdf("M_pdf");
    RooAbsPdf* P_pdf = w->pdf("P_pdf");
    RooAbsPdf* M_sig_pdf = w->pdf("M_sig_pdf");
    RooAbsPdf* M_bkg_pdf = w->pdf("M_bkg_pdf");
    RooPlot* M_fr_0 = M->frame(Bins(binfit),Title("Mass, pseudodata & model"));
    data->plotOn(M_fr_0);
    M_pdf->plotOn(M_fr_0,Components(*M_sig_pdf),LineColor(kRed),LineStyle(kDashed));
    M_pdf->plotOn(M_fr_0,Components(*M_bkg_pdf),LineColor(kGray),LineStyle(kDashed));
    M_pdf->plotOn(M_fr_0);
    
    RooAbsPdf* P_sig_pdf = w->pdf("P_sig_pdf");
    RooAbsPdf* P_bkg_pdf = w->pdf("P_bkg_pdf");
    RooPlot* P_fr_0 = P->frame(Bins(100),Title("Momentum, pseudodata & model"));
    data->plotOn(P_fr_0);
    P_pdf->plotOn(P_fr_0,Components(*P_sig_pdf),LineColor(kRed),LineStyle(kDashed));
    P_pdf->plotOn(P_fr_0,Components(*P_bkg_pdf),LineColor(kGray),LineStyle(kDashed));
    P_pdf->plotOn(P_fr_0);
    
    TPaveText *M_ptext_0 = new TPaveText(0.65,0.63,0.8,0.85,"NDC");
    M_ptext_0->AddText(Form("Ns = %d", int(Ns->getVal()) ));
    M_ptext_0->AddText(Form("Nb = %d", int(Nb->getVal()) ));
    M_ptext_0->AddText(Form("M_mean = %0.3f", M_mean->getVal() ));
    M_ptext_0->AddText(Form("M_sigma = %0.3f", M_sigma->getVal() ));
    M_ptext_0->AddText(Form("c1 = %0.3f", M_c1->getVal() ));
    M_ptext_0->SetBorderSize(0); M_ptext_0->SetFillColor(0); M_ptext_0->SetTextSize(0.053); M_ptext_0->SetTextFont(42);
    M_fr_0->addObject(M_ptext_0);
    
    TPaveText *P_ptext_0 = new TPaveText(0.65,0.63,0.8,0.85,"NDC");
    P_ptext_0->AddText(Form("P_mean_s = %0.1f", P_mean_s->getVal() ));
    P_ptext_0->AddText(Form("P_mean_b = %0.1f", P_mean_b->getVal() ));
    P_ptext_0->AddText(Form("P_sigma_s = %0.1f", P_sigma_s->getVal() ));
    P_ptext_0->AddText(Form("P_sigma_b = %0.1f", P_sigma_b->getVal() ));
    P_ptext_0->SetBorderSize(0); P_ptext_0->SetFillColor(0); P_ptext_0->SetTextSize(0.053); P_ptext_0->SetTextFont(42);
    P_fr_0->addObject(P_ptext_0);
    
    TCanvas *c1 = new TCanvas("c1",Form("Pseudodata and Sidebands analysis"),100,50,1100,600);
    c1->Divide(2,2);
    
    c1->cd(1);  M_fr_0->GetYaxis()->SetTitleOffset(1.5);  M_fr_0->Draw();
    c1->cd(2);  P_fr_0->GetYaxis()->SetTitleOffset(1.5);  P_fr_0->Draw();
    if(dopause) { gPad->WaitPrimitive(); gPad->WaitPrimitive();} //Double-Click in pad to show
    
    
    // BLIND SIGNAL REGION, CREATE SIDEBANDS DATASET
    string SBcut = Form("M< %f || M> %f",SBlow->getVal(),SBhigh->getVal());
    RooDataSet* data_SB = (RooDataSet*) data->reduce(Cut( SBcut.c_str() ));
    data_SB->SetName("data_SB");
    
    cout << "Data set created with " <<  data->numEntries() << " events" <<endl;
    cout << "Mass sidebands set created with " <<  data_SB->numEntries() << " events" <<endl;
    
    
    // FIT SIDEBANDS DATASET
    M->setRange("R1",min_M,SBlow->getVal()) ;
    M->setRange("R2",SBhigh->getVal(),max_M) ;
    RooFitResult* fit_M_SB = M_bkg_pdf->fitTo(*data_SB, Range("R1,R2"),Minos(dominos), Save(kTRUE), NumCPU(ncpu),Verbose(kFALSE),PrintLevel(-1),PrintEvalErrors(-1)); //Range("R1,R2") //removed range when fitted directly to sidebands
    fit_M_SB->Print("v");
    
    //Can save snapshot of this fit:
    w->saveSnapshot("sidebands_fit", *params, kTRUE); //kTRUE import values of fit in workspace.
    //Can recover these values later with:
    //w->loadSnapshot("sidebands_fit");

    
    // ESTIMATE BKG IN FULL AND SIGNAL REGION (SR)
    M->setRange("SR",SBlow->getVal(),SBhigh->getVal()) ;
    M->setRange("full",min_M,max_M) ;
    RooArgSet* intvars = new RooArgSet(*M);
    RooArgSet* normvars = new RooArgSet(*M);
    RooAbsReal* fb_SB = M_bkg_pdf->createIntegral(*intvars, NormSet(*normvars),Range("R1,R2"));
    RooAbsReal* fb_SR = M_bkg_pdf->createIntegral(*intvars, NormSet(*normvars),Range("SR"));
    Double_t Nb_est = data_SB->numEntries() / fb_SB->getVal();
    Double_t Nb_SR = Nb_est *  fb_SR->getVal();
    
    cout << "Estimated background events: " << Nb_est << endl;
    cout << "Estimated background events in Signal Region (SR): " << Nb_SR << endl;
    
    
    // PLOT SIDEBANDS RESULTS
    RooPlot* M_fr_1 = M->frame(Bins(binfit),Title("Mass sidebands fit (blind: look at SB region only)"));
    data_SB->plotOn(M_fr_1); //Could also plot data.
    M_bkg_pdf->plotOn(M_fr_1); //,Range("R1,R2"),NormRange("R1,R2") //No need to specify range and norm range, since it is defined by fit range.
    Double_t chi2_bkg = M_fr_1->chiSquare(fit_M_SB->floatParsFinal().getSize()+1); //nparams + norm
    suppresswarnings(); //VisualizeErrors can be very noisy.
    M_bkg_pdf->plotOn(M_fr_1,Range("R1,R2"),NormRange("R1,R2"),VisualizeError(*fit_M_SB,1,kFALSE), DrawOption("L"),LineWidth(2),LineColor(kRed),LineStyle(kDashed)); //Visualize uncertainty at Z=1.
    resetwarnings();
    M_bkg_pdf->paramOn(M_fr_1,Parameters(RooArgSet(*M_c1)),Layout(0.57,0.89,0.8) );

    TPaveText *ptext = new TPaveText(0.4,0.13,0.6,0.35,"NDC");
    ptext->AddText(Form("#chi^{2}/ndf = %f", chi2_bkg));
    ptext->AddText(Form("Sidebands events (SB) = %d", int(data_SB->numEntries()) ));
    ptext->AddText(Form("Total bkg estimated from integration (Nb^{*} = SB/fb_{SB}) = %d", int(Nb_est) ));
    ptext->SetBorderSize(0); ptext->SetFillColor(0); ptext->SetTextSize(0.053); ptext->SetTextFont(42);
    M_fr_1->addObject(ptext);
    
    c1->cd(3); M_fr_1->GetYaxis()->SetTitleOffset(1.5);  M_fr_1->Draw();
    if(dopause) { gPad->WaitPrimitive(); gPad->WaitPrimitive();} //Double-Click in pad to show


    // BKG INTERPOLATION, FIRST LOOK AT THE SIGNAL REGION

    // Coarse estimation of peak significance: assumes Signal events = Total events - Estimated Bkg.
    Double_t SoverSpB = (data->numEntries() - Nb_est) / TMath::Sqrt(data->numEntries() - Nb_est + Nb_SR);

    //Unblind and plot Signal Region
    RooPlot* M_fr_2 = M->frame(Bins(binfit),Title("Signal region unblinding"));
    data->plotOn(M_fr_2);
    M_bkg_pdf->plotOn(M_fr_2,Range("R1,R2"),Normalization(data_SB->numEntries(),RooAbsReal::NumEvent));
    M_bkg_pdf->plotOn(M_fr_2,LineStyle(kDotted),LineColor(kGreen),Range("full"),NormRange("R1,R2")); //Need to specify plot and norm range, since normalization range is defined by the fit Range.
    M_bkg_pdf->plotOn(M_fr_2,LineStyle(kDashed),LineColor(kMagenta),Range("SR"),NormRange("R1,R2"));

    TPaveText *ptext_2 = new TPaveText(0.35,0.62,0.7,0.85,"NDC");
    ptext_2->AddText(Form("Bkg expected in SR (Nb_{SR}^{*} = Nb^{*} fb_{SR}) = %d", int(Nb_SR) ));
    ptext_2->AddText(Form("Total events observed in SR (N - SB) = %d ", data->numEntries() - data_SB->numEntries() ));
    ptext_2->AddText(Form("S/#sqrt{S+B} = (N - Nb^{*})/ #sqrt{N - Nb^{*} + Nb_{SR}^{*}} in SR = %0.1f", SoverSpB ));
    ptext_2->SetBorderSize(0); ptext_2->SetFillColor(0); ptext_2->SetTextSize(0.053); ptext_2->SetTextFont(42);
    M_fr_2->addObject(ptext_2);
    
    c1->cd(4); M_fr_2->GetYaxis()->SetTitleOffset(1.5);  M_fr_2->Draw();
    if(dopause) gPad->WaitPrimitive(); //Click in pad to show

    c1->Print("unblind.png");

    
    //LETS SAVE IN WORKSPACE
    w->import(*M_pdf) ;
    w->import(*P_pdf) ;
    w->import(*model) ;
    w->import(*data) ;
    w->import(*data_SB) ;
    //w->import(*fit_M_SB);
    w->import(*SBlow) ;
    w->import(*SBhigh) ;
    w->Print() ;
    w->writeToFile("workspace.root") ;

    return;
}



void suppresswarnings(){
    RooMsgService::instance().setStreamStatus(0,kFALSE);
    RooMsgService::instance().setStreamStatus(1,kFALSE);
    //RooMsgService::instance().setStreamStatus(2,kFALSE);
}

void resetwarnings(){
    RooMsgService::instance().setStreamStatus(0,kTRUE);
    RooMsgService::instance().setStreamStatus(1,kTRUE);
    //RooMsgService::instance().setStreamStatus(2,kTRUE);
}
