/**
 * ezsp_defs.h
 *
 *  EZSP Data structure definition
 *
 *  Created on: Apr 27, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_EZSP_DEFS_H_
#define PI_ZIGBEE_LIB_EZSP_DEFS_H_

#include <cstdint>

namespace zb_ezsp {

/**
 * There is a new EZSP frame format in Release 6.7. Frame IDs have been expanded from 1 byte (8 bits) to 2 bytes (16 bits). Frame ID
 *   values are now shown with 4 hex digits to emphasize the larger Frame ID numbering space.
 */
//#define VER_8 1
#ifdef VER_8
using id_type = uint16_t;
#else
using id_type = uint8_t;
#endif

class EzspVersion {
public:
    EzspVersion() {}
    ~EzspVersion() {}

    static uint8_t _ver;
    static uint8_t ver(){
        return _ver;
    }

};

using EZSP_Bool = uint8_t;
using EZSP_EzspConfigId = uint8_t;


using EId = enum EFrame_ID : id_type {
    ID_version = 0x00,
    ID_callback = 0x06,                     //Allows the NCP to respond with a pending callback.
    ID_noCallbacks = 0x07,                  //Indicates that there are currently no pending callbacks.
    ID_startScan = 0x1A,
    ID_networkFoundHandler = 0x1B,
    ID_energyScanResultHandler = 0x48,
    ID_invalidCommand = 0x58,
    ID_Echo = 0x81
};

using EzspNetworkScanType = enum EFrame_EzspNetworkScanType : uint8_t {
    EZSP_ENERGY_SCAN = 0x00,    //An energy scan scans each channel for its RSSI value.
    EZSP_ACTIVE_SCAN = 0x01     //An active scan scans each channel for available networks.
};

/**
 * List of Ember Status
 */
using EmberStatus = enum EFrame_EmberStatus : uint8_t{
EMBER_SUCCESS = 0x00,                       //The generic 'no error' message.
EMBER_ERR_FATAL = 0x01,                     // The generic 'fatal error' message.
EMBER_BAD_ARGUMENT = 0x02,                  // An invalid value was passed as an argument to a function
EMBER_EEPROM_MFG_STACK_VERSION_MISMATCH = 0x04,         // The manufacturing and stack token format in non-volatile memory is different than what the stack expects (returned at initialization).
EMBER_INCOMPATIBLE_STATIC_MEMORY_DEFINITIONS = 0x05,    // The static memory definitions in ember-static-memory.h are incompatible with this stack version.
EMBER_EEPROM_MFG_VERSION_MISMATCH = 0x06,   // The manufacturing token format in non-volatile memory is different than what the stack expects (returned at initialization).
EMBER_EEPROM_STACK_VERSION_MISMATCH = 0x07, // The stack token format in non-volatile memory is different than what the stack expects (returned at initialization).
EMBER_NO_BUFFERS = 0x18,                    // There are no more buffers.
EMBER_SERIAL_INVALID_BAUD_RATE = 0x20,      // Specified an invalid baud rate.
EMBER_SERIAL_INVALID_PORT = 0x21,           // Specified an invalid serial port.
EMBER_SERIAL_TX_OVERFLOW = 0x22,            // Tried to send too much data.
EMBER_SERIAL_RX_OVERFLOW = 0x23,            // There was not enough space to store a received character and the character was dropped.
EMBER_SERIAL_RX_FRAME_ERROR = 0x24,         // Detected a UART framing error.
EMBER_SERIAL_RX_PARITY_ERROR = 0x25,        // Detected a UART parity error.
EMBER_SERIAL_RX_EMPTY = 0x26,               // There is no received data to process.
EMBER_SERIAL_RX_OVERRUN_ERROR = 0x27,       // The receive interrupt was not handled in time, and a character was dropped.
EMBER_MAC_TRANSMIT_QUEUE_FULL = 0x39,       // The MAC transmit queue is full.
EMBER_MAC_UNKNOWN_HEADER_TYPE = 0x3A,       //  MAC header FCR error on receive.
EMBER_MAC_SCANNING = 0x3D,                  // The MAC can't complete this task because it is scanning.
EMBER_MAC_NO_DATA = 0x31,                   // No pending data exists for device doing a data poll.
EMBER_MAC_JOINED_NETWORK = 0x32,            // Attempt to scan when we are joined to a network.
EMBER_MAC_BAD_SCAN_DURATION = 0x33,         // Scan duration must be 0 to 14 inclusive. Attempt was made to scan with an incorrect duration value.
EMBER_MAC_INCORRECT_SCAN_TYPE = 0x34,       // emberStartScan was czb_ezspalled with an incorrect scan type.
EMBER_MAC_INVALID_CHANNEL_MASK = 0x35,      // emberStartScan was called with an invalid channel mask.
EMBER_MAC_COMMAND_TRANSMIT_FAILURE = 0x36,  // Failed to scan current channel because we were unable to transmit the relevant MAC command.
EMBER_MAC_NO_ACK_RECEIVED  = 0x40,          // We expected to receive an ACK following the transmission, but the MAC level ACK was never received.
EMBER_MAC_INDIRECT_TIMEOUT = 0x42,          // Indirect data message timed out before polled.

EMBER_SIM_EEPROM_ERASE_PAGE_GREEN = 0x43,   // The Simulated EEPROM is telling the application that there is at least one flash page to be erased.
                                            //The GREEN status means the current page has not filled above the ERASE_CRITICAL_THRESHOLD.
                                            //The application should call the function halSimEepromErasePage when it can to erase a page.

EMBER_SIM_EEPROM_ERASE_PAGE_RED = 0x44,     // The Simulated EEPROM is telling the application that there is at least one flash page to be erased.
                                            //The RED status means the current page has filled above the ERASE_CRITICAL_THRESHOLD. Due to the shrinking availability of write space, there is a
                                            //danger of data loss. The application must call the function halSimEepromErasePage as soon as possible to erase a page.

EMBER_SIM_EEPROM_FULL = 0x45,               // The Simulated EEPROM has run out of room to write any new data and the data trying to be set has been
                                            //lost. This error code is the result of ignoring the SIM_EEPROM_ERASE_PAGE_RED error code. The application must call the function
                                            //halSimEepromErasePage to make room for any further calls to set a token.

EMBER_ERR_FLASH_WRITE_INHIBITED = 0x46,     // A fatal error has occurred while trying to write data to the Flash. The target memory attempting to be
                                            //programmed is already programmed. The flash write routines were asked to flip a bit from a 0 to 1, which
                                            //is physically impossible and the write was therefore inhibited. The data in the flash cannot be trusted after this error.

EMBER_ERR_FLASH_VERIFY_FAILED = 0x47,       // A fatal error has occurred while trying to write data to the Flash and the write verification has failed. The
                                            //data in the flash cannot be trusted after this error, and it is possible this error is the result of exceeding the life cycles of the flash.

EMBER_SIM_EEPROM_INIT_1_FAILED = 0x48,      // Attempt 1 to initialize the Simulated EEPROM has failed. This failure means the information already
                                            //stored in Flash (or a lack thereof), is fatally incompatible with the token information compiled into the code image being run.

EMBER_SIM_EEPROM_INIT_2_FAILED = 0x49,      // Attempt 2 to initialize the Simulated EEPROM has failed. This failure means Attempt 1 failed, and the
                                            //token system failed to properly reload default tokens and reset the Simulated EEPROM.

EMBER_SIM_EEPROM_INIT_3_FAILED = 0x4A,      // Attempt 3 to initialize the Simulated EEPROM has failed. This failure means one or both of the tokens
                                            //TOKEN_MFG_NVDATA_VERSION or TOKEN_STACK_NVDATA_VERSION were incorrect and the token system failed to properly reload default tokens and reset the Simulated EEPROM.

EMBER_ERR_FLASH_PROG_FAIL = 0x4B,           // A fatal error has occurred while trying to write data to the flash, possibly due to write protection or an invalid address. The data in the flash cannot be
                                            //trusted after this error, and it is possible this error is the result of exceeding the life cycles of the flash.

EMBER_ERR_FLASH_ERASE_FAIL = 0x4C,          // A fatal error has occurred while trying to erase flash, possibly due to write protection. The data in the flash cannot be trusted after this error, and it is possible
                                            //this error is the result of exceeding the life cycles of the flash.

EMBER_ERR_BOOTLOADER_TRAP_TABLE_BAD = 0x58, // The bootloader received an invalid message (failed attempt to go into bootloader).
EMBER_ERR_BOOTLOADER_TRAP_UNKNOWN = 0x59,   // Bootloader received an invalid message (failed attempt to go into bootloader).
EMBER_ERR_BOOTLOADER_NO_IMAGE = 0x5A,       // The bootloader cannot complete the bootload operation because either an image was not found or the image exceeded memory bounds.
EMBER_DELIVERY_FAILED = 0x66,               // The APS layer attempted to send or deliver a message, but it failed.
EMBER_BINDING_INDEX_OUT_OF_RANGE = 0x69,    // This binding index is out of range of the current binding table.
EMBER_ADDRESS_TABLE_INDEX_OUT_OF_RANGE = 0x6A,  // This address table index is out of range for the current address table.
EMBER_INVALID_BINDING_INDEX = 0x6C,         // An invalid binding table index was given to a function.
EMBER_INVALID_CALL = 0x70,                  //The API call is not allowed given the current state of the stack.
EMBER_COST_NOT_KNOWN = 0x71,                // The link cost to a node is not known.
EMBER_MAX_MESSAGE_LIMIT_REACHED = 0x72,     // The maximum number of in-flight messages (i.e.EMBER_APS_UNICAST_MESSAGE_COUNT) hasbeen reached.
EMBER_MESSAGE_TOO_LONG = 0x74,              // The message to be transmitted is too big to fit into a single over-the-air packet.
EMBER_BINDING_IS_ACTIVE = 0x75,             // The application is trying to delete or overwrite a binding that is in use.
EMBER_ADDRESS_TABLE_ENTRY_IS_ACTIVE = 0x76, // The application is trying to overwrite an address table entry that is in use.
EMBER_ADC_CONVERSION_DONE = 0x80,           // Conversion is complete.
EMBER_ADC_CONVERSION_BUSY = 0x81,           // Conversion cannot be done because a request is being processed.
EMBER_ADC_CONVERSION_DEFERRED = 0x82,       // Conversion is deferred until the current request has been processed.
EMBER_ADC_NO_CONVERSION_PENDING = 0x84,     // No results are pending.
EMBER_SLEEP_INTERRUPTED = 0x85,             // Sleeping (for a duration) has been abnormally interrupted and exited prematurely.
EMBER_PHY_TX_UNDERFLOW = 0x88,              // The transmit hardware buffer underflowed.
EMBER_PHY_TX_INCOMPLETE = 0x89,             // The transmit hardware did not finish transmitting a packet.
EMBER_PHY_INVALID_CHANNEL = 0x8A,           // An unsupported channel setting was specified.
EMBER_PHY_INVALID_POWER = 0x8B,             // An unsupported power setting was specified.
EMBER_PHY_TX_BUSY = 0x8C,                   // The packet cannot be transmitted because the physical MAC layer is currently transmitting a packet. (This is used for the MAC backoff algorithm.)
EMBER_PHY_TX_CCA_FAIL = 0x8D,               // The transmit attempt failed because all CCA attempts indicated that the channel was busy
EMBER_PHY_OSCILLATOR_CHECK_FAILED = 0x8E,   // The software installed on the hardware doesn't recognize the hardware radio type.
EMBER_PHY_ACK_RECEIVED = 0x8F,              // The expected ACK was received after the last transmission.
EMBER_NETWORK_UP = 0x90,                    // The stack software has completed initialization and is ready to send and receive packets over the air.
EMBER_NETWORK_DOWN = 0x91,                  // The network is not operating.
EMBER_JOIN_FAILED = 0x94,                   // An attempt to join a network failed.
EMBER_MOVE_FAILED = 0x96,                   // After moving, a mobile node's attempt to re-establish contact with the network failed.

EMBER_CANNOT_JOIN_AS_ROUTER = 0x98,         // An attempt to join as a router failed due to a ZigBee versus ZigBee Pro incompatibility. ZigBee devices
                                            //joining ZigBee Pro networks (or vice versa) must join as End Devices, not Routers.

EMBER_NODE_ID_CHANGED = 0x99,               // The local node ID has changed. The application can obtain the new node ID by calling emberGetNodeId().
EMBER_PAN_ID_CHANGED = 0x9A,                // The local PAN ID has changed. The application can obtain the new PAN ID by calling emberGetPanId().
EMBER_NO_BEACONS = 0xAB,                    // An attempt to join or rejoin the network failed because no router beacons could be heard by the joining node.
EMBER_RECEIVED_KEY_IN_THE_CLEAR = 0xAC,     // An attempt was made to join a Secured Network using a pre-configured key, but the Trust Center sent
                                            //back a Network Key in-the-clear when an encrypted Network Key was required.
EMBER_NO_NETWORK_KEY_RECEIVED = 0xAD,       // An attempt was made to join a Secured Network, but the device did not receive a Network Key.
EMBER_NO_LINK_KEY_RECEIVED = 0xAE,          // After a device joined a Secured Network, a Link Key was requested but no response was ever received.
EMBER_PRECONFIGURED_KEY_REQUIRED = 0xAF,    // An attempt was made to join a Secured Network without a pre-configured key, but the Trust Center sent encrypted data using a pre-configured key.
EMBER_NOT_JOINED = 0x93,                    // The node has not joined a network.
EMBER_INVALID_SECURITY_LEVEL = 0x95,        // The chosen security level (the value of EMBER_SECURITY_LEVEL) is not supported by the stack.
EMBER_NETWORK_BUSY = 0xA1,                  // A message cannot be sent because the network is currently overloaded.
EMBER_INVALID_ENDPOINT = 0xA3,              // The application tried to send a message using an endpoint that it has not defined.
EMBER_BINDING_HAS_CHANGED = 0xA4,           // The application tried to use a binding that has been remotely modified and the change has not yet been reported to the application.
EMBER_INSUFFICIENT_RANDOM_DATA = 0xA5,      // An attempt to generate random bytes failed because of insufficient random data from the radio.
EMBER_APS_ENCRYPTION_ERROR = 0xA6,          // There was an error in trying to encrypt at the APS Level. This could result from either an inability to determine the long address of the recipient from the
                                            //short address (no entry in the binding table) or there is no link key entry in the table associated with the destination,
                                            //or there was a failure to load the correct key into the encryption core.
EMBER_SECURITY_STATE_NOT_SET = 0xA8,        // There was an attempt to form or join a network with security without calling emberSetInitialSecurityState() first.
EMBER_KEY_TABLE_INVALID_ADDRESS = 0xB3,     // There was an attempt to set an entry in the key table using an invalid long address. An entry cannot be set using either the local device's or Trust Center's IEEE
                                            //address. Or an entry already exists in the table with the same IEEE address. An Address of all zeros or all F's are not valid addresses in 802.15.4.
EMBER_SECURITY_CONFIGURATION_INVALID = 0xB7,    // There was an attempt to set a security configuration that is not valid given the other security settings.
EMBER_TOO_SOON_FOR_SWITCH_KEY = 0xB8,       // There was an attempt to broadcast a key switch too quickly after broadcasting the next network key. The Trust Center must wait at least a period equal to the
                                            //broadcast timeout so that all routers have a chance to receive the broadcast of the new network key.
EMBER_KEY_NOT_AUTHORIZED = 0xBB,            // The message could not be sent because the link key corresponding to the destination is not authorized for use in APS data messages. APS Commands (sent
                                            //by the stack) are allowed. To use it for encryption of APS data messages it must be authorized using a key agreement protocol (such as CBKE).
EMBER_SECURITY_DATA_INVALID = 0xBD,         // The security data provided was not valid, or an integrity check failed.
EMBER_SOURCE_ROUTE_FAILURE = 0xA9,          // A ZigBee route error command frame was received indicating that a source routed message from this node failed en route.
EMBER_MANY_TO_ONE_ROUTE_FAILURE = 0xAA,     // A ZigBee route error command frame was received indicating that a message sent to this node along a
                                            //many-to-one route failed en route. The route error frame was delivered by an ad-hoc search for a functioning route.
EMBER_STACK_AND_HARDWARE_MISMATCH = 0xB0,   // A critical and fatal error indicating that the version of the stack trying to run does not match with the chip it
                                            //is running on. The software (stack) on the chip must be replaced with software that is compatible with the chip.

EMBER_INDEX_OUT_OF_RANGE = 0xB1,            // An index was passed into the function that was larger than the valid range.
EMBER_TABLE_FULL = 0xB4,                    // There are no empty entries left in the table.
EMBER_TABLE_ENTRY_ERASED = 0xB6,            // The requested table entry has been erased and contains no valid data.
EMBER_LIBRARY_NOT_PRESENT = 0xB5,           // The requested function cannot be executed because the library that contains the necessary functionality is not present.
EMBER_OPERATION_IN_PROGRESS = 0xBA,         // The stack accepted the command and is currently processing the request. The results will be returned via an appropriate handler.
EMBER_APPLICATION_ERROR_0 = 0xF0,           // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
EMBER_APPLICATION_ERROR_1 = 0xF1,           // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL
EMBER_APPLICATION_ERROR_2 = 0xF2,           // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
EMBER_APPLICATION_ERROR_3 = 0xF3,           // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
EMBER_APPLICATION_ERROR_4 = 0xF4,           // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
EMBER_APPLICATION_ERROR_5 = 0xF5,           // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
EMBER_APPLICATION_ERROR_6 = 0xF6,           // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
EMBER_APPLICATION_ERROR_7 = 0xF7,           // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
EMBER_APPLICATION_ERROR_8 = 0xF8,           // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
EMBER_APPLICATION_ERROR_9 = 0xF9,           // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
EMBER_APPLICATION_ERROR_10 = 0xFA,          // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
EMBER_APPLICATION_ERROR_11 = 0xFB,          // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
EMBER_APPLICATION_ERROR_12 = 0xFC,          // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
EMBER_APPLICATION_ERROR_13 = 0xFD,          // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
EMBER_APPLICATION_ERROR_14 = 0xFE,          // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
EMBER_APPLICATION_ERROR_15 = 0xFF,          // This error is reserved for customer application use. This will never be returned from any portion of the network stack or HAL.
};

}

#endif
