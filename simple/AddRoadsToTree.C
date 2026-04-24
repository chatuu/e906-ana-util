#include <iostream>
#include <vector>
#include <string>

// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"
#include "TString.h"
#include "TSystem.h"
#include "TKey.h"
#include "TList.h"

// Custom headers
#include "UtilGeom.h"
#include "UtilTrack.h"
#include "UtilTrigger.h"

using namespace std;

void ProcessSingleTree(TFile* fIn, TFile* fOut, const char* treeName)
{
    cout << "Processing Tree: " << treeName << "..." << endl;

    // 1. Get Input Tree safely
    TTree* tIn = (TTree*)fIn->Get(treeName);
    if (!tIn) {
        cout << "Warning: Cannot find TTree '" << treeName << "' in input file. Skipping." << endl;
        return;
    }

    // 2. Setup Input Variables
    Int_t runID;
    Double_t z_st1 = 676.587, z_st3 = 1959.12;

    Double_t x1_st1, y1_st1, x1_st3, y1_st3;
    Double_t px1_st1, py1_st1, pz1_st1, px1_st3, py1_st3, pz1_st3;

    Double_t x2_st1, y2_st1, x2_st3, y2_st3;
    Double_t px2_st1, py2_st1, pz2_st1, px2_st3, py2_st3, pz2_st3;

    tIn->SetBranchAddress("runID",   &runID);

    // Track 1
    tIn->SetBranchAddress("x1_st1",  &x1_st1);
    tIn->SetBranchAddress("y1_st1",  &y1_st1);
    tIn->SetBranchAddress("px1_st1", &px1_st1);
    tIn->SetBranchAddress("py1_st1", &py1_st1);
    tIn->SetBranchAddress("pz1_st1", &pz1_st1);
    tIn->SetBranchAddress("x1_st3",  &x1_st3);
    tIn->SetBranchAddress("y1_st3",  &y1_st3);
    tIn->SetBranchAddress("px1_st3", &px1_st3);
    tIn->SetBranchAddress("py1_st3", &py1_st3);
    tIn->SetBranchAddress("pz1_st3", &pz1_st3);

    // Track 2
    tIn->SetBranchAddress("x2_st1",  &x2_st1);
    tIn->SetBranchAddress("y2_st1",  &y2_st1);
    tIn->SetBranchAddress("px2_st1", &px2_st1);
    tIn->SetBranchAddress("py2_st1", &py2_st1);
    tIn->SetBranchAddress("pz2_st1", &pz2_st1);
    tIn->SetBranchAddress("x2_st3",  &x2_st3);
    tIn->SetBranchAddress("y2_st3",  &y2_st3);
    tIn->SetBranchAddress("px2_st3", &px2_st3);
    tIn->SetBranchAddress("py2_st3", &py2_st3);
    tIn->SetBranchAddress("pz2_st3", &pz2_st3);

    // 3. Setup Output Tree
    fOut->cd();
    TTree* tOut = tIn->CloneTree(0);

    std::vector<int>* posRoad = new std::vector<int>();
    std::vector<int>* negRoad = new std::vector<int>();

    tOut->Branch("posRoad", &posRoad);
    tOut->Branch("negRoad", &negRoad);

    // 4. Processing Loop
    Long64_t nEntries = tIn->GetEntries();
    int current_run_id_loaded = -1;

    for (Long64_t i = 0; i < nEntries; i++) {
        tIn->GetEntry(i);

        if (runID != current_run_id_loaded) {
            UtilGeom::Init(runID, "../opts");
            current_run_id_loaded = runID;
        }

        posRoad->clear();
        TVector3 pos1_st1(x1_st1, y1_st1, z_st1);
        TVector3 mom1_st1(px1_st1, py1_st1, pz1_st1);
        TVector3 pos1_st3(x1_st3, y1_st3, z_st3);
        TVector3 mom1_st3(px1_st3, py1_st3, pz1_st3);
        *posRoad = UtilTrack::FindMatchedRoads(pos1_st1, mom1_st1, pos1_st3, mom1_st3);

        negRoad->clear();
        TVector3 pos2_st1(x2_st1, y2_st1, z_st1);
        TVector3 mom2_st1(px2_st1, py2_st1, pz2_st1);
        TVector3 pos2_st3(x2_st3, y2_st3, z_st3);
        TVector3 mom2_st3(px2_st3, py2_st3, pz2_st3);
        *negRoad = UtilTrack::FindMatchedRoads(pos2_st1, mom2_st1, pos2_st3, mom2_st3);

        tOut->Fill();
    }

    // 5. Save this tree
    tOut->Write("", TObject::kOverwrite);

    delete posRoad;
    delete negRoad;
}

void AddRoadsToTree(const char* inputFileName, const char* outputFileName)
{
    TFile* fIn = TFile::Open(inputFileName, "READ");
    if (!fIn || fIn->IsZombie()) {
        cout << "Error: Cannot open input file: " << inputFileName << endl;
        return;
    }

    cout << "Creating Output File: " << outputFileName << endl;
    TFile* fOut = new TFile(outputFileName, "RECREATE");

    // Dynamically check for trees inside the file to process
    TIter next(fIn->GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)next())) {
        if (strcmp(key->GetClassName(), "TTree") == 0) {
            ProcessSingleTree(fIn, fOut, key->GetName());
        }
    }

    fOut->Close();
    fIn->Close();

    delete fOut;
    delete fIn;
}
