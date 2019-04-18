TCanvas* NSkinCanvas;

class NSkinUpdater : public TNamed
{
private:
    Double_t fTotalIntegral;
    Double_t fTotalTime;
    TDirectory* fDir;
    TStopwatch* fWatch;
    TF1* fFunc;
    TF1* fFuncSig;

public:
    NSkinUpdater(const Char_t* name, TDirectory* dir)
        : TName(name, name)
    {
        fTotalIntegral = 0;
        fTotalTime = 0;
        fDir = dir;
        TH1* h = (TH1*)fDir->Get("NSkin_IM");
        if (h) h->Reset();
        fWatch = new TStopwatch();
        fFunc = 0;
        fFuncSig = 0;
    }

    void UpdateCanvas()
    {
        TH1* h = (TH1*)fDir->Get("NSkin_IM");

        if (h)
        {
            if (!fFunc)
                fFunc = new TF1("func", "gaus(0)+pol2(3)", 90, 190);
            fFunc->SetParameters(100, 135, 10, 100, 0.1, 0.01, 0.01);
            fFunc->SetParLimits(0, 0, 1e6);
            fFunc->SetParLimits(1, 130, 140);
            fFunc->SetParLimits(2, 6, 12);
            h->Fit(func, "RB0Q");
           
            if (!fFuncSig)
            {
                fFuncSig = new TF1("sig", "gaus", 90, 190);
                fFuncSig->SetLineColor(kBlue);
            }
            fFuncSig->SetParameter(0, fFunc->GetParameter(0));
            fFuncSig->SetParameter(1, fFunc->GetParameter(1));
            fFuncSig->SetParameter(2, fFunc->GetParameter(2));
            Double_t integral = fFuncSig->Integral(90, 190);

            Double_t time = fWatch->RealTime();
            fTotalIntegral += integral;
            fTotalTime += time;
                
            Double_t rate = integral * 4 / time * 60;
            Double_t avr_rate = fTotalIntegral * 4 / fTotalTime * 60;

            Int_t min = fTotalTime / 60;
            Int_t sec = (fTotalTime - min*60.);
            printf("\n");
            printf("%.3e pi0/min (total avr: %.3e)  Time: %02d:%02d min\n", rate, avr_rate, min, sec);
            printf("\n");
            
            NSkinCanvas->cd(2);
            fFunc->Draw("same");
            fFuncSig->Draw("same");
            NSkinCanvas->Update();
            NSkinCanvas->Draw();
            NSkinCanvas->SaveAs("nskin.pdf");
            h->Reset();
            fWatch->Start();
        }
    }
};

void extra_check_nskin()
{
    const Int_t n = 6;
    const Char_t* hName[n] = { "NSkin_TaggerTime",
                               "NSkin_IM",
                               "NSkin_IM_ME",
                               "NSkin_IM_Theta_Cut_ME",
                               "NSkin_IM_Eg_Cut_ME",
                               "NSkin_ME_Eg_Cut_IM"
                             };
    
    NSkinCanvas = new TCanvas("NSkin_Canvas", "Neutron skin monitoring");
    NSkinCanvas->DivideSquare(n);
    //NSkinCanvas->SetWindowSize(1200,1000);
    //NSkinCanvas->SetWindowPosition(1920,500);

    TH1* h[n];
    for (Int_t i = 0; i < n; i++)
    {
        h[i] = (TH1*)gDirectory->Get(hName[i]);
        NSkinCanvas->cd(i+1);
        if (h[i])
        {
            if (h[i]->InheritsFrom("TH2"))
                h[i]->Draw("colz");
            else
                h[i]->Draw();
        }
    }

    NSkinUpdater* updater = new NSkinUpdater("NSkinUpdater", gDirectory);
    TTimer* timer = new TTimer(120000);
    timer->Connect("Timeout()", "NSkinUpdater", updater, "UpdateCanvas()");  
    timer->Start();
}

