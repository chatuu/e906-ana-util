#ifndef _UTIL_GEOM__H_
#define _UTIL_GEOM__H_
#include <JobOptsSvc.h>
#include <GeomSvc.h>
#include <E906DataCatalog.h>
#include "UtilGeom.h"

namespace UtilGeom {

/// Initialize the geometry info by the given dataset name.
void Init(const std::string ds, const std::string dir_opts)
{
  std::string fn_opts = dir_opts + "/dataset_" + ds + ".opts";
  cout << "Opt file =  " << fn_opts << endl;
  JobOptsSvc::instance()->init(fn_opts.c_str());
  GeomSvc::instance()->init();
}
  
/// Initialize the geometry info by the given run ID.
void Init(const int run_id, const std::string dir_opts)
{
  E906DataCatalog* e906dc = E906DataCatalog::instance();
  std::string ds = e906dc->FindRun(run_id);
  if (ds == "") {
    cout << "UtilGeom::Init():  Cannot find the given run (" << run_id << ").  Abort.\n";
    exit(1);
  }
  //e906dc->PreSetDataset(ds);
  //e906dc->PreSetRun(run_id);
  Init(ds, dir_opts);
}

}; // End of "namespace UtilGeom"

#endif // _UTIL_GEOM__H_
