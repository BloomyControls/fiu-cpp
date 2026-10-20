/*
 * Copyright © 2026, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#include <bci/fiu/CommonTypes.h>
#include <bci/fiu/FiuClient.h>

#include <cassert>
#include <cstdint>
#include <string_view>

#include "Message.h"
#include "SerialPort.h"
#include "Util.h"

namespace bci::fiu {

using ec = ErrorCode;
using util::Err;

unsigned int FiuClient::Version() noexcept { return FIU_VERSION; }

FiuClient::FiuClient() : box_id_{}, port_{std::make_unique<SerialPort>()} { }

FiuClient::FiuClient(FiuClient&& other) noexcept = default;

FiuClient& FiuClient::operator=(FiuClient&& rhs) noexcept = default;

FiuClient::~FiuClient() = default;

ErrorCode FiuClient::Open(const std::string& port) {
  if (!port_) {
    return ec::kInvalidObject;
  }
  return port_->Open(port);
}

void FiuClient::Close() noexcept {
  if (port_) {
    port_->Close();
  }
}

ErrorCode FiuClient::SetBoxID(std::uint8_t box_id) noexcept {
  if (box_id > 0xf) {
    return ec::kBoxIDOutOfRange;
  }

  box_id_ = box_id;

  return ec::kSuccess;
}

std::uint8_t FiuClient::GetBoxID() const noexcept { return box_id_; }

ErrorCode FiuClient::SetChannelState(unsigned int channel,
                                     ChannelState state) const {
  if (channel >= kChannelCount && channel != 99) {
    return ec::kChannelIndexOutOfRange;
  }

  if (channel == 99 && (state == ChannelState::kGroundFault ||
                        state == ChannelState::kVoltageMeasurement ||
                        state == ChannelState::kCurrentMeasurement)) {
    return ec::kInvalidArgument;
  }

  Command cmd;
  switch (state) {
    case ChannelState::kDisconnected:
      cmd = Command::kDisconnect;
      break;
    case ChannelState::kConnected:
      cmd = Command::kConnect;
      break;
    case ChannelState::kVoltageMeasurement:
      cmd = Command::kSetVoltageMode;
      break;
    case ChannelState::kCurrentMeasurement:
      cmd = Command::kSetCurrentMode;
      break;
    case ChannelState::kGroundFault:
      cmd = Command::kSetGroundFault;
      break;
    default:
      return ec::kInvalidArgument;
  }

  Message msg(box_id_, cmd);
  msg.Append("{:02}", channel == 99 ? channel : channel + 1);
  return SendAndCheck(msg.Finalize());
}

ErrorCode FiuClient::SetAllChannelStates(ChannelState state) const {
  return SetChannelState(99, state);
}

Result<std::array<ChannelState, kChannelCount>> FiuClient::GetChannelStates()
    const {
  Message msg(box_id_, Command::kGetRelayStates);

  auto resp = SendAndRecv(msg.Finalize());
  if (!resp) {
    return Err(resp.error());
  }

  if (resp->size() != kChannelCount) {
    return Err(ec::kInvalidResponse);
  }

  std::array<ChannelState, kChannelCount> result;
  for (unsigned i{0U}; i < kChannelCount; ++i) {
    switch (resp->at(i)) {
      case 'C':
        result[i] = ChannelState::kConnected;
        break;
      case 'D':
        result[i] = ChannelState::kDisconnected;
        break;
      case 'V':
        result[i] = ChannelState::kVoltageMeasurement;
        break;
      case 'I':
        result[i] = ChannelState::kCurrentMeasurement;
        break;
      case 'F':
        result[i] = ChannelState::kGroundFault;
        break;
      default:
        return Err(ec::kInvalidResponse);
    }
  }

  return result;
}

Result<FirmwareVersion> FiuClient::GetFirmwareVersion() const {
  Message msg(box_id_, Command::kGetSWVersion);

  auto resp = SendAndRecv(msg.Finalize());
  if (!resp) {
    return Err(resp.error());
  }

  std::string_view rv{*resp};

  if (rv.size() < 3) {
    return Err(ec::kInvalidResponse);
  }

  auto sep = rv.find('.');
  if (sep == std::string_view::npos) {
    return Err(ec::kInvalidResponse);
  }

  auto lhs = rv.substr(0, sep);
  auto rhs = rv.substr(sep + 1);
  if (lhs.empty() || rhs.empty()) {
    return Err(ec::kInvalidResponse);
  }

  auto maj = util::ParseInt<std::uint8_t>(lhs);
  auto min = util::ParseInt<std::uint8_t>(rhs);
  if (!maj || !min) {
    return Err(ec::kInvalidResponse);
  }

  return FirmwareVersion{.major = *maj, .minor = *min};
}

ErrorCode FiuClient::SetSoftwareInterlock(bool en) const {
  Message msg(box_id_, Command::kSetSoftwareInterlock);
  msg.Append("{:d}", en);
  return SendAndCheck(msg.Finalize());
}

Result<bool> FiuClient::GetInterlockState() const {
  Message msg(box_id_, Command::kGetInterlockState);

  auto resp = SendAndRecv(msg.Finalize());
  if (!resp) {
    return Err(resp.error());
  }

  if (resp->size() != 1) {
    return Err(ec::kInvalidResponse);
  }

  switch (resp->front()) {
    case '0':
      return false;
    case '1':
      return true;
    default:
      return Err(ec::kInvalidResponse);
  }
}

Result<std::string> FiuClient::SendAndRecv(std::string_view buf) const {
  if (!port_) {
    return Err(ec::kInvalidObject);
  }

  ec e = port_->Write(buf);
  if (e != ec::kSuccess) {
    return Err(e);
  }

  // XXX is 100ms okay?
  auto resp = port_->ReadLine(100U);
  if (!resp) {
    return resp;
  }

  if (resp->size() < 3) {
    return Err(ec::kInvalidResponse);
  }

  std::string_view rv{*resp};
  auto crc_in = util::ParseInt<std::uint8_t>(rv.substr(rv.size() - 2), 16);
  if (!crc_in) {
    return Err(ec::kInvalidResponse);
  }

  rv.remove_suffix(2);
  std::uint8_t sum{};
  for (auto c : rv) {
    sum += static_cast<std::uint8_t>(c);
  }

  if (sum != crc_in) {
    return Err(ec::kChecksumMismatch);
  }

  const int status = static_cast<int>(rv.front() - '0');
  if (status < 0 || status > 3) {
    return Err(ec::kInvalidResponse);
  }

  rv.remove_prefix(1);

  if (status == 2) {
    return Err(ec::kSyntaxError);
  }

  if (status == 3) {
    return Err(util::TranslateDeviceError(rv));
  }

  // move here so that in C++23 and beyond, we can benefit from a potentially
  // more efficient std::string::substr()&& (https://wg21.link/P2438R2)
  return std::move(resp).value().substr(1, resp->size() - 3);
}

ErrorCode FiuClient::SendAndCheck(std::string_view buf) const {
  auto res = SendAndRecv(buf);
  if (!res) {
    return res.error();
  }
  return ec::kSuccess;
}

}  // namespace bci::fiu
