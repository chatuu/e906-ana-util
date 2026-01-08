#ifndef _UTIL_GEOM__H_
#define _UTIL_GEOM__H_
#include <string>

namespace UtilGeom {
  void Init(const int run_id, const std::string dir_opts="opts");
  void Init(const std::string ds, const std::string dir_opts="opts");
}; // namespace UtilGeom

#endif // _UTIL_GEOM__H_
