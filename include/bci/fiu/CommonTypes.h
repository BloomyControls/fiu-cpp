/*
 * Copyright © 2026, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

/**
 * @file
 * @brief Defines common types for used with FIU client.
 */
#ifndef FIU_CPP_INCLUDE_BCI_FIU_COMMONTYPES_H
#define FIU_CPP_INCLUDE_BCI_FIU_COMMONTYPES_H

#include <cstdint>

#include "fiu_export.h"
#include "util/expected.hpp"

namespace bci::fiu {

/// FIU channel count.
inline constexpr unsigned int kChannelCount{24U};

/// FIU channel states.
enum class ChannelState : std::uint8_t {
  // Values must match the C macros!
  kDisconnected = 0,        ///< Disconnected
  kConnected = 1,           ///< Connected
  kVoltageMeasurement = 2,  ///< Voltage measurement mode
  kCurrentMeasurement = 3,  ///< Current measurement mode
  kGroundFault = 4,         ///< Ground fault mode
};

/// Error codes returned by the driver.
enum class ErrorCode : int {
  // Values must match the C macros!
  kSuccess = 0,                  ///< Success (no error)
  kBoxIDOutOfRange = -1,         ///< Box ID out of range
  kChannelIndexOutOfRange = -2,  ///< Channel index out of range
  kInvalidArgument = -3,         ///< Invalid argument
  kInvalidResponse = -4,         ///< Invalid response received from FIU
  kAlreadyConnected = -5,        ///< Already connected
  kFailedToOpenPort = -6,        ///< Failed to open serial port
  kFailedToConfigurePort = -7,   ///< Failed to configure serial port
  kNotConnected = -8,            ///< Not connected
  kSendFailed = -9,              ///< Failed to send data through serial port
  kReadTimedOut = -10,           ///< Read timed out
  kReadFailed = -11,             ///< Read failed (serial port error)
  kChecksumMismatch = -12,       ///< Checksum mismatch on received message
  kSyntaxError = -13,            ///< Syntax error (from device)
  kSystemInterlocked = -14,      ///< System interlocked (from device)
  kResourceConflict = -15,       ///< Resource conflict (from device)
  kUnknownDeviceError = -16,     ///< Unknown error (from device)
  kInvalidObject = -17,          ///< Invalid client object
  kBufferTooSmall = -18,         ///< Buffer too small
  kAllocationFailed = -19,       ///< Allocation failed (C only)
  kUnexpectedException = -20,    ///< Unexpected exception (C only)
};

/// FIU firmware version.
struct FirmwareVersion {
  uint8_t major;  ///< Major version
  uint8_t minor;  ///< Minor version
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

/**
 * @brief Get a string describing an error code.
 *
 * @param[in] ec error code
 *
 * @return Error message. This function never returns a null pointer.
 */
FIU_API const char* ErrorMessage(ErrorCode ec) noexcept;

}  // namespace bci::fiu

#endif /* FIU_CPP_INCLUDE_BCI_FIU_COMMONTYPES_H */
