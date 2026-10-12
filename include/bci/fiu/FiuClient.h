/*
 * Copyright © 2026, Bloomy Controls, Inc.. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

/**
 * @file
 * @brief Declares FIU client functionality.
 */
#ifndef FIU_CPP_INCLUDE_BCI_FIU_FIUCLIENT_H
#define FIU_CPP_INCLUDE_BCI_FIU_FIUCLIENT_H

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#include "CommonTypes.h"
#include "fiu_export.h"

// These comments ensure doxygen correctly generates docs.
/**
 * @namespace bci
 * @brief Bloomy Controls root namespace.
 */
/**
 * @namespace bci::fiu
 * @brief Contains FIU-related code.
 */

namespace bci::fiu {

class SerialPort;

/**
 * @brief Serial client for communicating with the Bloomy Controls Fault
 * Insertion Unit (FIU).
 */
class FiuClient {
 public:
  /**
   * @brief Get the library version as an integer. For example, version 1.3.2
   * would return 10302.
   *
   * @return Library version.
   */
  FIU_API static unsigned int Version() noexcept;

  /// Default constructor.
  FIU_API FiuClient();

  /// Move constructor.
  FIU_API FiuClient(FiuClient&& other) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs client to move from
   *
   * @return Reference to self.
   */
  FIU_API FiuClient& operator=(FiuClient&& rhs) noexcept;

  /// Destructor.
  FIU_API ~FiuClient();

  /**
   * @brief Open a serial port to connect to the FIU.
   *
   * @param[in] port serial port to open (e.g., COM4)
   *
   * @return An error code.
   */
  FIU_API ErrorCode Open(const std::string& port);

  /// Close the serial port.
  FIU_API void Close() noexcept;

  /**
   * @brief Set the target box ID. This can be done at runtime to quickly target
   * another device.
   *
   * @param[in] box_id new target box ID, 0-15
   *
   * @return An error code.
   */
  FIU_API ErrorCode SetBoxID(std::uint8_t box_id) noexcept;

  /**
   * @brief Get the target box ID.
   *
   * @return Target box ID.
   */
  FIU_API std::uint8_t GetBoxID() const noexcept;

  /**
   * @brief Set the state of a channel.
   *
   * @param[in] channel target channel, 0-23
   * @param[in] state desired channel state
   *
   * @return An error code.
   */
  FIU_API ErrorCode SetChannelState(unsigned int channel,
                                    ChannelState state) const;

  /**
   * @brief Set the state of all channels.
   *
   * This function can only be used with the connected and disconnected states,
   * as the others require the use of shared resources.
   *
   * @param[in] state desired channel state
   *
   * @return An error code.
   */
  FIU_API ErrorCode SetAllChannelStates(ChannelState state) const;

  /**
   * @brief Get the states of all 24 channels.
   *
   * @return Result containing the relay states or an error code.
   */
  FIU_API Result<std::array<ChannelState, kChannelCount>> GetChannelStates()
      const;

  /**
   * @brief Get the firmware version loaded onto the FIU.
   *
   * @return Result containing the firmware version or an error code.
   */
  FIU_API Result<FirmwareVersion> GetFirmwareVersion() const;

  /**
   * @brief Set the state of the software interlock.
   *
   * @param[in] en whether to enable the software interlock
   *
   * @return An error code.
   */
  FIU_API ErrorCode SetSoftwareInterlock(bool en) const;

  /**
   * @brief Get the interlock state.
   *
   * @return Result containing the interlock state or an error code.
   */
  FIU_API Result<bool> GetInterlockState() const;

 private:
  /// FIU box ID.
  std::uint8_t box_id_;
  /// Serial port.
  std::unique_ptr<SerialPort> port_;

  /**
   * @brief Send a message to the FIU and wait for a response. This function
   * should be used for commands which return 1.
   *
   * @param[in] buf message to send
   *
   * @return Result containing the response or an error code.
   */
  FIU_API Result<std::string> SendAndRecv(std::string_view buf) const;

  /**
   * @brief Send a message to the FIU and wait for a response. This function
   * should be used for commands which return 0, 2, or 3.
   *
   * @param[in] buf message to send
   *
   * @return An error code.
   */
  FIU_API ErrorCode SendAndCheck(std::string_view buf) const;
};

}  // namespace bci::fiu

#endif /* FIU_CPP_INCLUDE_BCI_FIU_FIUCLIENT_H */
