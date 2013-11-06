#include "TA2GoAT.h"

ClassImp(TA2GoAT)

TA2GoAT::TA2GoAT(const char* Name, TA2Analysis* Analysis) : TA2AccessSQL(Name, Analysis),
                                                                    file(0),
                                                                    treeRawEvent(0),
								    treeTagger(0),
								    treeTrigger(0),
                                                                    treeDetectorHits(0),
                                                                    treeScaler(0),
                                                                    nParticles(0),
                                                                    Px(0),
                                                                    Py(0),
                                                                    Pz(0),
                                                                    E(0),
                                                                    time(0),
                                                                    clusterSize(0),
                                                                    Apparatus(0),
                                                                    d_E(0),
                                                                    WC0_E(0),
                                                                    WC1_E(0),
                                                                    WC_Vertex_X(0),
                                                                    WC_Vertex_Y(0),
                                                                    WC_Vertex_Z(0),
                                                                    nTagged(0),
                                                                    tagged_ch(0),
                                                                    tagged_t(0),
                                                                    nNaI_Hits(0),
                                                                    NaI_Hits(0),
                                                                    nPID_Hits(0),
                                                                    PID_Hits(0),
                                                                    nWC_Hits(0),
                                                                    WC_Hits(0),                                                                                                    nBaF2_PbWO4_Hits(0),
                                                                    BaF2_PbWO4_Hits(0),
                                                                    nVeto_Hits(0),
                                                                    Veto_Hits(0),
                                                                    ESum(0),
                                                                    CBMult(0),
                                                                    TAPSMult(0),
                                                                    eventNumber(0),
                                                                    eventID(0)
{
    	strcpy(outputFolder,"~");
    	strcpy(fileName,"RootTree");

    	AddCmdList(RootTreeConfigKeys);
}


TA2GoAT::~TA2GoAT()
{
	if(treeRawEvent)
		delete treeRawEvent;
	if(treeTagger)
		delete treeTagger;
	if(treeDetectorHits)
		delete treeDetectorHits;
	if(treeScaler)
		delete treeScaler;
    	if(file)
		delete file;
}


void    TA2GoAT::LoadVariable()
{
	// Including histogram output for testing purposes (quick check of variables)
   	TA2AccessSQL::LoadVariable();

    	TA2DataManager::LoadVariable("nParticles", 	&nParticles,	EISingleX);
    	TA2DataManager::LoadVariable("Px", 			Px,		EDMultiX);
    	TA2DataManager::LoadVariable("Py", 			Py,		EDMultiX);
    	TA2DataManager::LoadVariable("Pz", 			Pz,		EDMultiX);
    	TA2DataManager::LoadVariable("E", 			E,		EDMultiX);
    	TA2DataManager::LoadVariable("time", 		time,		EDMultiX);

    	TA2DataManager::LoadVariable("nTagged", 	&nTagged,	EISingleX);
    	TA2DataManager::LoadVariable("taggedCh", 	tagged_ch,	EIMultiX);
    	TA2DataManager::LoadVariable("taggedT", 	tagged_t,	EDMultiX);    

    	TA2DataManager::LoadVariable("dE", 		d_E,		EDMultiX);
    	TA2DataManager::LoadVariable("WC0E", 		WC0_E,		EDMultiX);   
    	TA2DataManager::LoadVariable("WC1E", 		WC1_E,		EDMultiX);
        
    	return;
    
}


void    TA2GoAT::SetConfig(Char_t* line, Int_t key)
{
    	switch(key)
    	{
    	case EG_OUTPUT_FOLDER:
     	   strcpy(outputFolder,line);
     	   while(outputFolder[strlen(outputFolder)-1]=='\n' || outputFolder[strlen(outputFolder)-1]=='\r')
			outputFolder[strlen(outputFolder)-1]='\0';
        return;
    	case EG_FILE_NAME:
        	strcpy(fileName,line);
        	while(fileName[strlen(fileName)-1]=='\n' || fileName[strlen(fileName)-1]=='\r')
			fileName[strlen(fileName)-1]='\0';
        return;
    	default:
        	TA2AccessSQL::SetConfig(line, key);
    	}
}

void    TA2GoAT::PostInit()
{
    
    	Px			= new Double_t[TA2GoAT_MAX_PARTICLE];
    	Py			= new Double_t[TA2GoAT_MAX_PARTICLE];
    	Pz			= new Double_t[TA2GoAT_MAX_PARTICLE];
    	E			= new Double_t[TA2GoAT_MAX_PARTICLE];
    	time		= new Double_t[TA2GoAT_MAX_PARTICLE];
    	clusterSize  = new UChar_t[TA2GoAT_MAX_PARTICLE];
    
    	tagged_ch	= new Int_t[TA2GoAT_MAX_TAGGER];
    	tagged_t	= new Double_t[TA2GoAT_MAX_TAGGER];
    
    	Apparatus	= new UChar_t[TA2GoAT_MAX_PARTICLE];
    	d_E			= new Double_t[TA2GoAT_MAX_PARTICLE];
    	WC0_E		= new Double_t[TA2GoAT_MAX_PARTICLE];
    	WC1_E		= new Double_t[TA2GoAT_MAX_PARTICLE];
    
    	WC_Vertex_X = new Double_t[TA2GoAT_MAX_PARTICLE];
    	WC_Vertex_Y = new Double_t[TA2GoAT_MAX_PARTICLE];
    	WC_Vertex_Z = new Double_t[TA2GoAT_MAX_PARTICLE];
    
    	NaI_Hits		= new Int_t[TA2GoAT_MAX_HITS];  
    	PID_Hits		= new Int_t[TA2GoAT_MAX_HITS];
    	WC_Hits			= new Int_t[TA2GoAT_MAX_HITS];
    	BaF2_PbWO4_Hits	= new Int_t[TA2GoAT_MAX_HITS];
    	Veto_Hits		= new Int_t[TA2GoAT_MAX_HITS];
    
    	printf("---------\n");
    	printf("Init Tree\n");
    	printf("---------\n");
    
    	// Append input filename to output tree name.
    	TString fullName;
    	if(gAR->GetProcessType() == EMCProcess) fullName = gAR->GetTreeFileList(0);        
    	else  fullName = gAR->GetFileName();
		
	int length = fullName.Length();
	int last = 0;
	for (int i = 0; i < length; i++)
	{
		int index = fullName.Index("/"); 
		fullName.Remove(0,index+1);	
		if (index == -1) break;
		last += index+1; 
	}
	Char_t inFile[256], str[256];
    	if(gAR->GetProcessType() == EMCProcess) 
		sscanf( gAR->GetTreeFileList(0)+last, "%[^.].root\n", inFile);       
    	else    
		sscanf( gAR->GetFileName()+last, "%[^.].dat\n", inFile);	

    	sprintf(str, "%s/%s_%s.root", outputFolder, fileName, inFile);        
        
   	file		= new TFile(str,"RECREATE");
	treeRawEvent	= new TTree("treeRawEvent", "treeRawEvent");
	treeTagger	= new TTree("treeTagger","treeTagger");
	treeTrigger	= new TTree("treeTrigger","treeTrigger");
	treeDetectorHits = new TTree("treeDetectorHits", "treeDetectorHits");
	treeScaler	= new TTree("treeScaler", "treeScaler");
	
	treeRawEvent->Branch("nParticles",&nParticles,"nParticles/I");
	treeRawEvent->Branch("Px", Px, "Px[nParticles]/D");
	treeRawEvent->Branch("Py", Py, "Py[nParticles]/D");
	treeRawEvent->Branch("Pz", Pz, "Pz[nParticles]/D");
	treeRawEvent->Branch("E",  E,  "E[nParticles]/D");	
	treeRawEvent->Branch("time", time, "time[nParticles]/D");
	treeRawEvent->Branch("clusterSize", clusterSize, "clusterSize[nParticles]/b");
	treeRawEvent->Branch("Apparatus", Apparatus, "Apparatus[nParticles]/b");
	treeRawEvent->Branch("d_E", d_E, "d_E[nParticles]/D");	
	treeRawEvent->Branch("WC0_E", WC0_E, "WC0_E[nParticles]/D");	
	treeRawEvent->Branch("WC1_E", WC1_E, "WC1_E[nParticles]/D");
	treeRawEvent->Branch("WC_Vertex_X", WC_Vertex_X, "WC_Vertex_X[nParticles]/D");	
	treeRawEvent->Branch("WC_Vertex_Y", WC_Vertex_Y, "WC_Vertex_Y[nParticles]/D");	
	treeRawEvent->Branch("WC_Vertex_Z", WC_Vertex_Z, "WC_Vertex_Z[nParticles]/D");
	
	treeTagger->Branch("nTagged", &nTagged,"nTagged/I");
	treeTagger->Branch("tagged_ch", tagged_ch, "tagged_ch[nTagged]/I");
	treeTagger->Branch("tagged_t", tagged_t, "tagged_t[nTagged]/D");

	treeTrigger->Branch("ESum", &ESum, "ESum/D");
	treeTrigger->Branch("CBMult", &CBMult, "CBMult/I");
	treeTrigger->Branch("TAPSMult", &TAPSMult, "TAPSMult/I");

	treeDetectorHits->Branch("nNaI_Hits", &nNaI_Hits, "nNaI_Hits/I");
	treeDetectorHits->Branch("NaI_Hits", NaI_Hits, "NaI_Hits[nNaI_Hits]/I");
	treeDetectorHits->Branch("nPID_Hits", &nPID_Hits, "nPID_Hits/I");
	treeDetectorHits->Branch("PID_Hits", PID_Hits, "PID_Hits[nPID_Hits]/I");
	treeDetectorHits->Branch("nWC_Hits", &nWC_Hits, "nWC_Hits/I");
	treeDetectorHits->Branch("WC_Hits", WC_Hits, "WC_Hits[nWC_Hits]/I");	
	treeDetectorHits->Branch("nBaF2_PbWO4_Hits", &nBaF2_PbWO4_Hits, "nBaF2_PbWO4_Hits/I");
	treeDetectorHits->Branch("BaF2_PbWO4_Hits", BaF2_PbWO4_Hits, "BaF2_PbWO4_Hits[nBaF2_PbWO4_Hits]/I");
	treeDetectorHits->Branch("nVeto_Hits", &nVeto_Hits, "nVeto_Hits/I");
	treeDetectorHits->Branch("Veto_Hits", Veto_Hits, "Veto_Hits[nVeto_Hits]/I");
	
	treeScaler->Branch("eventNumber", &eventNumber, "eventNumber/I");
	treeScaler->Branch("eventID", &eventID, "eventID/I");
	printf("GetMaxScaler: %d\n", GetMaxScaler());
	sprintf(str, "Scaler[%d]/i", GetMaxScaler());
	treeScaler->Branch("Scaler", fScaler, str);
	
	gROOT->cd();
	
	eventNumber	= 0;
	
	// Default SQL-physics initialisation
    	TA2AccessSQL::PostInit();	

}


void    TA2GoAT::Reconstruct()
{
	//Is Scaler Read
	if(gAR->IsScalerRead())
	{
		eventID	= gAN->GetNDAQEvent();
		
		//for(int i=0; i<GetMaxScaler(); i++)
		//	printf("Scaler%d: %d\n", i, fScaler[i]);
		treeScaler->Fill();		
		return;
	}
	
	// Collect Tagger M0 Hits
	nTagged	= fLadder->GetNhits();
	for(int i=0; i<nTagged; i++)
	{
		tagged_ch[i]	= fLadder->GetHits(i);
		tagged_t[i]		= (fLadder->GetTimeOR())[i];
	}
	
	// Collect Tagger M+ Hits
	for(int m=1; m<fLadder->GetNMultihit(); m++)
	{
		for(int i=0; i<fLadder->GetNhitsM(m); i++)
		{
			tagged_ch[nTagged+i]	= (fLadder->GetHitsM(m))[i];
			tagged_t[nTagged+i]		= (fLadder->GetTimeORM(m))[i];
		}
		nTagged	+= fLadder->GetNhitsM(m);
	}
	
	// Collect CB Hits
    	nParticles	= fCB->GetNParticle();      
	for(int i=0; i<nParticles; i++)
	{
		Apparatus[i]	= (UChar_t)EAppCB;			
		Px[i]		= fCB->GetParticles(i).GetPx();
		Py[i]		= fCB->GetParticles(i).GetPy();
		Pz[i]		= fCB->GetParticles(i).GetPz();
		E[i]		= fCB->GetParticles(i).GetE();
		time[i]		= fCB->GetParticles(i).GetTime();	
		clusterSize[i]  = (UChar_t)fCB->GetParticles(i).GetClusterSize();
		d_E[i]		= fCB->GetParticles(i).GetVetoEnergy();
		WC0_E[i]	= fCB->GetParticles(i).GetEnergyMwpc0();
	 	WC1_E[i]	= fCB->GetParticles(i).GetEnergyMwpc1();
	 	WC_Vertex_X[i]  = fCB->GetParticles(i).GetPsVertex().X();
	 	WC_Vertex_Y[i]  = fCB->GetParticles(i).GetPsVertex().Y();
	 	WC_Vertex_Z[i]  = fCB->GetParticles(i).GetPsVertex().Z();
	}
	// Collect TAPS Hits
	for(int i=0; i<fTAPS->GetNParticle(); i++)
	{
		Apparatus[nParticles+i]		= (UChar_t)EAppTAPS;		
		Px[nParticles+i]		= fTAPS->GetParticles(i).GetPx();
		Py[nParticles+i]		= fTAPS->GetParticles(i).GetPy();
		Pz[nParticles+i]		= fTAPS->GetParticles(i).GetPz();
		E[nParticles+i]			= fTAPS->GetParticles(i).GetE();
		time[nParticles+i]		= fTAPS->GetParticles(i).GetTime();
		clusterSize[nParticles+i]  	= (UChar_t)fTAPS->GetParticles(i).GetClusterSize();
		d_E[nParticles+i]		= fTAPS->GetParticles(i).GetVetoEnergy();
		WC0_E[nParticles+i]		= ENullFloat; 
	 	WC1_E[nParticles+i]    		= ENullFloat; 
	 	WC_Vertex_X[nParticles+i]  	= ENullFloat; 
	 	WC_Vertex_Y[nParticles+i]  	= ENullFloat; 
	 	WC_Vertex_Z[nParticles+i]  	= ENullFloat; 
	}
	nParticles += fTAPS->GetNParticle(); // update number of particles

	// Get Detector Hits
	nNaI_Hits = fNaI->GetNhits();
	for(int i=0; i<nNaI_Hits; i++)   { NaI_Hits[i] = fNaI->GetHits(i); }

	nPID_Hits = fPID->GetNhits();
	for(int i=0; i<nPID_Hits; i++)   { PID_Hits[i] = fPID->GetHits(i); }

	nWC_Hits = fMWPC->GetNhits();
	for(int i=0; i<nWC_Hits; i++)    { WC_Hits[i] = fMWPC->GetHits(i); }

	nBaF2_PbWO4_Hits = fBaF2PWO->GetNhits();
	for(int i=0; i<nBaF2_PbWO4_Hits; i++) 
						  { BaF2_PbWO4_Hits[i] = fBaF2PWO->GetHits(i); }

	nVeto_Hits = fVeto->GetNhits();
	for(int i=0; i<nVeto_Hits; i++) { Veto_Hits[i] = fVeto->GetHits(i); }
	

		//Apply EndBuffer
    	Px[nParticles] 		= EBufferEnd;
    	Py[nParticles] 		= EBufferEnd;
    	Pz[nParticles] 		= EBufferEnd;
    	E[nParticles] 		= EBufferEnd;
    	time[nParticles] 	= EBufferEnd;
    	WC0_E[nParticles] 	= EBufferEnd;
    	WC1_E[nParticles] 	= EBufferEnd;
    	WC_Vertex_X[nParticles] = EBufferEnd;  
    	WC_Vertex_Y[nParticles] = EBufferEnd;    
		WC_Vertex_Z[nParticles] = EBufferEnd;    
		d_E[nParticles] 	= EBufferEnd;    
    	tagged_ch[nTagged] 	= EBufferEnd;
    	tagged_t[nTagged] 	= EBufferEnd;	
	
		//Fill Trees
		treeRawEvent->Fill();
		treeTagger->Fill();
		treeTrigger->Fill();
		treeDetectorHits->Fill();

		//increment event number
		eventNumber++;	
}

void    TA2GoAT::Finish()
{
	printf("------------------\n");
	printf("Write Tree to file\n");
	printf("------------------\n");
	
	file->cd();
	
	if(treeRawEvent) 
	{
		treeRawEvent->Write();	// Write	
		delete treeRawEvent; 	// Close and delete in memory
	}
	if(treeTagger) 
	{
		treeTagger->Write();	// Write	
		delete treeTagger; 	// Close and delete in memory
	}	
	if(treeTrigger) 
	{
		treeTrigger->Write();	// Write	
		delete treeTrigger; 	// Close and delete in memory
	}		
	if(treeDetectorHits) 
	{
		treeDetectorHits->Write();// Write	
		delete treeDetectorHits;  // Close and delete in memory
	}		
	if(treeScaler) 
	{
		treeScaler->Write();	// Write	
		delete treeScaler; 	// Close and delete in memory
    }
    if(file) 
		delete file;		// Close and delete in memory

	
	TA2AccessSQL::Finish();
}

void    TA2GoAT::ParseMisc(char* line)
{
	TA2AccessSQL::ParseMisc(line);
}
