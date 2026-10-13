/*
 * Copyright © 2026, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#include "SerialPort.h"

#include <boost/asio.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <chrono>
#include <istream>
#include <string>
#include <string_view>

#include "Util.h"

namespace bci::fiu {

using util::Err;

SerialPort::SerialPort() : port_{io_context_} { }

SerialPort::~SerialPort() { Close(); }

ErrorCode SerialPort::Open(const std::string& port) {
  boost::system::error_code ec{};

  if (port_.is_open()) {
    return ErrorCode::kAlreadyConnected;
  }

  port_.open(port, ec);
  if (ec) {
    return ErrorCode::kFailedToOpenPort;
  }

  using boost::asio::serial_port;

  port_.set_option(serial_port::baud_rate(115200), ec);
  if (ec) {
    return ErrorCode::kFailedToConfigurePort;
  }

  port_.set_option(serial_port::character_size(8), ec);
  if (ec) {
    return ErrorCode::kFailedToConfigurePort;
  }

  port_.set_option(serial_port::parity(serial_port::parity::type::none), ec);
  if (ec) {
    return ErrorCode::kFailedToConfigurePort;
  }

  port_.set_option(serial_port::stop_bits(serial_port::stop_bits::type::one),
                   ec);
  if (ec) {
    return ErrorCode::kFailedToConfigurePort;
  }

  port_.set_option(
      serial_port::flow_control(serial_port::flow_control::type::none), ec);
  if (ec) {
    return ErrorCode::kFailedToConfigurePort;
  }

  return ErrorCode::kSuccess;
}

void SerialPort::Close() noexcept {
  boost::system::error_code ignored;
  port_.close(ignored);
}

ErrorCode SerialPort::Write(std::string_view data) {
  if (!port_.is_open()) {
    return ErrorCode::kNotConnected;
  }

  boost::system::error_code ec{};

  boost::asio::write(port_, boost::asio::buffer(data), ec);
  if (ec) {
    return ErrorCode::kSendFailed;
  }

  return ErrorCode::kSuccess;
}

Result<std::string> SerialPort::ReadLine(unsigned int timeout_ms) {
  if (!port_.is_open()) {
    return Err(ErrorCode::kNotConnected);
  }

  boost::system::error_code ec;

  const auto read_handler = [&](auto&& e, auto&&) { ec = e; };
  boost::asio::async_read_until(port_, input_buffer_, '\r', read_handler);

  if (!Run(timeout_ms)) {
    return Err(ErrorCode::kReadTimedOut);
  }

  if (ec) {
    return Err(ErrorCode::kReadFailed);
  }

  std::string line;
  std::istream is(&input_buffer_);
  std::getline(is, line, '\r');

  return line;
}

bool SerialPort::Run(unsigned int timeout_ms) {
  io_context_.restart();

  io_context_.run_for(std::chrono::milliseconds(timeout_ms));

  if (!io_context_.stopped()) {
    boost::system::error_code ignored;
    port_.cancel(ignored);
    io_context_.run();
    return false;
  }

  return true;
}

}  // namespace bci::fiu
