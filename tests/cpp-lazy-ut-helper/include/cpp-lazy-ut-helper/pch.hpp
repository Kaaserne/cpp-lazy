#pragma once

#include <Lz/detail/compiler_config.hpp>
#include <algorithm>
#include <array>
#include <deque>
#include <forward_list>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#ifndef LZ_HAS_CXX_14
#include <memory>
#endif
#include <numeric>
#include <queue>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef LZ_HAS_CXX_17
#include <string_view>
#endif

#ifndef LZ_STANDALONE
#include <fmt/format.h>
#include <fmt/ostream.h>
#endif

#include <Lz/algorithm/empty.hpp>
#include <Lz/algorithm/equal.hpp>
#include <Lz/algorithm/has_many.hpp>
#include <Lz/algorithm/has_one.hpp>
#include <Lz/detail/iterables/reverse.hpp>
