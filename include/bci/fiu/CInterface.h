/*
 * Copyright © 2026, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

/**
 * @file
 * @brief C-style interfaced to the library.
 */
#ifndef FIU_CPP_INCLUDE_BCI_FIU_CINTERFACE_H
#define FIU_CPP_INCLUDE_BCI_FIU_CINTERFACE_H

#include <stdbool.h>
#include <stdint.h>

#include "fiu_export.h"

/**
 * @defgroup CInterface C Interface
 *
 * This interface provides a wrapper around the C++ library for use in C and
 * other languages.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @defgroup CErrors Error Codes
 * Error codes returned by the FIU client functions.
 * @{
 */
/// Success (no error)
#define FIU_SUCCESS (0)
/// Box ID out of range
#define FIU_ERR_BOX_ID_OUT_OF_RANGE (-1)
/// Channel index out of range
#define FIU_ERR_CHANNEL_INDEX_OUT_OF_RANGE (-2)
/// Invalid argument
#define FIU_ERR_INVALID_ARGUMENT (-3)
/// Invalid response received from FIU
#define FIU_ERR_INVALID_RESPONSE (-4)
/// Already connected
#define FIU_ERR_ALREADY_CONNECTED (-5)
/// Failed to open serial port
#define FIU_ERR_FAILED_TO_OPEN_PORT (-6)
/// Failed to configure serial port
#define FIU_ERR_FAILED_TO_CONFIGURE_PORT (-7)
/// Not connected
#define FIU_ERR_NOT_CONNECTED (-8)
/// Failed to send data through serial port
#define FIU_ERR_SEND_FAILED (-9)
/// Read timed out
#define FIU_ERR_READ_TIMED_OUT (-10)
/// Read failed (serial port error)
#define FIU_ERR_READ_FAILED (-11)
/// Checksum mismatch on received message
#define FIU_ERR_CHECKSUM_MISMATCH (-12)
/// Syntax error (from device)
#define FIU_ERR_SYNTAX_ERROR (-13)
/// System interlocked (from device)
#define FIU_ERR_SYSTEM_INTERLOCKED (-14)
/// Resource conflict (from device)
#define FIU_ERR_RESOURCE_CONFLICT (-15)
/// Unknown error (from device)
#define FIU_ERR_UNKNOWN_DEVICE_ERROR (-16)
/// Invalid client object
#define FIU_ERR_INVALID_OBJECT (-17)
/// Buffer too small
#define FIU_ERR_BUFFER_TOO_SMALL (-18)
/// Allocation failed (C only)
#define FIU_ERR_ALLOCATION_FAILED (-19)
/// Unexpected exception (C only)
#define FIU_ERR_UNEXPECTED_EXCEPTION (-20)
/** @} */

/**
 * @defgroup CChannelStates Channel States
 * FIU channel states.
 * @{
 */
/// Channel disconnected
#define FIU_CHAN_STATE_DISCONNECTED (0)
/// Channel connected
#define FIU_CHAN_STATE_CONNECTED (1)
/// Voltage measurement mode
#define FIU_CHAN_STATE_VMEAS (2)
/// Current measurement mode
#define FIU_CHAN_STATE_IMEAS (3)
/// Ground fault mode
#define FIU_CHAN_STATE_FAULT (4)
/** @} */

/**
 * @defgroup CInterlockTypes Interlock Types
 * FIU interlock types.
 *
 * @note Legacy FIU only supports hardware interlocks. Software interlocks are
 * only supported by FIU1KV.
 * @{
 */
/// No interlock
#define FIU_INTERLOCK_TYPE_NONE (0)
/// Hardware interlock
#define FIU_INTERLOCK_TYPE_HARDWARE (1)
/// Software interlock
#define FIU_INTERLOCK_TYPE_SOFTWARE (2)
/// Both hardware and software interlocks
#define FIU_INTERLOCK_TYPE_BOTH (3)
/** @} */

/// FIU firmware version.
typedef struct FiuFirmwareVersion {
  uint8_t major;  ///< Minor version
  uint8_t minor;  ///< Major version
} FiuFirmwareVersion;

/// FIU client handle
typedef void* FiuClientHandle;

/**
 * @brief Get the version of the library as an unsigned base 10 integer. For
 * example, version 1.2.3 would return 10203.
 *
 * This is intended to be used to check for the existence of certain
 * functionality based on library version, particularly in wrappers (such as
 * Python).
 *
 * @return The library version.
 */
FIU_API unsigned int FiuClient_Version();

/**
 * @brief Get a message describing an error code returned by the library.
 *
 * @note This function never returns a null pointer.
 *
 * @param[in] error error code
 *
 * @return A null-terminated error message.
 */
FIU_API const char* FiuClient_ErrorMessage(int error);

/**
 * @brief Initialize an FIU client.
 *
 * @note The caller must destroy the client with FiuClient_Destroy() when it is
 * no longer needed.
 *
 * @param[out] handle_out pointer to a handle to initialize (handle should be
 * zeroed)
 *
 * @return 0 on success or a negative error code.
 */
FIU_API int FiuClient_Init(FiuClientHandle* handle_out);

/**
 * @brief Destroy an FIU client.
 *
 * @param[in,out] handle pointer to a handle to destroy
 */
FIU_API void FiuClient_Destroy(FiuClientHandle* handle);

/**
 * @brief Open a serial connection to the FIU.
 *
 * @param[in] handle FIU client
 * @param[in] port serial port to open (e.g., COM4)
 *
 * @return 0 on success or a negative error code.
 */
FIU_API int FiuClient_Open(FiuClientHandle handle, const char* port);

/**
 * @brief Close the serial port.
 *
 * @note This does not destroy the client.
 *
 * @param[in] handle FIU client
 *
 * @return 0 on success or a negative error code.
 */
FIU_API int FiuClient_Close(FiuClientHandle handle);

/**
 * @brief Set the target box ID.
 *
 * This can be changed on the fly to target multiple devices on the same bus.
 *
 * @param[in] handle FIU client
 * @param[in] box_id new target box ID, 0-15
 *
 * @return 0 on success or a negative error code.
 */
FIU_API int FiuClient_SetBoxID(FiuClientHandle handle, uint8_t box_id);

/**
 * @brief Get the target box ID.
 *
 * @param[in] handle FIU client
 * @param[out] box_id_out address of an integer to store the box ID
 *
 * @return 0 on success or a negative error code.
 */
FIU_API int FiuClient_GetBoxID(FiuClientHandle handle, uint8_t* box_id_out);

/**
 * @brief Set the state of a channel.
 *
 * @param[in] handle FIU client
 * @param[in] channel target channel, 0-23
 * @param[in] state desired channel state (see @ref CChannelStates)
 *
 * @return 0 on success or a negative error code.
 */
FIU_API int FiuClient_SetChannelState(FiuClientHandle handle,
                                      unsigned int channel,
                                      uint8_t state);

/**
 * @brief Set the state of all channels.
 *
 * This function can only be used with the connected and disconnected states, as
 * the others require the use of shared resources.
 *
 * @param[in] handle FIU client
 * @param[in] state desired channel state (see @ref CChannelStates)
 *
 * @return 0 on success or a negative error code.
 */
FIU_API int FiuClient_SetAllChannelStates(FiuClientHandle handle,
                                          uint8_t state);

/**
 * @brief Get the states of all 24 channels.
 *
 * @param[in] handle FIU client
 * @param[out] states_out array to store states in
 * @param[in] states_len length of @a states_out
 *
 * @return 0 on success or a negative error code.
 */
FIU_API int FiuClient_GetChannelStates(FiuClientHandle handle,
                                       uint8_t states_out[],
                                       unsigned int states_len);

/**
 * @brief Get the firmware version loaded onto the FIU.
 *
 * @param[in] handle FIU client
 * @param[out] version_out address of a firmware version structure to store the
 * result
 *
 * @return 0 on success or a negative error code.
 */
FIU_API int FiuClient_GetFirmwareVersion(FiuClientHandle handle,
                                         FiuFirmwareVersion* version_out);

/**
 * @brief Set the state of the software interlock.
 *
 * @param[in] handle FIU client
 * @param[in] en whether to enable the software interlock
 *
 * @return 0 on success or a negative error code.
 */
FIU_API int FiuClient_SetSoftwareInterlock(FiuClientHandle handle, bool en);

/**
 * @brief Get the system interlock state.
 *
 * @param[in] handle FIU client
 * @param[out] interlock_out address of an integer to store the interlock type
 * (see @ref CInterlockTypes)
 *
 * @return 0 on success or a negative error code.
 */
FIU_API int FiuClient_GetInterlockState(FiuClientHandle handle,
                                        uint8_t* interlock_out);

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

// close CInterface group
/** @} */

#endif  /* FIU_CPP_INCLUDE_BCI_FIU_CINTERFACE_H */
