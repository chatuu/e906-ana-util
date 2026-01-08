# e906-ana-util/simple:  A Set of Simple Macros to Find Track Roads

It contains a set of ROOT macros (*.C & *.h) that finds roads matching to a given track.
It doesn't depend on `../src/`.


## Basic Usage

```
source ../setup.sh
root -b TestOneTrack.C
```

Run ID (`run_id`) and track parameters (`pos1`, `mom1`, etc.) are hard-coded in the macro.
The macro prints road IDs (and corresponding hodo IDs) that match to the given track.

You might modify `TestOneTrack.C` (or create a similar macro) to do what you need.
For example you might
* Read a tracked file (like `track_012525_r1.7.0.root`),
* Find matched roads for each track, and
* Write to an output file the matched roads together with the original variables.


## Advanced Usage

All functions necessary for the road finding are defined in `UtilGeom.h`, `UtilTrack.h` and `UtilTrigger.h`.
You might copy these header files into your own program.
In such case you have to
* Properly set the environmental variable as done in `setup.sh`, and
* Use a valid path to `opts/` when calling `UtilGeom::Init()`
in your program.
