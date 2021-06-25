{

	TFile f1("a2GoAT/TA2Pi0ComptonMC.root","READ");

//	f1.ls()
//	Pi0ComptonTree->Scan()
//	Pi0ComptonTree->Draw("Pi0Theta")

	Pi0ComptonTree->AddFriend("EvGenTree","evgen/pi0_p_chan85_in.root");

	Pi0ComptonTree->Draw("Pi0Theta:EvGenTree.ScatTheta", "", "colz");

}
