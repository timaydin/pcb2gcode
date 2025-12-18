#define BOOST_TEST_MODULE eulerian paths tests
#include <boost/test/unit_test.hpp>

#include <tuple>
#include <utility>
#include "geometry_int.hpp"

#include "eulerian_paths.hpp"
#include "bg_operators.hpp"

using std::vector;
using std::pair;
using std::make_pair;
using namespace eulerian_paths;

BOOST_AUTO_TEST_SUITE(eulerian_paths_tests)

template <typename linestring_type>
static void print_euler_paths(vector<pair<linestring_type, bool>> euler_paths) {
  for (const auto& euler_path : euler_paths) {
    for (const auto& point : euler_path.first) {
      std::cout << point << " ";
    }
    std::cout << (euler_path.second ? "reversible" : "not-reversible") << std::endl;
  }
}

template <typename point_type, typename linestring_type>
void run_test(vector<pair<linestring_type, bool>> mls, size_t expected_size) {
  vector<pair<linestring_type, bool>> result =
    get_eulerian_paths<point_type, linestring_type>(mls);
  std::string test_name = boost::unit_test::framework::current_test_case().full_name();
  std::cout << "Test " << test_name << " result:" << std::endl;
  print_euler_paths(result);
  std::cout << std::endl;
  BOOST_CHECK(check_eulerian_paths<point_type>(mls, result));
  BOOST_CHECK_EQUAL(result.size(), expected_size);
}


BOOST_AUTO_TEST_CASE(do_nothing_points) {
  linestring_type ls;
  ls.push_back(point_type(1,1));
  ls.push_back(point_type(2,2));
  ls.push_back(point_type(3,4));
  vector<pair<linestring_type, bool>> mls;
  mls.push_back(make_pair(ls, true));
  run_test<point_type>(mls, 1);
}

// 3x3 grid connected like a window pane:
// 1---2---3
// |   |   |
// 4---5---6
// |   |   |
// 7---8---9
BOOST_AUTO_TEST_CASE(window_pane) {
  vector<pair<vector<int>, bool>> mls{
      make_pair(vector<int>{1,2}, true),
      make_pair(vector<int>{2,3}, true),
      make_pair(vector<int>{4,5}, true),
      make_pair(vector<int>{5,6}, true),
      make_pair(vector<int>{7,8}, true),
      make_pair(vector<int>{8,9}, true),
      make_pair(vector<int>{1,4}, true),
      make_pair(vector<int>{4,7}, true),
      make_pair(vector<int>{2,5}, true),
      make_pair(vector<int>{5,8}, true),
      make_pair(vector<int>{3,6}, true),
      make_pair(vector<int>{6,9}, true),
    };
  run_test<int>(mls, 2);
}

// 3x3 grid connected like a window pane, but corners are longer paths:
// 1---2---3
// |   |   |
// 4---5---6
// |   |   |
// 7---8---9
BOOST_AUTO_TEST_CASE(window_pane_with_longer_corners) {
  vector<pair<vector<int>, bool>> mls{
      make_pair(vector<int>{4,5}, true),
      make_pair(vector<int>{5,6}, true),
      make_pair(vector<int>{4,7,8}, true),
      make_pair(vector<int>{2,5}, true),
      make_pair(vector<int>{5,8}, true),
      make_pair(vector<int>{6,9,8}, true),
      make_pair(vector<int>{4,1,2}, true),
      make_pair(vector<int>{2,3,6}, true),
    };
  run_test<int>(mls, 2);
}

// Bridge
// 5---2---1---6
// |   |   |   |
// 3---4   7---8
BOOST_AUTO_TEST_CASE(bridge) {
  vector<pair<vector<int>, bool>> mls{
      make_pair(vector<int>{5,2}, true),
      make_pair(vector<int>{2,1}, true),
      make_pair(vector<int>{1,6}, true),
      make_pair(vector<int>{3,4}, true),
      make_pair(vector<int>{7,8}, true),
      make_pair(vector<int>{5,3}, true),
      make_pair(vector<int>{2,4}, true),
      make_pair(vector<int>{1,7}, true),
      make_pair(vector<int>{6,8}, true),
    };
  run_test<int>(mls, 1);
}

// Disjoint Loops and two degenerate paths
// 5---2   1---6  0---9
// |   |   |   |
// 3---4   7---8
BOOST_AUTO_TEST_CASE(disjoint_loops) {
  vector<pair<vector<int>, bool>> mls{
      make_pair(vector<int>{5,2}, true),
      make_pair(vector<int>{1,6}, true),
      make_pair(vector<int>{3,4}, true),
      make_pair(vector<int>{7,8}, true),
      make_pair(vector<int>{5,3}, true),
      make_pair(vector<int>{2,4}, true),
      make_pair(vector<int>{1,7}, true),
      make_pair(vector<int>{6,8}, true),
      make_pair(vector<int>{0,9}, true),
      make_pair(vector<int>{}, true),
      make_pair(vector<int>{12}, true),
    };
  run_test<int>(mls, 3);
}

// bidi and directional together
// 1-->2
// |   |
// v   |
// 3---4
BOOST_AUTO_TEST_CASE(mixed1) {
  vector<pair<vector<int>, bool>> mls{
      make_pair(vector<int>{1,2}, false),
      make_pair(vector<int>{1,3}, false),
      make_pair(vector<int>{2,4}, true),
      make_pair(vector<int>{3,4}, true),
    };
  run_test<int>(mls, 2);
}

// bidi and directional together
// 1<--2
// |   |
// v   |
// 3---4
BOOST_AUTO_TEST_CASE(mixed2) {
  vector<pair<vector<int>, bool>> mls{
      make_pair(vector<int>{2,1}, false),
      make_pair(vector<int>{1,3}, false),
      make_pair(vector<int>{2,4}, true),
      make_pair(vector<int>{3,4}, true),
    };
  run_test<int>(mls, 1);
}

// 3x3 grid bidi
// 1---2---3
// |   |   |
// |   v   |
// 4-->5<--6
// |   ^   |
// |   |   |
// 7---8---9
BOOST_AUTO_TEST_CASE(mixed3) {
  vector<pair<vector<int>, bool>> mls{
      make_pair(vector<int>{1,2}, true),
      make_pair(vector<int>{2,3}, true),
      make_pair(vector<int>{1,4}, true),
      make_pair(vector<int>{2,5}, false),
      make_pair(vector<int>{3,6}, true),
      make_pair(vector<int>{4,5}, false),
      make_pair(vector<int>{6,5}, false),
      make_pair(vector<int>{4,7}, true),
      make_pair(vector<int>{8,5}, false),
      make_pair(vector<int>{6,9}, true),
      make_pair(vector<int>{7,8}, true),
      make_pair(vector<int>{8,9}, true),
    };
  run_test<int>(mls, 4);
}

// At least one of the paths must be turned around.
BOOST_AUTO_TEST_CASE(start_second) {
  vector<pair<vector<int>, bool>> mls{
      make_pair(vector<int>{0,1}, true),
      make_pair(vector<int>{0,2}, true),
    };
  run_test<int>(mls, 1);
}

// Directional paths with a loop.
BOOST_AUTO_TEST_CASE(directional_loop) {
  vector<pair<vector<int>, bool>> mls{
      make_pair(vector<int>{0, 0}, false),
      make_pair(vector<int>{1, 0}, false),
    };
  run_test<int>(mls, 1);
}

// Prefer straight lines.
// Draw a windmill shape.
BOOST_AUTO_TEST_CASE(prefer_straight_lines) {
  vector<pair<linestring_type_fp, bool>> mls{
    {{{5,5}, {6,0}, {4,0}, {5,5}}, true},
    {{{5,5}, {10,4}, {10, 6}, {5,5}}, true},
    {{{5,5}, {4,10}, {6, 10}, {5,5}}, true},
    {{{5,5}, {0,4}, {0, 6}, {5,5}}, true},
  };
  vector<pair<linestring_type_fp, bool>> result =
      get_eulerian_paths<point_type_fp, linestring_type_fp>(mls);
  vector<pair<linestring_type_fp, bool>> expected{
    {
      {
        {5, 5},
        {6, 0},
        {4, 0},
        {5, 5},
        {6, 10},
        {4, 10},
        {5, 5},
        {10, 4},
        {10, 6},
        {5, 5},
        {0, 4},
        {0, 6},
        {5, 5},
      },
     true},
  };
  BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(must_start_tests) {
  vector<std::tuple<size_t, size_t, size_t, bool>> tests{
    // Sum = 0
    std::make_tuple(0, 0, 0, false),

    // Sum = 1
    std::make_tuple(0, 0, 1, true),
    std::make_tuple(0, 1, 0, false),
    std::make_tuple(1, 0, 0, true),

    // Sum = 2
    std::make_tuple(0, 0, 2, false),
    std::make_tuple(0, 1, 1, false),
    std::make_tuple(0, 2, 0, false),
    std::make_tuple(1, 0, 1, false),
    std::make_tuple(1, 1, 0, false),
    std::make_tuple(2, 0, 0, true),

    // Sum = 3
    std::make_tuple(0, 0, 3, true),
    std::make_tuple(0, 1, 2, true),
    std::make_tuple(0, 2, 1, false),
    std::make_tuple(0, 3, 0, false),
    std::make_tuple(1, 0, 2, true),
    std::make_tuple(1, 1, 1, true),
    std::make_tuple(1, 2, 0, false),
    std::make_tuple(2, 0, 1, true),
    std::make_tuple(2, 1, 0, true),
    std::make_tuple(3, 0, 0, true),

    // Sum = 4
    std::make_tuple(0, 0, 4, false),
    std::make_tuple(0, 1, 3, false),
    std::make_tuple(0, 2, 2, false),
    std::make_tuple(0, 3, 1, false),
    std::make_tuple(0, 4, 0, false),
    std::make_tuple(1, 0, 3, false),
    std::make_tuple(1, 1, 2, false),
    std::make_tuple(1, 2, 1, false),
    std::make_tuple(1, 3, 0, false),
    std::make_tuple(2, 0, 2, false),
    std::make_tuple(2, 1, 1, false),
    std::make_tuple(2, 2, 0, false),
    std::make_tuple(3, 0, 1, true),
    std::make_tuple(3, 1, 0, true),
    std::make_tuple(4, 0, 0, true),
  };
  for (const auto& test : tests) {
    BOOST_TEST_CONTEXT("must_start_helper(" << std::get<0>(test) << ", " << std::get<1>(test) << ", " << std::get<2>(test) << ")") {
      BOOST_CHECK_EQUAL(must_start_helper(std::get<0>(test), std::get<1>(test), std::get<2>(test)), std::get<3>(test));
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
