module;

#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <format>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
#include <optional>
#include <random>
#include <regex>
#include <sstream>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

// clang-format off

#ifndef LZ_STANDALONE
  #include <fmt/format.h>
  #include <fmt/ostream.h>
#endif

// clang-format on

export module lz;

#ifndef LZ_MODULE_EXPORT
#define LZ_MODULE_EXPORT export
#define LZ_MODULE_EXPORT_SCOPE_BEGIN export {
#define LZ_MODULE_EXPORT_SCOPE_END }
#endif

#include "Lz/algorithm.hpp"
#include "Lz/any_iterable.hpp"
#include "Lz/c_string.hpp"
#include "Lz/cached_size.hpp"
#include "Lz/cartesian_product.hpp"
#include "Lz/chunk_if.hpp"
#include "Lz/chunks.hpp"
#include "Lz/common.hpp"
#include "Lz/concatenate.hpp"
#include "Lz/drop.hpp"
#include "Lz/drop_while.hpp"
#include "Lz/enumerate.hpp"
#include "Lz/except.hpp"
#include "Lz/exclude.hpp"
#include "Lz/exclusive_scan.hpp"
#include "Lz/filter.hpp"
#include "Lz/flatten.hpp"
#include "Lz/generate.hpp"
#include "Lz/generate_while.hpp"
#include "Lz/group_by.hpp"
#include "Lz/inclusive_scan.hpp"
#include "Lz/interleave.hpp"
#include "Lz/intersection.hpp"
#include "Lz/iter_tools.hpp"
#include "Lz/join_where.hpp"
#include "Lz/loop.hpp"
#include "Lz/map.hpp"
#include "Lz/optional.hpp"
#include "Lz/random.hpp"
#include "Lz/range.hpp"
#include "Lz/regex_split.hpp"
#include "Lz/repeat.hpp"
#include "Lz/reverse.hpp"
#include "Lz/rotate.hpp"
#include "Lz/slice.hpp"
#include "Lz/split.hpp"
#include "Lz/stream.hpp"
#include "Lz/string_view.hpp"
#include "Lz/take.hpp"
#include "Lz/take_every.hpp"
#include "Lz/take_while.hpp"
#include "Lz/unique.hpp"
#include "Lz/zip.hpp"
#include "Lz/zip_longest.hpp"
