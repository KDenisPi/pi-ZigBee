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
using EmberNodeId = uint16_t;           // 16-bit ZigBee network address.

using EId = enum EFrame_ID : id_type {
    ID_version = 0x00,
    ID_callback = 0x06,                     //Allows the NCP to respond with a pending callback.
    ID_noCallbacks = 0x07,                  //Indicates that there are currently no pending callbacks.
    ID_networkInit = 0x17,                  //Resume network operation after a reboot. The node retains its original type. This should be called on startup whether or
                                            //not the node was previously part of a network. EMBER_NOT_JOINED is returned if the node is not part of a network.
    ID_networkState = 0x18,                 //Returns a value indicating whether the node is joining, joined to, or leaving a network.
    ID_stackStatusHandler = 0x19,           //A callback invoked when the status of the stack changes. If the status parameter equals EMBER_NETWORK_UP, then the getNetworkParameters command
                                            //can be called to obtain the new network parameters. If any of the parameters are being stored in nonvolatile memory by the Host, the stored values should be updated.
    ID_startScan = 0x1A,
    ID_networkFoundHandler = 0x1B,
    ID_scanCompleteHandler = 0x1C,
    ID_stopScan = 0x1D,
    ID_formNetwork = 0x1E,                  //Forms a new network by becoming the coordinator.
    ID_leaveNetwork = 0x20,                 //Causes the stack to leave the current network. This generates a stackStatusHandler callback to indicate that the network
                                            //is down. The radio will not be used until after sending a formNetwork or joinNetwork command.

    ID_getNetworkParameters = 0x28,         //Returns the current network parameters.
    ID_energyScanResultHandler = 0x48,
    ID_getConfigurationValue = 0x52,
    ID_setConfigurationValue = 0x53,
    ID_invalidCommand = 0x58,
    ID_networkInitExtended  = 0x70,    //Similar to ezspNetworkInit(). Resume network operation after a reboot. This command is different in that it accepts options to control the network initialization.
    ID_Echo = 0x81,
    ID_getValue = 0xAA,
    ID_setValue = 0xAB
};

using EzspNetworkScanType = enum EFrame_EzspNetworkScanType : uint8_t {
    EZSP_ENERGY_SCAN = 0x00,    //An energy scan scans each channel for its RSSI value.
    EZSP_ACTIVE_SCAN = 0x01     //An active scan scans each channel for available networks.
};

/**
 * List of EZSP Statuses
 */
using EzspStatus = enum EFrame_EzspStatus : uint8_t{
    EZSP_SUCCESS = 0x00,                        // Success.
    EZSP_SPI_ERR_FATAL = 0x10,                  // Fatal error.
    EZSP_SPI_ERR_NCP_RESET = 0x11,              // The Response frame of the current transaction indicates the NCP has reset.
    EZSP_SPI_ERR_OVERSIZED_EZSP_FRAME = 0x12,   // The NCP is reporting that the Command frame of the current transaction is oversized (the length byte is too large).
    EZSP_SPI_ERR_ABORTED_TRANSACTION = 0x13,    // The Response frame of the current transaction indicates the previous transaction was aborted (nSSEL deasserted too soon).
    EZSP_SPI_ERR_MISSING_FRAME_TERMINATOR = 0x14, // The Response frame of the current transaction indicates the frame terminator is missing from the Command frame.
    EZSP_SPI_ERR_WAIT_SECTION_TIMEOUT = 0x15,   // The NCP has not provided a Response within the time limit defined by WAIT_SECTION_TIMEOUT.
    EZSP_SPI_ERR_NO_FRAME_TERMINATOR = 0x16,    // The Response frame from the NCP is missing the frame terminator.
    EZSP_SPI_ERR_EZSP_COMMAND_OVERSIZED = 0x17, // The Host attempted to send an oversized Command (thelength byte is too large) and the AVR's spi-protocol.c blocked the transmission.
    EZSP_SPI_ERR_EZSP_RESPONSE_OVERSIZED = 0x18,// The NCP attempted to send an oversized Response (the length byte is too large) and the AVR's spi-protocol.c blocked the reception.
    EZSP_SPI_WAITING_FOR_RESPONSE = 0x19,       // The Host has sent the Command and is still waiting for the NCP to send a Response.
    EZSP_SPI_ERR_HANDSHAKE_TIMEOUT = 0x1A,      // The NCP has not asserted nHOST_INT within the time limit defined by WAKE_HANDSHAKE_TIMEOUT.
    EZSP_SPI_ERR_STARTUP_TIMEOUT = 0x1B,        // The NCP has not asserted nHOST_INT after an NCP reset within the time limit defined by STARTUP_TIMEOUT.
    EZSP_SPI_ERR_STARTUP_FAIL = 0x1C,           // The Host attempted to verify the SPI Protocol activity and version number, and the verification failed.
    EZSP_SPI_ERR_UNSUPPORTED_SPI_COMMAND = 0x1D, // The Host has sent a command with a SPI Byte that is unsupported by the current mode the NCP is operating in.
    EZSP_ASH_IN_PROGRESS = 0x20,                // Operation not yet complete.
    EZSP_HOST_FATAL_ERROR = 0x21,               // Fatal error detected by host.
    EZSP_ASH_NCP_FATAL_ERROR = 0x22,            // Fatal error detected by NCP.
    EZSP_DATA_FRAME_TOO_LONG = 0x23,            // Tried to send DATA frame too long.
    EZSP_DATA_FRAME_TOO_SHORT = 0x24,           // Tried to send DATA frame too short.
    EZSP_NO_TX_SPACE = 0x25,                    // No space for tx'ed DATA frame.
    EZSP_NO_RX_SPACE = 0x26,                    // No space for rec'd DATA frame.
    EZSP_NO_RX_DATA = 0x27,                     // No receive data available.
    EZSP_NOT_CONNECTED = 0x28,                  // Not in Connected state.
    EZSP_ERROR_VERSION_NOT_SET = 0x30,          // The NCP received a command before the EZSP version had been set.
    EZSP_ERROR_INVALID_FRAME_ID = 0x31,         // The NCP received a command containing an unsupported frame ID.
    EZSP_ERROR_WRONG_DIRECTION = 0x32,          // The direction flag in the frame control field was incorrect.
    EZSP_ERROR_TRUNCATED = 0x33,                // The truncated flag in the frame control field was set, indicating there was not enough memory available to
                                                //complete the response or that the response would have exceeded the maximum EZSP frame length.
    EZSP_ERROR_OVERFLOW =0x34,                  // The overflow flag in the frame control field was set, indicating one or more callbacks occurred since
                                                //the previous response and there was not enough memory available to report them to the Host.
    EZSP_ERROR_OUT_OF_MEMORY = 0x35,            // Insufficient memory was available.
    EZSP_ERROR_INVALID_VALUE = 0x36,            // The value was out of bounds.
    EZSP_ERROR_INVALID_ID = 0x37,               // The configuration id was not recognized.
    EZSP_ERROR_INVALID_CALL = 0x38,             // Configuration values can no longer be modified.
    EZSP_ERROR_NO_RESPONSE = 0x39,              // The NCP failed to respond to a command.
    EZSP_ERROR_COMMAND_TOO_LONG = 0x40,         // The length of the command exceeded the maximum EZSP frame length.
    EZSP_ERROR_QUEUE_FULL = 0x41,               // The UART receive queue was full causing a callback response to be dropped.
    EZSP_ERROR_COMMAND_FILTERED = 0x42,         // The command has been filtered out by NCP.
    EZSP_ERROR_SECURITY_KEY_ALREADY_SET = 0x43, // EZSP Security Key is already set
    EZSP_ERROR_SECURITY_TYPE_INVALID = 0x44,    // EZSP Security Type is invalid
    EZSP_ERROR_SECURITY_PARAMETERS_INVALID = 0x45, // EZSP Security Parameters are invalid
    EZSP_ERROR_SECURITY_PARAMETERS_ALREADY_SET = 0x46, // EZSP Security Parameters are already set
    EZSP_ERROR_SECURITY_KEY_NOT_SET = 0x47,     // EZSP Security Key is not set
    EZSP_ERROR_SECURITY_PARAMETERS_NOT_SET = 0x48, // EZSP Security Parameters are not set
    EZSP_ERROR_UNSUPPORTED_CONTROL = 0x49,      // Received frame with unsupported control byte
    EZSP_ERROR_UNSECURE_FRAME = 0x4A,           // Received frame is unsecure, when security is established
    EZSP_ASH_ERROR_VERSION = 0x50,              // Incompatible ASH version
    EZSP_ASH_ERROR_TIMEOUTS = 0x51,             // Exceeded max ACK timeouts
    EZSP_ASH_ERROR_RESET_FAIL = 0x52,           // Timed out waiting for RSTACK
    EZSP_ASH_ERROR_NCP_RESET = 0x53,            // Unexpected ncp reset
    EZSP_ERROR_SERIAL_INIT = 0x54,              // Serial port initialization failed
    EZSP_ASH_ERROR_NCP_TYPE = 0x55,             // Invalid ncp processor type
    EZSP_ASH_ERROR_RESET_METHOD = 0x56,         // Invalid ncp reset method
    EZSP_ASH_ERROR_XON_XOFF = 0x57,             // XON/XOFF not supported by host driver
    EZSP_ASH_STARTED = 0x70,                    // ASH protocol started
    EZSP_ASH_CONNECTED = 0x71,                  // ASH protocol connected
    EZSP_ASH_DISCONNECTED = 0x72,               // ASH protocol disconnected
    EZSP_ASH_ACK_TIMEOUT = 0x73,                // Timer expired waiting for ack
    EZSP_ASH_CANCELLED = 0x74,                  // Frame in progress cancelled
    EZSP_ASH_OUT_OF_SEQUENCE = 0x75,            // Received frame out of sequence
    EZSP_ASH_BAD_CRC = 0x76,                    // Received frame with CRC error
    EZSP_ASH_COMM_ERROR = 0x77,                 // Received frame with comm error
    EZSP_ASH_BAD_ACKNUM = 0x78,                 // Received frame with bad ackNum
    EZSP_ASH_TOO_SHORT = 0x79,                  // Received frame shorter than minimum
    EZSP_ASH_TOO_LONG = 0x7A,                   // Received frame longer than maximum
    EZSP_ASH_BAD_CONTROL = 0x7B,                // Received frame with illegal control byte
    EZSP_ASH_BAD_LENGTH = 0x7C,                 // Received frame with illegal length for its type
    EZSP_ASH_ACK_RECEIVED = 0x7D,               // Received ASH Ack
    EZSP_ASH_ACK_SENT = 0x7E,                   // Sent ASH Ack
    EZSP_NO_ERROR = 0xFF                        // No reset or error
};

/**
 * List of Ember Statuses
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

using EzspValueId = enum EFrame_EzspValueId : uint8_t {
    EZSP_VALUE_TOKEN_STACK_NODE_DATA = 0x00,                // The contents of the node data stack token.
    EZSP_VALUE_MAC_PASSTHROUGH_FLAGS = 0x01,                // The types of MAC passthrough messages that the host wishes to receive.
    EZSP_VALUE_EMBERNET_PASSTHROUGH_SOURCE_ADDRESS = 0x02,  // The source address used to filter legacy EmberNet messages when the EMBER_MAC_PASSTHROUGH_EMBERNET_SOURCE
                                                            //flag is set in EZSP_VALUE_MAC_PASSTHROUGH_FLAGS.
    EZSP_VALUE_FREE_BUFFERS = 0x03,                         // The number of available message buffers.
    EZSP_VALUE_UART_SYNCH_CALLBACKS = 0x04,                 // Selects sending synchronous callbacks in ezsp-uart.
    EZSP_VALUE_MAXIMUM_INCOMING_TRANSFER_SIZE = 0x05,       // The maximum incoming transfer size for the local node.
    EZSP_VALUE_MAXIMUM_OUTGOING_TRANSFER_SIZE = 0x06,       // The maximum outgoing transfer size for the local node.
    EZSP_VALUE_STACK_TOKEN_WRITING = 0x07,                  // A boolean indicating whether stack tokens are written to persistent storage as they change.
    EZSP_VALUE_STACK_IS_PERFORMING_REJOIN = 0x08,           // A read-only value indicating whether the stack is currently performing a rejoin.
    EZSP_VALUE_MAC_FILTER_LIST = 0x09,                      // A list of EmberMacFilterMatchData values.
    EZSP_VALUE_EXTENDED_SECURITY_BITMASK = 0x0A,            // The Ember Extended Security Bitmask.
    EZSP_VALUE_NODE_SHORT_ID = 0x0B,                        // The node short ID.
    EZSP_VALUE_DESCRIPTOR_CAPABILITY = 0x0C,                // The descriptor capability of the local node.
    EZSP_VALUE_STACK_DEVICE_REQUEST_SEQUENCE_NUMBER = 0x0D, // The stack device request sequence number of the local node.
    EZSP_VALUE_RADIO_HOLD_OFF = 0x0E,                       // Enable or disable radio hold-off.
    EZSP_VALUE_ENDPOINT_FLAGS = 0x0F,                       // The flags field associated with the endpoint data.
    EZSP_VALUE_MFG_SECURITY_CONFIG = 0x10,                  // Enable/disable the Mfg security config key settings.
    EZSP_VALUE_VERSION_INFO = 0x11,                         //Retrieves the version information from the stack on the NCP.
    EZSP_VALUE_NEXT_HOST_REJOIN_REASON = 0x12,              // This will get/set the rejoin reason noted by the host for a subsequent call to emberFindAndRejoinNetwork(). After a
                                                            //call to emberFindAndRejoinNetwork() the host's rejoin reason will be set to EMBER_REJOIN_REASON_NONE.
                                                            //The NCP will store the rejoin reason used by the call to emberFindAndRejoinNetwork()
    EZSP_VALUE_LAST_REJOIN_REASON = 0x13,                   // This is the reason that the last rejoin took place. This value may only be retrieved, not set. The rejoin may have been
                                                            //initiated by the stack (NCP) or the application (host). If a host initiated a rejoin
                                                            //the reason will be set by default to EMBER_REJOIN_DUE_TO_APP_EVENT_1. If the
                                                            //application wishes to denote its own rejoin reasons it can do so by calling
                                                            //ezspSetValue(EMBER_VALUE_HOST_REJOIN_REASON,EMBER_REJOIN_DUE_TO_APP_EVENT_X). X is a number corresponding to one of the app events defined.
                                                            //If the NCP initiated a rejoin it will record this value internallyfor retrieval by ezspGetValue(EZSP_VALUE_REAL_REJOIN_REASON).
    EZSP_VALUE_NEXT_ZIGBEE_SEQUENCE_NUMBER = 0x14,          // The next ZigBee sequence number.
    EZSP_VALUE_CCA_THRESHOLD = 0x15,                        // CCA energy detect threshold for radio.
    EZSP_VALUE_SET_COUNTER_THRESHOLD = 0x17,                // The threshold value for a counter
    EZSP_VALUE_RESET_COUNTER_THRESHOLDS = 0x18,             // Resets all counters thresholds to 0xFF
    EZSP_VALUE_CLEAR_COUNTERS = 0x19,                       // Clears all the counters
    EZSP_VALUE_RF4CE_BASE_CHANNEL = 0x1A,                   // The device RF4CE base channel
    EZSP_VALUE_RF4CE_SUPPORTED_DEVICE_TYPES_LIST = 0x1B,    // The RF4CE device types supported by the node
    EZSP_VALUE_RF4CE_SUPPORTED_PROFILES_LIST = 0x1C,        // The RF4CE profiles supported by the node
    EZSP_VALUE_ENABLE_R21_BEHAVIOR = 0x29,                  // Setting this byte enables R21 behavior on the NCP.
    EZSP_VALUE_ANTENNA_MODE = 0x30,                         // Configure the antenna mode(0-primary,1-secondary,2-toggle on tx ack fail).
    EZSP_VALUE_ENABLE_PTA = 0x31,                           // Enable or disable packet traffic arbitration.
    EZSP_VALUE_PTA_OPTIONS = 0x32,                          // Set packet traffic arbitration configuration options.
    EZSP_VALUE_MFGLIB_OPTIONS = 0x33,                       // Configure manufacturing library options (0-non-CSMA transmits,1-CSMA transmits).
    EZSP_VALUE_RF4CE_GDP_BINDING_RECIPIENT_PARAMETERS = 0x1D, // The GDP binding recipient parameters
    EZSP_VALUE_RF4CE_GDP_PUSH_BUTTON_STIMULUS_RECEIVED_PENDING_FLAG = 0x1E, // The GDP binding push button stimulus received pending flag EZSP_VALUE_NEXT_HOST_REJOIN_REASON
    EZSP_VALUE_RF4CE_GDP_BINDING_PROXY_FLAG = 0x1F,         // The GDP originator proxy flag in the advanced binding options
    EZSP_VALUE_RF4CE_GDP_APPLICATION_SPECIFIC_USER_STRING = 0x20, // The GDP application specific user string
    EZSP_VALUE_RF4CE_MSO_USER_STRING = 0x21,                // The MSO user string
    EZSP_VALUE_RF4CE_MSO_BINDING_RECIPIENT_PARAMETERS = 0x22, // The MSO binding recipient parameters
    EZSP_VALUE_NWK_FRAME_COUNTER = 0x23,                    // The NWK layer security frame counter value
    EZSP_VALUE_APS_FRAME_COUNTER = 0x24,                    // The APS layer security frame counter value
    EZSP_VALUE_RETRY_DEVICE_TYPE = 0x25,                    // Sets the device type to use on the next rejoin using device type
    EZSP_VALUE_RF4CE_BASE_CHANNEL_ = 0x26,                   // The device RF4CE base channel
    EZSP_VALUE_RF4CE_SUPPORTED_DEVICE_TYPES_LIST_ = 0x27,    // The RF4CE device types supported by the node
    EZSP_VALUE_RF4CE_SUPPORTED_PROFILES_LIST_ = 0x28         // The RF4CE profiles supported by the node
};

/**
 *
 */
using EzspConfigId = enum EFrame_EzspConfigId : uint8_t {
    EZSP_CONFIG_PACKET_BUFFER_COUNT = 0x01,     // The number of packet buffers available to the stack. When set to the special value 0xFF, the NCP will allocate all remaining configuration RAM towards
                                                //packet buffers, such that the resulting count will be the largest whole number of packet buffers that can fit into the available memory.
    EZSP_CONFIG_NEIGHBOR_TABLE_SIZE = 0x02,     // The maximum number of router neighbors the stack can keep track of. A neighbor is a node within radio range.
    EZSP_CONFIG_APS_UNICAST_MESSAGE_COUNT = 0x03, // The maximum number of APS retried messages the stack can be transmitting at any time.
    EZSP_CONFIG_BINDING_TABLE_SIZE = 0x04,      // The maximum number of non-volatile bindings supported by the stack.
    EZSP_CONFIG_ADDRESS_TABLE_SIZE = 0x05,      // The maximum number of EUI64 to network address associations that the stack can maintain for the application. (Note, the total number of such address
                                                //associations maintained by the NCP is the sum of the value of this setting and the value of ::EZSP_CONFIG_TRUST_CENTER_ADDRESS_CACHE_SIZE.).
    EZSP_CONFIG_MULTICAST_TABLE_SIZE = 0x06,    // The maximum number of multicast groups that the device may be a member of.
    EZSP_CONFIG_ROUTE_TABLE_SIZE = 0x07,        // The maximum number of destinations to which a node can route messages. This includes both messages originating at this node and those relayed for others.
    EZSP_CONFIG_DISCOVERY_TABLE_SIZE = 0x08,    // The number of simultaneous route discoveries that a node will support.
    EZSP_CONFIG_STACK_PROFILE = 0x0C,           // Specifies the stack profile.
    EZSP_CONFIG_SECURITY_LEVEL = 0x0D,          // The security level used for security at the MAC and network layers. The supported values are 0 (no security) and
                                                //5 (payload is encrypted and a four-byte MIC is used for authentication).
    EZSP_CONFIG_MAX_HOPS = 0x10,                // The maximum number of hops for a message.
    EZSP_CONFIG_MAX_END_DEVICE_CHILDREN = 0x11, // The maximum number of end device children that a router will support.
    EZSP_CONFIG_INDIRECT_TRANSMISSION_TIMEOUT = 0x12, // The maximum amount of time that the MAC will hold a message for indirect transmission to a child.
    EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT = 0x1,  // The maximum amount of time that an end device child can wait between polls. If no poll is heard within
                                                //this timeout, then the parent removes the end device from its tables.
    EZSP_CONFIG_MOBILE_NODE_POLL_TIMEOUT = 0x14 // The maximum amount of time that a mobile node can wait between polls. If no poll is heard within this
                                                //timeout, then the parent removes the mobile node from its tables.
};

/**
 *
 */
enum EmberNetworkInitBitmask : uint16_t {
    EMBER_NETWORK_INIT_NO_OPTIONS  = 0x0000,            // No options for Network Init
    EMBER_NETWORK_INIT_PARENT_INFO_IN_TOKEN = 0x0001,   // Save parent info (node ID and EUI64) in a token during joining/rejoin, and restore on reboot.
    EMBER_NETWORK_INIT_END_DEVICE_REJOIN_ON_REBOOT = 0x0002  // Send a rejoin request as an end device on reboot if parent information is persisted. (ver 8)
};

enum EmberNetworkStatus : uint8_t {
    EMBER_NO_NETWORK  = 0x00,       //The node is not associated with a network in any way.
    EMBER_JOINING_NETWORK = 0x01,   //The node is currently attempting to join a network.
    EMBER_JOINED_NETWORK = 0x02,    //The node is joined to a network.
    EMBER_JOINED_NETWORK_NO_PARENT = 0x03,  //The node is an end device joined to a network but its parent is not responding.
    EMBER_LEAVING_NETWORK = 0x04    //The node is in the process of leaving its current network.
};

enum EmberJoinMethod : uint8_t {        // The type of method used for joining.
    EMBER_USE_MAC_ASSOCIATION = 0x00,   //Normally devices use MAC Association to join a network, which respects the "permit joining" flag in the MAC Beacon. For mobile
                                        //nodes this value causes the device to use an Ember Mobile Node Join, which is functionally equivalent to a MAC association. This value should be used by default.

    EMBER_USE_NWK_REJOIN = 0x01,        //For those networks where the "permit joining" flag is never turned on, they will need to use a ZigBee NWK Rejoin. This value
                                        //causes the rejoin to be sent without NWK security and the Trust Center will be asked to send the NWK key to the device. The
                                        //NWK key sent to the device can be encrypted with the device's corresponding Trust Center link key. That is determined by the
                                        //::EmberJoinDecision on the Trust Center returned by the ::emberTrustCenterJoinHandler(). For a mobile node this value
                                        //will cause it to use an Ember Mobile node rejoin, which is functionally equivalent.

    EMBER_USE_NWK_REJOIN_HAVE_NWK_KEY = 0x02,   //For those networks where the "permit joining" flag is never turned on, they will need to use a NWK Rejoin. If those devices have
                                        //been preconfigured with the NWK key (including sequence number) they can use a secured rejoin. This is only necessary for
                                        //end devices since they need a parent. Routers can simply use the ::EMBER_USE_NWK_COMMISSIONING join method below.

    EMBER_USE_NWK_COMMISSIONING = 0x03  //For those networks where all network and security information is known ahead of time, a router device may be commissioned such
                                        //that it does not need to send any messages to begin communicating on the network.
};


enum EmberNodeType : uint8_t {
    EMBER_UNKNOWN_DEVICE = 0x00,    // Device is not joined.
    EMBER_COORDINATOR = 0x01,       // Will relay messages and can act as a parent to other nodes.
    EMBER_ROUTER = 0x02,            // Will relay messages and can act as a parent to other nodes.
    EMBER_END_DEVICE = 0x03,        // Communicates only with its parent and will not relay messages.
    EMBER_SLEEPY_END_DEVICE = 0x04, // An end device whose radio can be turned off to save power. The application must poll to receive messages.
    EMBER_MOBILE_END_DEVICE = 0x05  // A sleepy end device that can move through the network.
};

}

#endif
