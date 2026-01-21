#include <iostream>
#include <vector>
#include <string>

// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"
#include "TString.h"
#include "TSystem.h"

// Custom headers (These work because run.sh cd's into this folder)
#include "UtilGeom.h"
#include "UtilTrack.h"
#include "UtilTrigger.h"

using namespace std;

// The second argument is optional/dummy because we calculate the name inside
void AddRoadsToTree(const char* inputFileName, const char* dummy = "")
{
    // 1. Open input
    TFile* fIn = TFile::Open(inputFileName, "READ");
    if (!fIn || fIn->IsZombie()) {
        cout << "Error: Cannot open input file: " << inputFileName << endl;
        return;
    }

    TTree* tIn = (TTree*)fIn->Get("result_mix");
    if (!tIn) {
        cout << "Error: Cannot find TTree 'result_mix' in " << inputFileName << endl;
        fIn->Close();
        return;
    }

    // 2. Setup Variables
    Int_t runID;

    // Track 1
    Double_t x1_st1, y1_st1, z1_st1 = 676.587;
    Float_t px1_st1, py1_st1, pz1_st1;
    Double_t x1_st3, y1_st3, z1_st3 = 1959.12;
    Float_t px1_st3, py1_st3, pz1_st3;

    // Track 2
    Double_t x2_st1, y2_st1, z2_st1 = 676.587;
    Float_t px2_st1, py2_st1, pz2_st1;
    Double_t x2_st3, y2_st3, z2_st3 = 1959.12;
    Float_t px2_st3, py2_st3, pz2_st3;

    tIn->SetBranchStatus("*", 1);
    tIn->SetBranchAddress("runID",    &runID);

    // Address mapping
    tIn->SetBranchAddress("x1_st1",   &x1_st1); tIn->SetBranchAddress("y1_st1",   &y1_st1);
    tIn->SetBranchAddress("px1_st1",  &px1_st1); tIn->SetBranchAddress("py1_st1",  &py1_st1); tIn->SetBranchAddress("pz1_st1",  &pz1_st1);
    tIn->SetBranchAddress("x1_st3",   &x1_st3); tIn->SetBranchAddress("y1_st3",   &y1_st3);
    tIn->SetBranchAddress("px1_st3",  &px1_st3); tIn->SetBranchAddress("py1_st3",  &py1_st3); tIn->SetBranchAddress("pz1_st3",  &pz1_st3);

    tIn->SetBranchAddress("x2_st1",   &x2_st1); tIn->SetBranchAddress("y2_st1",   &y2_st1);
    tIn->SetBranchAddress("px2_st1",  &px2_st1); tIn->SetBranchAddress("py2_st1",  &py2_st1); tIn->SetBranchAddress("pz2_st1",  &pz2_st1);
    tIn->SetBranchAddress("x2_st3",   &x2_st3); tIn->SetBranchAddress("y2_st3",   &y2_st3);
    tIn->SetBranchAddress("px2_st3",  &px2_st3); tIn->SetBranchAddress("py2_st3",  &py2_st3); tIn->SetBranchAddress("pz2_st3",  &pz2_st3);

    // 3. Get RunID to generate filename
    if (tIn->GetEntries() > 0) {
        tIn->GetEntry(0);
    } else {
        cout << "Error: Empty tree." << endl;
        fIn->Close();
        return;
    }

    // Generate output filename: LH2_<runID>_mix.root
    // We save it in the directory where the script is running (or you can specify a path)
    //TString finalOutputName = Form("LH2_%d_mix.root", runID);
    //TString finalOutputName = Form("LD2_%d_mix.root", runID);
    TString finalOutputName = Form("Flask_%d_mix.root", runID);

    // If you want to save it in the same folder as input, use this instead:
    // TString sInput(inputFileName);
    // TString sDir = sInput(0, sInput.Last('/') + 1);
    // TString finalOutputName = sDir + Form("LH2_%d_mix.root", runID);

    cout << "Processing Run: " << runID << " -> " << finalOutputName << endl;

    TFile* fOut = new TFile(finalOutputName, "RECREATE");
    TTree* tOut = tIn->CloneTree(0);

    Int_t posRoad = 0;
    Int_t negRoad = 0;
    tOut->Branch("posRoad", &posRoad, "posRoad/I");
    tOut->Branch("negRoad", &negRoad, "negRoad/I");

    // 4. Loop
    Long64_t nEntries = tIn->GetEntries();
    int current_run_id_loaded = -1;

    // Ensure "opts" folder is visible. If "opts" is in parent dir, use "../opts"
    //UtilGeom::Init(runID, "opts");
    //current_run_id_loaded = runID;

    for (Long64_t i = 0; i < nEntries; i++) {
        tIn->GetEntry(i);

        if (runID != current_run_id_loaded) {
            UtilGeom::Init(runID, "../opts");
            current_run_id_loaded = runID;
        }

        // Pos Road
        TVector3 pos1_st1(x1_st1, y1_st1, z1_st1);
        TVector3 mom1_st1(px1_st1, py1_st1, pz1_st1);
        TVector3 pos1_st3(x1_st3, y1_st3, z1_st3);
        TVector3 mom1_st3(px1_st3, py1_st3, pz1_st3);
        vector<int> roads1 = UtilTrack::FindMatchedRoads(pos1_st1, mom1_st1, pos1_st3, mom1_st3);
        posRoad = (roads1.size() > 0) ? roads1[0] : 0;

        // Neg Road
        TVector3 pos2_st1(x2_st1, y2_st1, z2_st1);
        TVector3 mom2_st1(px2_st1, py2_st1, pz2_st1);
        TVector3 pos2_st3(x2_st3, y2_st3, z2_st3);
        TVector3 mom2_st3(px2_st3, py2_st3, pz2_st3);
        vector<int> roads2 = UtilTrack::FindMatchedRoads(pos2_st1, mom2_st1, pos2_st3, mom2_st3);
        negRoad = (roads2.size() > 0) ? roads2[0] : 0;

        tOut->Fill();
    }

    tOut->Write();
    fOut->Close();
    fIn->Close();
}
