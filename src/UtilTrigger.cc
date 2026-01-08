#include <cmath>
#include "UtilTrigger.h"
using namespace std;
namespace UtilTrigger {

/// Convert a set of hodo IDs to a roadset ID.
/**
 * Note that the max hodo IDs are 23 (H1), 16 (H2), 16 (H3) and 16 (H4).
 */
int Hodo2Road(const int h1, const int h2, const int h3, const int h4, const int tb)
{
  if (h1 * h2 * h3 * h4 * tb == 0) return 0; // if any is 0.
  int road = (h1-1)*16*16*16 + (h2-1)*16*16 + (h3-1)*16 + h4;
  return (tb > 0 ? +1 : -1) * road;
}

/// Convert a roadset ID to a set of hodo IDs.
void Road2Hodo(const int road, int& h1, int& h2, int& h3, int& h4, int& tb)
{
  if (road == 0) {
    h1 = h2 = h3 = h4 = tb = 0;
    return;
  }
  int rr = abs(road) - 1;
  h1 = 1 + (rr/16/16/16);
  h2 = 1 + (rr/16/16   ) %16;
  h3 = 1 + (rr/16      ) %16;
  h4 = 1 + (rr         ) %16;
  tb = road>0 ? +1 : -1;
}

/// Flip the given road ID in the left-right direction.
int FlipRoadLeftRight(const int road)
{
  int h1, h2, h3, h4, tb;
  Road2Hodo(road, h1, h2, h3, h4, tb);
  h1 = 24 - h1; // 1 <-> 23, 2 <-> 22,,,
  h2 = 17 - h2; // 1 <-> 16, 2 <-> 15,,,
  h3 = 17 - h3;
  h4 = 17 - h4;
  return Hodo2Road(h1, h2, h3, h4, tb);
}

/// Flip the given road ID in the top-bottom direction.
int FlipRoadTopBottom(const int road)
{
  int h1, h2, h3, h4, tb;
  Road2Hodo(road, h1, h2, h3, h4, tb);
  return Hodo2Road(h1, h2, h3, h4, -tb);
}
  
}; // End of "namespace UtilTrigger"
