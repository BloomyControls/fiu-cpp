/*
 * Copyright © 2026, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#ifndef FIU_CPP_SRC_UTIL_H
#define FIU_CPP_SRC_UTIL_H

#include <bci/fiu/CommonTypes.h>

#include <charconv>
#include <concepts>
#include <optional>
#include <string_view>
#include <utility>

namespace bci::fiu::util {

template <class T>
constexpr auto Err(T&& val) {
  return tl::unexpected(std::forward<T>(val));
}

template <class T>
requires std::integral<T>
constexpr std::optional<T> ParseInt(std::string_view str, int base = 10) {
  T v{};
  auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), v);
  if (ec != std::errc{}) {
    return std::nullopt;
  }
  return v;
}

}  // namespace bci::fiu::util

#endif  /* FIU_CPP_SRC_UTIL_H */
