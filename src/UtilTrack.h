#ifndef _UTIL_TRACK__H_
#define _UTIL_TRACK__H_
#include <vector>
class SRecTrack;
class TVector3;
//class TLorentzVector;

namespace UtilTrack {
  extern int verbosity;

  std::vector<int> FindMatchedRoads(SRecTrack* trk, const double margin=0);
  std::vector<int> FindMatchedRoads(const TVector3 pos1, const TVector3 mom1, const TVector3 pos3, const TVector3 mom3, const double margin=0);
}

#endif /* _UTIL_TRACK__H_ */
