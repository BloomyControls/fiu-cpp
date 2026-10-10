/*
 * Copyright © 2026, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

/**
 * @file
 * @brief Common types for FIU communications.
 */
#ifndef FIU_CPP_INCLUDE_BCI_FIU_COMMONTYPES_H
#define FIU_CPP_INCLUDE_BCI_FIU_COMMONTYPES_H

#include <cstdint>

#include "util/expected.hpp"

namespace bci::fiu {

/// FIU channel count.
inline constexpr unsigned int kChannelCount{24U};

enum class ChannelState : std::uint8_t {
  kDisconnected = 0,
  kConnected = 1,
  kVoltageMeasurement = 2,
  kCurrentMeasurement = 3,
  kGroundFault = 4,
};

enum class ErrorCode : int {
  kSuccess = 0,
  kBoxIDOutOfRange = -1,
  kChannelIndexOutOfRange = -2,
  kInvalidArgument = -3,
  kInvalidResponse = -4,
};

struct FirmwareVersion {
  uint8_t major;
  uint8_t minor;
};

/**
 * @brief Result type used to return values or error codes from driver
 * functions.
 *
 * `tl::expected` models C++23's `std::expected` and adds some other useful
 * functions.
 *
 * @tparam T type of the expected response
 */
template <class T>
using Result = tl::expected<T, ErrorCode>;

}  // namespace bci::fiu

#endif  /* FIU_CPP_INCLUDE_BCI_FIU_COMMONTYPES_H */
