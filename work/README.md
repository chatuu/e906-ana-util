# e906-ana-util/work

This directory should be used with a library file built from the source files.


## Basic Usage

```
source ../setup.sh
cmake-this
make-this
root -b TestOneTrack.C
```

Run ID (`run_id`) and track parameters (`pos1`, `mom1`, etc.) are hard-coded in the macro.
The macro prints road IDs (and corresponding hodo IDs) that match to the given track.
