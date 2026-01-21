#!/bin/bash

# 1. Define locations
# Adjust this path to exactly where your AddRoadsToTree.C file is located
MACRO_DIR="/seaquest/users/ckuruppu/Kenichi/e906-ana-util/simple"

# Adjust this path to where your input ROOT files are located
DATA_DIR="${MACRO_DIR}/ROOTFiles/Flask_Mixed"

# 2. Check directories
if [ ! -d "$MACRO_DIR" ]; then
    echo "Error: Macro directory $MACRO_DIR does not exist."
    exit 1
fi

if [ ! -d "$DATA_DIR" ]; then
    echo "Error: Data directory $DATA_DIR does not exist."
    exit 1
fi

echo "Starting parallel processing..."
echo "Macro Dir: $MACRO_DIR"

# 3. Enter the macro directory
# This is crucial: it ensures ROOT is launched from the same place
# as your manual test, allowing it to find necessary headers.
cd "$MACRO_DIR" || exit

count=0

# 4. Loop over files
for file in "$DATA_DIR"/*.root; do

    # Check if file exists
    [ -e "$file" ] || continue

    echo "Submitting: $file"

    # 5. Run ROOT
    # We use the exact syntax that worked for you manually.
    # We pass \"\" as the second argument because the macro handles the naming logic internally.
    root -l -b -q "AddRoadsToTree.C(\"$file\", \"\")" &

    ((count++))

    # Wait for background jobs to finish every 4 submissions
    if (( count % 4 == 0 )); then
        echo "--- Waiting for batch of 4 to finish ---"
        wait
    fi

done

# Wait for any remaining jobs
wait
echo "All processing complete."
