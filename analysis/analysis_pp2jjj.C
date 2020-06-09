
int analysis_pp2jjj()
{
    static const double etacut = 5.2;  // -a<.<a

    int nthreads = 4;
    ROOT::EnableImplicitMT(nthreads);
    TString infile = "../data/tag_1_delphes_events.root";
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

    const Int_t n_bin_x = 8;
    double x_bin[n_bin_x+1] = {10., 30., 40., 50., 60., 70., 80., 90., 100};

    TH2F* h_response = new TH2F("Response", "Response", n_bin_x, x_bin, 20, 0, 2);

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

    double v_x[8] = {20., 35., 45., 55., 65., 75., 85., 95.};
    std::vector<double> v_y;    
    for(int i = 0; i < 8; ++i){
        auto projection = h_response->ProjectionY("10_30", i+1, i+1);
        v_y.push_back(projection->GetMean());
    }

    TGraph* graph = new TGraph(8, v_x, &v_y[0]);
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
