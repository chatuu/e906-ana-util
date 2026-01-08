#include "UtilGeom.h"
#include "UtilTrack.h"
#include "UtilTrigger.h"
using namespace std;

void TestOneTrack()
{
  const int run_id = 10000;
  UtilGeom::Init(run_id, "../opts");
  cout << endl;

  const TVector3 pos1( 0.7, -37.0,  676.587);
  const TVector3 mom1(-1.0,   0.1,   20.0);
  const TVector3 pos3(-1.5, -84.0, 1959.12);
  const TVector3 mom3(-1.0,   0.1,   20.0);
  vector<int> list_road_id = UtilTrack::FindMatchedRoads(pos1, mom1, pos3, mom3);

  for (auto it = list_road_id.begin(); it != list_road_id.end(); it++) {
    int road_id = *it;
    int h1, h2, h3, h4, tb;
    UtilTrigger::Road2Hodo(road_id, h1, h2, h3, h4, tb);
    cout << "Road " << road_id << " --> Hodo " << h1 << " " << h2 << " " << h3 << " " << h4 << "  T/B " << tb << endl;
  }

  exit(0);
}
