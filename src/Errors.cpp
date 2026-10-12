/*
 * Copyright © 2026, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#include <bci/fiu/CommonTypes.h>

namespace bci::fiu {

const char* ErrorMessage(ErrorCode ec) noexcept {
  switch (ec) {
    case ErrorCode::kSuccess:
      return "Success";
    case ErrorCode::kBoxIDOutOfRange:
      return "Box ID out of range";
    case ErrorCode::kChannelIndexOutOfRange:
      return "Channel index out of range";
    case ErrorCode::kInvalidArgument:
      return "Invalid argument";
    case ErrorCode::kInvalidResponse:
      return "Invalid response";
    case ErrorCode::kAlreadyConnected:
      return "Already connected";
    case ErrorCode::kFailedToOpenPort:
      return "Failed to open serial port";
    case ErrorCode::kFailedToConfigurePort:
      return "Failed to configure serial port";
    case ErrorCode::kNotConnected:
      return "Client not connected";
    case ErrorCode::kSendFailed:
      return "Failed to send data (I/O error)";
    case ErrorCode::kReadTimedOut:
      return "Read timed out";
    case ErrorCode::kReadFailed:
      return "Read failed (I/O error)";
    case ErrorCode::kChecksumMismatch:
      return "Checksum mismatch on received message";
    case ErrorCode::kSyntaxError:
      return "Device error: syntax error";
    case ErrorCode::kSystemInterlocked:
      return "Device error: system interlocked";
    case ErrorCode::kResourceConflict:
      return "Device error: resource conflict";
    case ErrorCode::kUnknownDeviceError:
      return "Unknown device error";
    case ErrorCode::kInvalidObject:
      return "Invalid client object (bug)";
    case ErrorCode::kBufferTooSmall:
      return "Buffer too small";
    case ErrorCode::kAllocationFailed:
      return "Allocation failed";
    case ErrorCode::kUnexpectedException:
      return "Unexpected exception";
  }

  return "Unknown error";
}

}  // namespace bci::fiu
