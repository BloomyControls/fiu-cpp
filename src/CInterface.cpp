/*
 * Copyright © 2026, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#include <bci/fiu/CInterface.h>

#include <bci/fiu/FiuClient.h>

#include <new>
#include <string>

using namespace bci::fiu;
using ec = bci::fiu::ErrorCode;
using fc = bci::fiu::FiuClient;

// these two are just used to avoid either doing C-style casts, function style
// casts, or a static_cast<int>(...) on error codes all the time
[[gnu::always_inline, msvc::forceinline]]
constexpr int Eint(ec e) noexcept { return static_cast<int>(e); }

template <class T>
[[gnu::always_inline, msvc::forceinline]]
constexpr int Eint(const Result<T>& res) { return Eint(res.error()); }

inline constexpr int kSuccess = Eint(ec::kSuccess);

static FiuClient& GetClient(FiuClientHandle handle) {
  return *(FiuClient*)handle;
}

template <class... Args>
static int WrapSet(ErrorCode (FiuClient::*func)(Args...) const,
                   FiuClientHandle handle, Args... args) noexcept try {
  if (!handle || !func) {
    return Eint(ec::kInvalidArgument);
  }

  return Eint((GetClient(handle).*func)(args...));
} catch (const std::bad_alloc&) {
  return Eint(ec::kAllocationFailed);
} catch (...) {
  return Eint(ec::kUnexpectedException);
}

template <class T, class... Args>
static int WrapGet(Result<T> (FiuClient::*func)(Args...) const,
                   FiuClientHandle handle, T* res, Args... args) noexcept
    try {
  if (!handle || !func || !res) {
    return Eint(ec::kInvalidArgument);
  }

  ec err = ec::kSuccess;
  if (auto result = (GetClient(handle).*func)(args...)) {
    *res = *result;
  } else {
    err = result.error();
  }

  return Eint(err);
} catch (const std::bad_alloc&) {
  return Eint(ec::kAllocationFailed);
} catch (...) {
  return Eint(ec::kUnexpectedException);
}

template <class... Args>
static int WrapGet(ErrorCode (FiuClient::*func)(Args...) const,
                   FiuClientHandle handle, Args... args) noexcept try {
  if (!handle || !func) {
    return Eint(ec::kInvalidArgument);
  }

  return Eint((GetClient(handle).*func)(args...));
} catch (const std::bad_alloc&) {
  return Eint(ec::kAllocationFailed);
} catch (...) {
  return Eint(ec::kUnexpectedException);
}

unsigned int FiuClient_Version() { return FIU_VERSION; }

const char* FiuClient_ErrorMessage(int error) {
  return bci::fiu::ErrorMessage(static_cast<bci::fiu::ErrorCode>(error));
}

int FiuClient_Init(FiuClientHandle* handle_out) try {
  if (!handle_out) {
    return Eint(ec::kInvalidArgument);
  }

  FiuClient*& client_ptr = *(FiuClient**)handle_out;
  if (!client_ptr) {
    client_ptr = new FiuClient{};
  }

  return kSuccess;
} catch (const std::bad_alloc&) {
  return Eint(ec::kAllocationFailed);
} catch (...) {
  return Eint(ec::kUnexpectedException);
}

void FiuClient_Destroy(FiuClientHandle* handle) {
  if (handle && *handle) {
    delete (FiuClient*)*handle;
    *handle = nullptr;
  }
}

int FiuClient_Open(FiuClientHandle handle, const char* port) try {
  if (!handle || !port) {
    return Eint(ec::kInvalidArgument);
  }

  return Eint(GetClient(handle).Open(port));
} catch (const std::bad_alloc&) {
  return Eint(ec::kAllocationFailed);
} catch (...) {
  return Eint(ec::kUnexpectedException);
}

int FiuClient_Close(FiuClientHandle handle) try {
  if (!handle) {
    return Eint(ec::kInvalidArgument);
  }

  GetClient(handle).Close();

  return kSuccess;
} catch (const std::bad_alloc&) {
  return Eint(ec::kAllocationFailed);
} catch (...) {
  return Eint(ec::kUnexpectedException);
}

int FiuClient_SetBoxID(FiuClientHandle handle, uint8_t box_id) {
  if (!handle) {
    return Eint(ec::kInvalidArgument);
  }

  return Eint(GetClient(handle).SetBoxID(box_id));
}

int FiuClient_GetBoxID(FiuClientHandle handle, uint8_t* box_id_out) {
  if (!handle || !box_id_out) {
    return Eint(ec::kInvalidArgument);
  }

  *box_id_out = GetClient(handle).GetBoxID();

  return kSuccess;
}

int FiuClient_SetChannelState(FiuClientHandle handle, unsigned int channel,
                              uint8_t state) {
  return WrapSet(&fc::SetChannelState, handle, channel,
                 static_cast<ChannelState>(state));
}

int FiuClient_SetAllChannelStates(FiuClientHandle handle, uint8_t state) {
  return WrapSet(&fc::SetAllChannelStates, handle,
                 static_cast<ChannelState>(state));
}

int FiuClient_GetChannelStates(FiuClientHandle handle, uint8_t states_out[],
                               unsigned int states_len) try {
  if (!handle || !states_out) {
    return Eint(ec::kInvalidArgument);
  }

  if (states_len < kChannelCount) {
    return Eint(ec::kBufferTooSmall);
  }

  auto res = GetClient(handle).GetChannelStates();
  if (!res) {
    return Eint(res);
  }

  for (auto i{0U}; i < kChannelCount; ++i) {
    // use operator[] instead of at() to avoid 24 redundant bounds checks
    states_out[i] = static_cast<uint8_t>(res->operator[](i));
  }

  return kSuccess;
} catch (const std::bad_alloc&) {
  return Eint(ec::kAllocationFailed);
} catch (...) {
  return Eint(ec::kUnexpectedException);
}

int FiuClient_GetFirmwareVersion(FiuClientHandle handle,
                                 FiuFirmwareVersion* version_out) try {
  if (!handle || !version_out) {
    return Eint(ec::kInvalidArgument);
  }

  auto res = GetClient(handle).GetFirmwareVersion();
  if (!res) {
    return Eint(res);
  }

  // really wishing for https://wg21.link/P3817 right now...
  version_out->major = res->major;
  version_out->minor = res->minor;

  return kSuccess;
} catch (const std::bad_alloc&) {
  return Eint(ec::kAllocationFailed);
} catch (...) {
  return Eint(ec::kUnexpectedException);
}

int FiuClient_SetSoftwareInterlock(FiuClientHandle handle, bool en) {
  return WrapSet(&fc::SetSoftwareInterlock, handle, en);
}

int FiuClient_GetInterlockState(FiuClientHandle handle,
                                        bool* interlock_out) {
  return WrapGet(&fc::GetInterlockState, handle, interlock_out);
}

// check for programmer errors in enum values
static_assert(FIU_CHAN_STATE_DISCONNECTED == (int)ChannelState::kDisconnected);
static_assert(FIU_CHAN_STATE_CONNECTED == (int)ChannelState::kConnected);
static_assert(FIU_CHAN_STATE_VMEAS == (int)ChannelState::kVoltageMeasurement);
static_assert(FIU_CHAN_STATE_IMEAS == (int)ChannelState::kCurrentMeasurement);
static_assert(FIU_CHAN_STATE_FAULT == (int)ChannelState::kGroundFault);
