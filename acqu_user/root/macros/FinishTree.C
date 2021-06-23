void FinishTree( TString filename = "out/ARHistograms.root")
{
	TString name;

	std::cout << "End-of-Run macro executing" << std::endl;

	std::cout << "Closing tree files..." << std::endl;
 	TA2Pi0Compton* comp = (TA2Pi0Compton*)(gAN->GetPhysics());
 	comp->CloseTrees();

	std::cout << "Saving defined histograms..." << std::endl;
	TFile outfile( filename, "recreate");
	gROOT->GetList()->Write();
	outfile.Close();

	name = Form( "All histograms saved to %s", filename.Data());
	std::cout << name << std::endl;
}
