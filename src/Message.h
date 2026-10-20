/*
 * Copyright © 2026, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#ifndef FIU_CPP_SRC_MESSAGE_H
#define FIU_CPP_SRC_MESSAGE_H

#include <array>
#include <cstdint>
#include <format>
#include <string_view>
#include <utility>

namespace bci::fiu {

enum class Command : char {
  kConnect = 'C',
  kDisconnect = 'D',
  kSetVoltageMode = 'V',
  kSetCurrentMode = 'I',
  kSetGroundFault = 'F',
  kGetSWVersion = 'H',
  kGetRelayStates = 'S',
  kGetInterlockState = 'L',
  kSetSoftwareInterlock = 'O',
};

class Message {
 public:
  explicit Message(std::uint8_t box_id, Command cmd) {
    Append("{}{:X}", static_cast<char>(cmd), box_id & 0x0F);
  }

  // NOTE: this does NOT do any bounds checking! It is up to the caller to
  // ensure that they do not put too much data into the message. This code is
  // not exposed in the public library API.
  template <class... Args>
  void Append(std::format_string<Args...> fmt, Args&&... args) {
    it_ = std::format_to(it_, fmt, std::forward<Args>(args)...);
  }

  // Adds the CRC and terminator, then returns a view of the whole thing.
  std::string_view Finalize() {
    std::uint8_t sum{};
    // NOLINTNEXTLINE(*-qualified-auto): not correct with all STLs
    for (auto it{buf_.begin()}; it != it_; ++it) {
      sum += static_cast<std::uint8_t>(*it);
    }
    Append("{:02X}\r", sum);
    return ToView();
  }

  std::string_view ToView() const {
    return std::string_view{buf_.begin(), it_};
  }

  std::size_t Len() const { return it_ - buf_.begin(); }

 private:
  std::array<char, 16> buf_{};
  decltype(buf_)::iterator it_{buf_.begin()};
};

}  // namespace bci::fiu

#endif  /* FIU_CPP_SRC_MESSAGE_H */
