void FinishMacro( TString filename = "out/ARHistograms.root")
{
	TString name;

	std::cout << "End-of-Run macro executing" << std::endl;

	TFile outfile( filename, "recreate");
	gROOT->GetList()->Write();
	outfile.Close();

	name = Form( "All histograms saved to %s", filename.Data());
	std::cout << name << std::endl;
}
