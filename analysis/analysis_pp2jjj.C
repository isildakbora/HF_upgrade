std::vector<double> create_data_point_x(std::vector<double> v)
{   
    double x;
    std::vector<double> v_x;
    for (int i = 0; i < v.size()-1; ++i)
    {
        x = 0.5*(v.at(i) + v.at(i+1));
        v_x.push_back(x);
    }
    return v_x;
};

int analysis_pp2jjj()
{
    static const double etacut = 1.3;  // -a<.<a

    int nthreads = 4;
    ROOT::EnableImplicitMT(nthreads);
    TString infile = "tag_1_delphes_events.root";
    auto file = new TFile(infile);
    TTreeReader myReader("Delphes", file);

    auto outfile = new TFile("outfile_pp2jjj.root", "RECREATE");


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


    std::vector<double> x_bin{30., 40., 50., 60., 70., 80., 90., 100.,
                             110.,120.,130.,140.,150.,160.,170.,180.,190.,200.};
    //std::vector<double> x_bin{30., 50., 70., 90., 110., 140., 160., 180., 200.};

    TH2F* h_response = new TH2F("Response", "Response", x_bin.size()-1, &x_bin[0], 20, 0, 2);

    double ratio;
    // Loop over Events
    for (int i_event = 0; i_event < n_events; ++i_event)
    {
        myReader.Next();



        for (int i = 0; i < min(*rv_Jet_size, *rv_GenJet_size); ++i)
        {

            cut_GenJet_eta = abs(ra_GenJet_eta.At(i)) > etacut;
            if (cut_GenJet_eta)
            {
                continue;
            }

            ratio = ra_Jet_pT.At(i)/ra_GenJet_pT.At(i);
            h_response->Fill(ra_GenJet_pT.At(i), ratio);
        }

    }

    auto v_x = create_data_point_x(x_bin);
    std::vector<double> v_y;    
    for(int i = 0; i < x_bin.size()-1; ++i){
        auto projection = h_response->ProjectionY("10_30", i+1, i+1);
        v_y.push_back(projection->GetMean());
    }

    
    TCanvas *c1 = new TCanvas("c1");
    // draw a frame to define the range
    TH1F *hr = c1->DrawFrame(30,0,200,2);
    hr->SetXTitle("P_{T} [GeV]");
    hr->SetYTitle("Response");
    

    TGraph* graph = new TGraph(x_bin.size()-1, &v_x[0], &v_y[0]);
    
    //graph->Fit("pol0");
    
    
    graph->SetMarkerStyle(4);
    graph->Draw("P");

    


    outfile->cd();

    graph->Write();
    h_response->Write();
    outfile->Close();
    return 0;
}


int main()
{
    return analysis_pp2jjj();
}
