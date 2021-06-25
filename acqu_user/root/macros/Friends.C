{

	TCanvas *c1 = new TCanvas( "c1", "Truth Variables", 20, 20, 700, 700);
	c1->Divide( 1, 2);

	TFile f1("a2GoAT/TA2Pi0ComptonMC.root","READ");

//	f1.ls()
//	Pi0ComptonTree->Scan()
//	Pi0ComptonTree->Draw("Pi0Theta")

	Pi0ComptonTree->AddFriend("EvGenTree","evgen/pi0_p_chan85_in.root");

	c1->cd( 1);
	Pi0ComptonTree->Draw("Pi0Theta:EvGenTree.ScatTheta", "", "colz");

	c1->cd( 2);

	Pi0ComptonTree->Draw("Pi0Theta-EvGenTree.ScatTheta");

}
