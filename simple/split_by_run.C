#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <set>
#include <TString.h>

void split_by_run(const char* inFileName = "/seaquest/users/apun/e906_projects/rs67_merged_files/merged_RS67_3089flask.root") {

    // 1. Open the input file
    TFile *fin = TFile::Open(inFileName, "READ");
    if (!fin || fin->IsZombie()) {
        std::cout << "Error: Could not open input file: " << inFileName << std::endl;
        return;
    }

    // 2. Retrieve the original trees
    TTree *t_result = (TTree*)fin->Get("result");
    TTree *t_mix    = (TTree*)fin->Get("result_mix");

    if (!t_mix) {
        std::cout << "Error: Could not find 'result_mix' tree." << std::endl;
        fin->Close();
        return;
    }

    std::cout << "Processing file: " << inFileName << std::endl;

    // ---------------------------------------------------------
    // STEP A: Scan the tree to find all unique runIDs
    // ---------------------------------------------------------
    std::cout << "Scanning for unique Run IDs..." << std::endl;

    std::set<int> uniqueRuns;
    int runID;

    // optimization: Disable all branches except runID for fast scanning
    t_mix->SetBranchStatus("*", 0);
    t_mix->SetBranchStatus("runID", 1);

    // Check if branch exists
    if (t_mix->GetBranch("runID") == nullptr) {
         std::cout << "Error: 'runID' branch not found in result_mix." << std::endl;
         return;
    }

    t_mix->SetBranchAddress("runID", &runID);

    Long64_t nEntries = t_mix->GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        t_mix->GetEntry(i);
        uniqueRuns.insert(runID);
    }

    std::cout << "Found " << uniqueRuns.size() << " unique runs." << std::endl;

    // Re-enable all branches for the copying phase
    t_mix->SetBranchStatus("*", 1);

    // ---------------------------------------------------------
    // STEP B: Create separate files for each Run ID
    // ---------------------------------------------------------
    for (int run : uniqueRuns) {

        // Create a unique filename, e.g., "output_run_12345.root"
        TString outFileName = Form("output_skim_run_%d.root", run);
        std::cout << "Writing Run " << run << " to " << outFileName << "..." << std::endl;

        TFile *fout = TFile::Open(outFileName, "RECREATE");

        // Define the selection criteria string
        TString selection = Form("runID == %d", run);

        // Copy the subset of trees
        // CopyTree automatically creates the new tree in the current directory (fout)
        TTree *t_mix_new = t_mix->CopyTree(selection);

        // Also copy the 'result' tree if it exists and has runID
        if (t_result) {
            // We assume 'result' also has a runID branch.
            // If not, this CopyTree might return an empty tree or full tree depending on logic,
            // but standard standard SeaQuest/E906 trees usually share structure.
            TTree *t_result_new = t_result->CopyTree(selection);
        }

        // Write and Close the current output file
        fout->Write();
        fout->Close();

        // Note: t_mix_new and t_result_new are owned by fout and deleted when fout->Close() is called.
    }

    std::cout << "Done. All runs have been split into separate files." << std::endl;

    // Clean up input
    fin->Close();
}
