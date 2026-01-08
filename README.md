# e906-ana-util

A set of utility functions for E906 analyses.
The only main function at present is to find roads that match to a given track.


## Simple Usage

If you don't care about the processing speed, you can use the files under `simple/`.
They depend on `setup.sh` and `opts/` (not `src/`).
Please refer to `simple/README.md`.


## Adavnced Usage

You can use `work/` in case you care about speed and/or flexibility.
ROOT macros under `work/` loads a library that is built from the source files under `src/`.
Please refer to `work/README.md`.
