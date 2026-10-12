/*
 * Copyright © 2026, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#ifndef FIU_CPP_SRC_SERIALPORT_H
#define FIU_CPP_SRC_SERIALPORT_H

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/streambuf.hpp>
#include <string>
#include <string_view>

#include <bci/fiu/CommonTypes.h>

namespace bci::fiu {

class SerialPort {
 public:
  SerialPort();

  ~SerialPort();

  SerialPort(const SerialPort&) = delete;
  SerialPort& operator=(const SerialPort&) = delete;
  SerialPort(SerialPort&&) = delete;
  SerialPort& operator=(SerialPort&&) = delete;

  ErrorCode Open(const std::string& port);

  void Close() noexcept;

  ErrorCode Write(std::string_view data);

  Result<std::string> ReadLine(unsigned int timeout_ms);

 private:
  boost::asio::io_context io_context_;
  boost::asio::serial_port port_;
  boost::asio::streambuf input_buffer_;

  bool Run(unsigned int timeout_ms);
};

}  // namespace bci::fiu

#endif  /* FIU_CPP_SRC_SERIALPORT_H */
