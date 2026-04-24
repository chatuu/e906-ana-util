#!/bin/bash

# ==========================================
# Pipeline Execution Switches (1 = ON, 0 = OFF)
# ==========================================
RUN_PHASE1=1  # Split root files
RUN_PHASE2=1  # Add posRoad/negRoad (Requires Phase 1 output)
RUN_PHASE3=1  # Merge into final output (Requires Phase 2 output)

# Configuration
INPUT_LIST="input_root_files_new.txt"
MACRO_DIR=$(pwd)

# Hardcoded to 8 workers to prevent system lockups
CORES=8

SPLIT_DIR="${MACRO_DIR}/tmp_split_new"
PROC_DIR="${MACRO_DIR}/tmp_processed_new"
FINAL_DIR="${MACRO_DIR}/final_output_new"

mkdir -p "$SPLIT_DIR" "$PROC_DIR" "$FINAL_DIR"

echo "========================================"
echo " Starting Pipeline with $CORES Cores"
echo " Phase 1 (Split)   : $RUN_PHASE1"
echo " Phase 2 (Process) : $RUN_PHASE2"
echo " Phase 3 (Merge)   : $RUN_PHASE3"
echo "========================================"

# ---------------------------------------------------------
# PHASE 1: Split files by RunID and TTree (Optimized Parallel)
# ---------------------------------------------------------
if [ "$RUN_PHASE1" -eq 1 ]; then
    echo "[Phase 1] Splitting root files using $CORES parallel workers..."
    
    # xargs keeps exactly $CORES jobs running at all times
    cat "$INPUT_LIST" | grep -v '^$' | xargs -I {} -P "$CORES" bash -c '
        filepath="{}"
        basename=$(basename "$filepath" .root)
        root -l -b -q "'"${MACRO_DIR}"'/split_by_run.C(\"${filepath}\", \"'"${SPLIT_DIR}"'\", \"${basename}\")" > /dev/null 2>&1
    '

    echo "[Phase 1] Splitting complete."
else
    echo "[Phase 1] Skipped."
fi

# ---------------------------------------------------------
# PHASE 2: Process split files (Optimized Parallel)
# ---------------------------------------------------------
if [ "$RUN_PHASE2" -eq 1 ]; then
    echo "[Phase 2] Cleaning up old processed and final files..."
    # Using rm -rf because we now have nested directories
    rm -rf "${PROC_DIR:?}"/*
    rm -rf "${FINAL_DIR:?}"/*

    echo "[Phase 2] Adding posRoad and negRoad to split files using $CORES workers..."
    
    # Use find to recursively locate files in the new <runID>/<tree> folder structure
    find "${SPLIT_DIR}" -type f -name "*.root" | xargs -I {} -P "$CORES" bash -c '
        split_file="{}"
        
        # Calculate relative path to mirror directory structure in PROC_DIR
        rel_path="${split_file#'"${SPLIT_DIR}"'/}"
        out_file="'"${PROC_DIR}"'/${rel_path}"
        out_dir=$(dirname "$out_file")
        
        # Ensure the subdirectories exist before running ROOT
        mkdir -p "$out_dir"
        
        log_file="${out_file}.log"

        root -l -b -q "'"${MACRO_DIR}"'/AddRoadsToTree.C(\"${split_file}\", \"${out_file}\")" > "$log_file" 2>&1
    '

    echo "[Phase 2] Processing complete. Check tmp_processed_new logs for any errors."
else
    echo "[Phase 2] Skipped."
fi

# ---------------------------------------------------------
# PHASE 3: Merge chunks back into final files
# ---------------------------------------------------------
if [ "$RUN_PHASE3" -eq 1 ]; then
    echo "[Phase 3] Merging processed chunks back to final files..."

    while IFS= read -r filepath; do
        [ -z "$filepath" ] && continue

        basename=$(basename "$filepath" .root)
        
        # 1. Merge "result" tree chunks
        final_file_result="${FINAL_DIR}/${basename}_result_FinalData.root"
        # Find all chunks scattered in PROC_DIR/<runID>/result/
        chunks_result=$(find "${PROC_DIR}" -type f -name "${basename}_result_run_*.root")
        
        if [ -n "$chunks_result" ]; then
            echo "Merging into: $final_file_result"
            # shellcheck disable=SC2086 # We want the variable to expand to multiple file arguments
            hadd -f -k "$final_file_result" $chunks_result > /dev/null 2>&1
        fi

        # 2. Merge "result_mix" tree chunks
        final_file_mix="${FINAL_DIR}/${basename}_result_mix_FinalData.root"
        # Find all chunks scattered in PROC_DIR/<runID>/result_mix/
        chunks_mix=$(find "${PROC_DIR}" -type f -name "${basename}_result_mix_run_*.root")
        
        if [ -n "$chunks_mix" ]; then
            echo "Merging into: $final_file_mix"
            # shellcheck disable=SC2086
            hadd -f -k "$final_file_mix" $chunks_mix > /dev/null 2>&1
        fi

    done < "$INPUT_LIST"
    echo "[Phase 3] Merging complete."
else
    echo "[Phase 3] Skipped."
fi

echo "========================================"
echo " Pipeline Complete!"
if [ "$RUN_PHASE3" -eq 1 ]; then
    echo " Your final files are located in: $FINAL_DIR"
fi
echo "========================================"
