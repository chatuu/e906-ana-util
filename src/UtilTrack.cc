#include <cassert>
#include <GeomSvc.h>
#include <SRecEvent.h>
#include "UtilTrigger.h"
#include "UtilTrack.h"
using namespace std;

int UtilTrack::verbosity = 0;

/// Find all roads that match to the given track within the hodo paddle width plus the given margin.
/**
 * The closest paddle to the given track at each hodoscope X plane (H1T/B, H2T/B, H3T/B and H4T/B) is selected based on the expected track x-position.
 * The two adjacent paddles are also selected if they are within "width/2 + margin".
 * All selected paddles are used to list up matched roads.
 * @code
 *   SRecTrack* trk = ...
 *   const double margin = 1.0; // in cm
 *   vector<int> list_road_id = UtilTrack::FindMatchedRoad(trk, margin);
 * @endcode
 */
std::vector<int> UtilTrack::FindMatchedRoads(SRecTrack* trk, const double margin)
{
  return FindMatchedRoads(trk->getPositionVecSt1(), trk->getMomentumVecSt1(), trk->getPositionVecSt3(), trk->getMomentumVecSt3(), margin);
}

/// Find all roads that match to the given track within the hodo paddle width plus the given margin.
std::vector<int> UtilTrack::FindMatchedRoads(const TVector3 pos1, const TVector3 mom1, const TVector3 pos3, const TVector3 mom3, const double margin)  
{
  GeomSvc* geom = GeomSvc::instance();
  double y_st1 = pos1.Y();
  double y_st3 = pos3.Y();
  int top_bot = y_st3>0 ? +1 : -1;
  if (verbosity > 0) cout << "UtilTrack::FindMatchedRoads(): y_st1=" << y_st1 << " y_st3=" << y_st3 << " top_bot=" << top_bot << endl;
  
  vector<int> list_ele_id[5]; // 0=unused, 1=st1, 2=st2,,,
  for (int st = 1; st <= 4; st++) {
    string det_name = (string)"H" + (char)('0'+st) + (top_bot>0 ? 'T' : 'B');
    int det_id = geom->getDetectorID(det_name);
    Plane  det = geom->getPlane(det_id);
    double x_det = det.xc;
    double y_det = det.yc;
    double z_det = det.zc;
    double sinth = det.sintheta;
    double costh = det.costheta;
    int    n_ele = det.nElements;
    double space = det.spacing;
    double width = det.cellWidth;
    if (verbosity > 2) cout << "  st" << st << ":";
    if (verbosity > 3) cout << " x,y,z_det=" << x_det << "," << y_det << "," << z_det << " sin,cos=" << sinth << "," << costh << " n_ele=" << n_ele << " space=" << space << " width=" << width;

    const TVector3* pos = (st == 1 ? &pos1 : &pos3);
    const TVector3* mom = (st == 1 ? &mom1 : &mom3);
    double x_trk = pos->X() + (z_det - pos->Z()) * mom->X() / mom->Z();
    double y_trk = pos->Y() + (z_det - pos->Z()) * mom->Y() / mom->Z();
    
    // d_trk = Signed distance of the track position from the plane center axis.
    //   Center axis: y = y_det-(x-x_det)*costh/sinth ->  costh*x + sinth*y - sinth*y_det - costh*x_det = 0
    //   Note: theta > 0 for the 2nd quadrant (x<0, y>0).
    double d_trk = costh*x_trk + sinth*y_trk - sinth*y_det - costh*x_det; // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
    int ele_cent = (int)((n_ele+1)/2.0 + d_trk/space + 0.5);
    if (verbosity > 2) cout << " x,y,d_trk=" << x_trk << "," << y_trk << "," << d_trk;

    /// Check three elements, assuming the margin is much smaller than the half width.
    for (int i_ele = ele_cent - 1; i_ele <= ele_cent + 1; i_ele++) {
      if (i_ele <= 0 || i_ele > n_ele) continue;
      double d_ele = space * (i_ele - (n_ele+1)/2.0);
      if (verbosity > 2) cout << "  [ele=" << i_ele << " d=" << d_ele;
      if (verbosity > 3) cout << " edge=" << (width/2-fabs(d_trk-d_ele));
      if (verbosity > 2) cout << "]";
      if (fabs(d_trk - d_ele) < width/2 + margin) list_ele_id[st].push_back(i_ele);
    }
    if (verbosity > 2) cout << endl;
  }
  vector<int> list_road_id;
  for (auto it1 = list_ele_id[1].begin(); it1 != list_ele_id[1].end(); it1++) {
  for (auto it2 = list_ele_id[2].begin(); it2 != list_ele_id[2].end(); it2++) {
  for (auto it3 = list_ele_id[3].begin(); it3 != list_ele_id[3].end(); it3++) {
  for (auto it4 = list_ele_id[4].begin(); it4 != list_ele_id[4].end(); it4++) {
    if (verbosity > 0) cout << "  road " << *it1 << " " << *it2 << " " << *it3 << " " << *it4;
    if (verbosity > 1) cout << " " << UtilTrigger::Hodo2Road(*it1, *it2, *it3, *it4, top_bot);
    if (verbosity > 0) cout << endl;
    list_road_id.push_back(UtilTrigger::Hodo2Road(*it1, *it2, *it3, *it4, top_bot));
  }}}}
  return list_road_id;
}
