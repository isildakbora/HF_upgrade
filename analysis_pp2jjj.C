
int analysis_pp2jjj() 
{
    int a[]={50,60}; // pT range of genjets
    const Int_t n = (a[1]-a[0]);
    static const double etacut = 2;  // -a<.<a
    
    int nthreads = 4;
    ROOT::EnableImplicitMT(nthreads);
    TString infile = "../Data/tag_1_delphes_events.root";
    auto file = new TFile(infile);
    TTreeReader myReader("Delphes", file);

    auto outfile = new TFile("outfile_pp2jjj.root", "RECREATE");


    std::vector<float> v_genjet_pt;
    std::vector<float> v_jet_pt;
    std::vector<float> v_genjet_pt_group(n);
    std::vector<float> v_jet_pt_group(n);
    std::vector<float> v_jet_pt_response(n);

    // Create the TTree and branches
    TTree *outtree = new TTree("flatree", "flattree");
    

    outtree->Branch("br_pt_genjet", &v_genjet_pt);
    outtree->Branch("br_pt_jet", &v_jet_pt);
    //outtree->Branch("br_pt_ratio", &v_jet_ratio);

    //GenJet Definitions
    TTreeReaderValue<int> rv_GenJet_size(myReader, "GenJet_size");
    TTreeReaderArray<float> ra_GenJet_pT(myReader, "GenJet.PT");
    TTreeReaderArray<float> ra_GenJet_eta(myReader, "GenJet.Eta");
    
    //Jet Definitions
    TTreeReaderValue<int> rv_Jet_size(myReader, "Jet_size");
    TTreeReaderArray<float> ra_Jet_pT(myReader, "Jet.PT");
    TTreeReaderArray<float> ra_Jet_eta(myReader, "Jet.Eta");
    

    bool cut_GenJet_eta;
    bool cut_Jet_eta;
    int n_events;
    n_events = myReader.GetEntries(1);

    

    // Loop over Events
    for (int i_event = 0; i_event < n_events; ++i_event)
    {
        myReader.Next();


        for (int i = 0; i < *rv_GenJet_size ; ++i)
        {
            
            cut_GenJet_eta =abs(ra_GenJet_eta.At(i)) > etacut;
            if(cut_GenJet_eta) continue;

            
            if(a[0] <= ra_GenJet_pT.At(i) && ra_GenJet_pT.At(i) < a[1])
            {
            v_genjet_pt.push_back(ra_GenJet_pT.At(i));
                    
            v_genjet_pt_group[(int)ra_GenJet_pT.At(i) % a[0]] += 1;
            }


        }


        for (int k = 0; k < *rv_Jet_size ; ++k)
        {
            
            cut_Jet_eta =abs(ra_Jet_eta.At(k)) > etacut;
            if(cut_Jet_eta) continue;

            
            if(a[0] <= ra_Jet_pT.At(k) && ra_Jet_pT.At(k) < a[1])
            {
            v_jet_pt.push_back(ra_Jet_pT.At(k));

            v_jet_pt_group[(int)ra_Jet_pT.At(k) % a[0]] += 1;
            }
        }

        for (int i = 0; i < v_jet_pt_response.size(); ++i)
        {
         
        if(v_genjet_pt_group[i]==0 || v_jet_pt_group[i]==0 )
        {
            
            //v_jet_pt_response.resize(v_jet_pt_response.size()-1);  // 0-lar-dan kurtulalım
            continue;
        } 
        
        v_jet_pt_response[i] = v_jet_pt_group[i] / v_genjet_pt_group[i];  // the ratio
        
        }

    }

    outtree->Fill();

    // create data x,y -axises
    Double_t x[n], y[n];
    
    for (Int_t i = 0; i < n; ++i)
    {
        x[i] = i + a[0] + 0.4995;
        y[i] = v_jet_pt_response[i];
    }
    Double_t ymax = *max_element(y, y+n);
    TCanvas *c1 = new TCanvas("c1");
    // draw a frame to define the range
    TH1F *hr = c1->DrawFrame(a[0]-5,0,a[1]+5,ymax+1);
    hr->SetXTitle("P_{T} [GeV]");
   hr->SetYTitle("Response");
    // Graph
    TGraph *gr = new TGraph (n,x,y);
    
    //c1->cd();
   

    gr->Draw("C*");
    c1->Update();
    c1->Print("graph3.eps");

    std::cout << "n_events: " << n_events << std::endl;
    std::cout << "v_genjet_pt at 1 " << v_genjet_pt[1] << std::endl;
    std::cout << "v_genjet_pt size: " << v_genjet_pt.size() << std::endl;
    std::cout << "v_jet_pt size: " << v_jet_pt.size() << std::endl;

    std::cout << "v_genjet_pt_group size: " << v_genjet_pt_group.size() << std::endl;
    
    
    std::cout << "v_genje.t_pt_group elements: " << std::endl;
    for (int i :v_genjet_pt_group) std::cout <<  i << std::endl;
    

    std::cout << "v_jet_pt_group size: " << v_jet_pt_group.size() << std::endl;
    
    std::cout << "v_jet_pt_group elements: " << std::endl;
    for (int i :v_jet_pt_group) std::cout <<  i << std::endl;

    std::cout << "v_jet_pt_response size: " << v_jet_pt_response.size() << std::endl;
    for (double i :v_jet_pt_response) std::cout <<  i << std::endl;

    outfile->cd();
    outtree->Write();
    outfile->Close();
    
    return 0;
}


int main()
{
	return analysis_pp2jjj();
}
