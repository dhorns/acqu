void AHeT()
{
	TString name;

	name = "Acqu_pi0_he4_200_out.root";
	TFile *f1 = new TFile( name, "READ");
//	f1->ls();
	TTree *T = (TTree*)f1->Get( "detectorHits");
//	T->Print();

	TCanvas *c = new TCanvas( "canvas", "Showing Results", 500, 0, 700, 700);

//	c->Divide( 1, 2);
//	c->cd( 1);
//	T->Draw( "nActiveHits");
//	c->cd( 2);
//	T->Draw( "ActiveHits");

//	T->Scan( "nActiveHits");
//	T->Scan( "ActiveHits:ActiveEnergy:ActiveTime");
//	T->Scan( "ActiveHits:ActiveEnergy:ActiveTime", "ActiveEnergy > 5");
//	T->Draw( "ActiveEnergy>>h1(100, 0, 10)", "ActiveEnergy > 5");

//	T->Draw( "ActiveHits:ActiveEnergy:ActiveTime");
//	T->Draw( "ActiveHits:ActiveEnergy:ActiveTime", "ActiveEnergy > 0.5");

	
//	TH2D *h1 = new TH2D( "h1", "Energy vs Channel", 10, 0, 10, 100, 0, 10);
//	T->Project( "h1", "ActiveEnergy:ActiveHits", "ActiveEnergy > 2");
//	h1->Draw();
	
	T->AddFriend( "tracks");
//	T->Scan( "ActiveEnergy:tracks.clusterEnergy");
	T->Draw( "ActiveEnergy:tracks.clusterEnergy");

}
