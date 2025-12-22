#ifndef EULERIAN_PATHS_H
#define EULERIAN_PATHS_H

#include <vector>

#include "geometry.hpp"

namespace eulerian_paths {

bool must_start_helper(size_t out_edges, size_t in_edges, size_t bidi_edges);

template <typename point_t, typename linestring_t>
bool check_eulerian_paths(const std::vector<std::pair<linestring_t, bool>>& before,
                          const std::vector<std::pair<linestring_t, bool>>& after);

template <typename point_t, typename linestring_t>
std::vector<std::pair<linestring_t, bool>> get_eulerian_paths(const std::vector<std::pair<linestring_t, bool>>& paths);

multi_linestring_type_fp make_eulerian_paths(const multi_linestring_type_fp& paths, bool reversible, bool unique);

} // namespace eulerian_paths
#endif //EULERIAN_PATHS_H
