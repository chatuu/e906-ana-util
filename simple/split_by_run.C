#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <set>
#include <TString.h>
#include <TSystem.h> // Required for gSystem->mkdir()

// Helper function to process and split a single tree independently
void SplitTree(TTree* tree, const char* treeName, const char* outDir, const char* filePrefix) {
    if (!tree) return;

    std::cout << "Scanning runIDs in tree: " << treeName << std::endl;
    std::set<int> uniqueRuns;
    int runID;

    // Optimization: Disable all branches except runID for fast scanning
    tree->SetBranchStatus("*", 0);
    tree->SetBranchStatus("runID", 1);

    if (tree->GetBranch("runID") == nullptr) {
        std::cout << "Error: 'runID' branch not found in " << treeName << std::endl;
        return;
    }

    tree->SetBranchAddress("runID", &runID);

    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        tree->GetEntry(i);
        uniqueRuns.insert(runID);
    }

    // Re-enable all branches for the copying phase
    tree->SetBranchStatus("*", 1);

    // Create separate files for each Run ID
    for (int run : uniqueRuns) {
        // Create the directory structure: outDir/runID/treeName/
        TString runDir = Form("%s/%d/%s", outDir, run, treeName);
        gSystem->mkdir(runDir, kTRUE); // kTRUE means recursive (like mkdir -p)

        // Output file explicitly names the tree inside it
        TString outFileName = Form("%s/%s_%s_run_%d.root", runDir.Data(), filePrefix, treeName, run);
        TFile *fout = TFile::Open(outFileName, "RECREATE");

        TString selection = Form("runID == %d", run);
        TTree *t_new = tree->CopyTree(selection);

        fout->Write();
        fout->Close();
    }
}

void split_by_run(const char* inFileName, const char* outDir, const char* filePrefix) {
    TFile *fin = TFile::Open(inFileName, "READ");
    if (!fin || fin->IsZombie()) {
        std::cout << "Error: Could not open input file: " << inFileName << std::endl;
        return;
    }

    TTree *t_result = (TTree*)fin->Get("result");
    TTree *t_mix    = (TTree*)fin->Get("result_mix");

    std::cout << "Splitting file: " << inFileName << std::endl;

    // Process both TTrees separately
    if (t_result) SplitTree(t_result, "result", outDir, filePrefix);
    else std::cout << "Warning: Could not find 'result' tree in " << inFileName << std::endl;

    if (t_mix) SplitTree(t_mix, "result_mix", outDir, filePrefix);
    else std::cout << "Warning: Could not find 'result_mix' tree in " << inFileName << std::endl;

    fin->Close();
}
