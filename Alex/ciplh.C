//Profile Likelihood calculator
//confidence interval
//
//
using namespace RooStats;
void ciplh(int nsig = 100,  //number of sig events
	  int nbkg = 1000 //number of bkg events 
	  ){
  RooWorkspace w("w");
  w.factory("Exponential:bkg_pdf(x[0,10],a[-0.5,-2,-0.2])");//bkg
  w.factory("Gaussian:sig_pdf(x,mass[2],sigma[0.3])");//signal

  w.factory("SUM:model(nsig[30,200]*sig_pdf, nbkg[0,2000]*bkg_pdf)");//s+b

  RooAbsPdf * pdf = w.pdf("model");
  RooRealVar * x = w.var("x"); //observable

  w.var("nsig")->setVal(nsig);
  w.var("nbkg")->setVal(nbkg);

  RooRandom::randomGenerator()->SetSeed(10); 

  x->setBins(50);

  RooDataSet * data = pdf-> generate(*x);//generate toy data
  data ->SetName("data");
  w.import(*data);

  TCanvas *c0 = new TCanvas("c0","c0",620,620);
  c0->cd();
  c0->Draw();
  RooPlot * plotfit = x->frame();
  data->plotOn(plotfit);


  RooFitResult * r = pdf->fitTo(*data, RooFit::Save(true), RooFit::Minimizer("Minuit2","Migrad"));
  r->Print();

  pdf->plotOn(plotfit);
  pdf->plotOn(plotfit, RooFit::Components("bkg_pdf"), RooFit::LineStyle(kDashed) );
  pdf->plotOn(plotfit, RooFit::Components("sig_pdf"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed) );
  pdf->paramOn(plotfit);
  plotfit->Draw();

  /////////// MoldeConfig
  RooStats::ModelConfig mc("ModelConfig",&w);
  mc.SetPdf(*pdf);
  mc.SetParametersOfInterest(*w.var("nsig"));
  mc.SetObservables(*w.var("x"));
  w.defineSet("nuisParams","a,nbkg"); 
  mc.SetNuisanceParameters(*w.set("nuisParams"));
  w.import(mc);

  w.writeToFile("GaussianExpModel.root",true);

  /////////// PL
  Double_t cl = 0.683; // 68% cl interval
  ProfileLikelihoodCalculator pl(*data,mc);
  pl.SetConfidenceLevel(cl);
  LikelihoodInterval * interval = pl.GetInterval();
  
  RooRealVar * poi = (RooRealVar*) mc.GetParametersOfInterest()->first();
  Double_t lowerlimit = interval->LowerLimit(*poi);
  Double_t upperlimit = interval->UpperLimit(*poi);

  TCanvas *c1 = new TCanvas("c1","c1",620,620);
  c1->cd();
  c1->Draw();
  LikelihoodIntervalPlot * plot = new LikelihoodIntervalPlot(interval);
  plot->SetRange(50,150);
  plot->Draw("");
  cout<<endl<<"68% cl interval on "<<poi->GetName()<<" is: ["<<lowerlimit << ","<<upperlimit<<"] "<<endl;
}
