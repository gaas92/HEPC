using namespace RooStats;
using namespace RooFit;

void cls( const char* infile =  "CountingModel.root", 
          const char* workspaceName = "w",
          const char* modelConfigName = "ModelConfig",
          const char* dataName = "data" )
{
  // open input file 
  TFile *file = TFile::Open(infile);

  // get the workspace out of the file
  RooWorkspace* w = (RooWorkspace*) file->Get(workspaceName);


  // get the modelConfig out of the file
  RooStats::ModelConfig* mc = (RooStats::ModelConfig*) w->obj(modelConfigName);

  // get the modelConfig out of the file
  RooAbsData* data = w->data(dataName);

  ModelConfig*  sbModel = (RooStats::ModelConfig*) w->obj(modelConfigName);
  RooRealVar* poi = (RooRealVar*) sbModel->GetParametersOfInterest()->first();
  ModelConfig * bModel = (ModelConfig*) sbModel->Clone();
  bModel->SetName(TString(sbModel->GetName())+TString("_with_poi_0"));      
  poi->setVal(0);
  bModel->SetSnapshot( *poi  );

  // asymptotic calculator
  AsymptoticCalculator  ac(*data, *bModel, *sbModel);
  ac.SetOneSided(true);  // for one-side tests (limits)

  // create hypotest inverter 
  HypoTestInverter calc(ac);    // for asymptotic 
  calc.SetConfidenceLevel(0.95);
  calc.UseCLs(true);//use CLs

  int npoints = 50;  // number of points to scan
  double poimin = poi->getMin();
  double poimax = poi->getMax();

  calc.SetFixedScan(npoints,poimin,poimax);   
  HypoTestInverterResult * r = calc.GetInterval();  
  double upperLimit = r->UpperLimit();

  std::cout << "The computed upper limit is: " << upperLimit << std::endl;     
  std::cout << "Expected upper limits, using the B (alternate) model : " << std::endl;
  std::cout << "Expected limit (median) " << r->GetExpectedUpperLimit(0) << std::endl;   
  std::cout << "Expected limit (-1 sig) " << r->GetExpectedUpperLimit(-1) << std::endl;   
  std::cout << "Expected limit (+1 sig) " << r->GetExpectedUpperLimit(1) << std::endl;      
  // plot now the result of the scan    
  HypoTestInverterPlot *plot = new HypoTestInverterPlot("HTI_Result_Plot","HypoTest Scan Result",r);   
  TCanvas * c1 = new TCanvas("HypoTestInverter Scan");
  c1->cd();
  c1->Draw();    
  plot->Draw("CLb 2CL");  // plot also CLb and CLs+b 


}
