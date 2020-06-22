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
    //static const double etacut = 2.85;  //  abs(eta) >2.85

    int nthreads = 4;
    ROOT::EnableImplicitMT(nthreads);
    //TString infile = "run_04/no_Scale_newCard_delphes_events.root";
    //TString infile = "run_05/no_Scale_delphes_events.root";
    TString infile = "run_06/Scale_delphes_events.root";
    auto file = new TFile(infile);
    TTreeReader myReader("Delphes", file);

    //auto outfile = new TFile("outFiles/05_noScale_eta>2.85.root", "RECREATE");
    //auto outfile = new TFile("outFiles/05_noScale_eta<1.3.root", "RECREATE");
    auto outfile = new TFile("outFiles/06_Scale_eta<1.3.root", "RECREATE");
    //auto outfile = new TFile("outFiles/outfile_02_noScale_absEta_bigger_2.85.root", "RECREATE");


    //GenJet Definitions
    TTreeReaderValue<int> rv_GenJet_size(myReader, "GenJet_size");
    TTreeReaderArray<float> ra_GenJet_pT(myReader, "GenJet.PT");
    TTreeReaderArray<float> ra_GenJet_eta(myReader, "GenJet.Eta");

    //Jet Definitions
    TTreeReaderValue<int> rv_Jet_size(myReader, "Jet_size");
    TTreeReaderArray<float> ra_Jet_pT(myReader, "Jet.PT");
    TTreeReaderArray<float> ra_Jet_eta(myReader, "Jet.Eta");


    bool cut_GenJet_eta;
    int n_events;
    n_events = myReader.GetEntries(1);


    std::vector<double> x_bin{10., 20., 30., 40., 50., 60., 70., 80., 90., 100.,
                             110.,120.,130.,140.,150.,160.,170.,180.,190.,200.,
                             210.,220.,230.,240.,250.,260.,270.,280.,290.,300.,
                             310.,320.,330.,340.,350.,360.,370.,380.,390.,400.,
                             410.,420.,430.,440.,450.,460.,470.,480.,490.,500.,
                             510.,520.,530.,540.,550.,560.,570.,580.,590.,600.,
                             610.,620.,630.,640.,650.,660.,670.,680.,690.,700.,
                             710.,720.,730.,740.,750.,760.,770.,780.,790.,800.,
                             810.,820.,830.,840.,850.,860.,870.,880.,890.,900.,
                             910.,920.,930.,940.,950.,960.,970.,980.,990.,1000.};

    const Int_t n = x_bin.size()-1;                         

    TH2F* h_response = new TH2F("Response", //const char *  name
                                "Response", //const char *   title
                                         n, //Int_t           nbinsx
                                 &x_bin[0], //const Double_t *  xbins
                                        20, //Int_t         nbinsy
                                         0, //Double_t     ylow
                                         2  //Double_t    yup
                                );

    double ratio;
    // Loop over Events
    for (int i_event = 0; i_event < n_events; ++i_event)
    {
        myReader.Next();



        for (int i = 0; i < min(*rv_Jet_size, *rv_GenJet_size); ++i)
        {
            
            cut_GenJet_eta = abs(ra_GenJet_eta.At(i)) > etacut;
            //if (!cut_GenJet_eta)
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

    Double_t ex[n];
    Double_t ey[n];


    for(int i = 0; i < n; ++i)
    {
        auto projection = h_response->ProjectionY("20_30", i+1, i+1);
        v_y.push_back(projection->GetMean());
        ey[i] = (projection->GetRMS());        
    }

    for (int i = 0; i < n; ++i)
    {
        ex[i]=(x_bin[i+1]-x_bin[i])/2;
    }
    
    
    
    TGraphErrors * graphErr = new TGraphErrors(n, &v_x[0], &v_y[0], ex, ey);
    
    graphErr->SetMarkerStyle(4);
    graphErr->SetLineWidth(1);
    graphErr->SetMinimum(0);
    graphErr->SetMaximum(2);
    
    outfile->cd();
    graphErr->Write();
    h_response->Write();
    outfile->Close();
    return 0;
}


int main()
{
    return analysis_pp2jjj();
}
