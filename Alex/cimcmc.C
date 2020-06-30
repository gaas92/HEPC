//MCMC calculator
//confidence interval
//
//
using namespace RooStats;
void cimcmc(const char* inputfile = "GaussianExpModel.root",
	    const char* wsname = "w",
	    const char* modelconfigname = "ModelConfig",
	    const char* dataName = "data"
	    ){

  TFile *file = TFile::Open(inputfile);
  RooWorkspace* w = (RooWorkspace*) file->Get(wsname);
  RooStats::ModelConfig* mc = (RooStats::ModelConfig*) w->obj(modelconfigname);
  RooAbsData* data = w->data(dataName);

  MCMCCalculator mcmc(*data,*mc);
  mcmc.SetConfidenceLevel(0.683); //68% cl interval
  SequentialProposal sp(0.1);
  mcmc.SetProposalFunction(sp);
  mcmc.SetNumIters(100000);// Metropolis-Hastings algorithm iterations
  mcmc.SetNumBurnInSteps(1000);// first N steps to be ignored as burn-in
  mcmc.SetLeftSideTailFraction(0.5); //central Bayesian interval

  MCMCInterval* interval = mcmc.GetInterval();
  RooRealVar* poi = (RooRealVar*) mc->GetParametersOfInterest()->first();

  TCanvas *c1 = new TCanvas("c1","c1",620,620);
  c1->cd();
  c1->Draw();
  MCMCIntervalPlot plot(*interval);
  plot.Draw();

  Double_t lowerlimit = interval->LowerLimit(*poi);
  Double_t upperlimit = interval->UpperLimit(*poi);
  cout<<endl<<"68% cl interval on "<<poi->GetName()<<" is: ["<<lowerlimit<< ","<<upperlimit<<"] "<<endl;

}
