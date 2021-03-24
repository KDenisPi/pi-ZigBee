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

#include "ezsp_types.h"

namespace zb_ezsp {

//The default value for the centralized security global trust center link key shall have a value of 5A 69 67 42 65 65 41 6C 6C 69 61 6E 63 65 30 39 (ZigBeeAlliance09).
//EmberKeyData GlobalTrustCenterLinkKey = {0x5A, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6C, 0x6C, 0x69, 0x61, 0x6E, 0x63, 0x65, 0x30, 0x39};

using EId = enum EFrame_ID : id_type {
    ID_version = 0x00,
    ID_callback = 0x06,                     //Allows the NCP to respond with a pending callback.
    ID_noCallbacks = 0x07,                  //Indicates that there are currently no pending callbacks.
    ID_getMfgToken = 0x0B,                  //Retrieves a manufacturing token from the Flash Information Area of the NCP (except for EZSP_STACK_CAL_DATA which is managed by the stack).
    ID_stackTokenChangedHandler = 0x0D,     //A callback invoked to inform the application that a stack token has changed.
    ID_networkInit = 0x17,                  //Resume network operation after a reboot. The node retains its original type. This should be called on startup whether or
                                            //not the node was previously part of a network. EMBER_NOT_JOINED is returned if the node is not part of a network.

    ID_requestLinkKey = 0x14,               //A function to request a Link Key from the Trust Center with another device on the Network (which could be the Trust Center).
                                            //A Link Key with the Trust Center is possible but the requesting device cannot be the Trust Center.
                                            // Link Keys are optional in ZigBee Standard Security and thus the stack cannot know whether the other device supports them.

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

    ID_permitJoining = 0x22,                //Tells the stack to allow other nodes to join the network with this node as their parent. Joining is initially disabled by default.
    ID_childJoinHandler = 0x23,             //Indicates that a child has joined or left.
    ID_trustCenterJoinHandler = 0x24,       //
    ID_getEui64 = 0x26,                     //Returns the EUI64 ID of the local node.
    ID_getNodeId = 0x27,                    //Returns the 16-bit node ID of the local node.
    ID_getNetworkParameters = 0x28,         //Returns the current network parameters.
    ID_getParentChildParameters = 0x29,     //Returns information about the children of the local node and the parent of the local node.

    ID_clearBindingTable = 0x2A,            //Deletes all binding table entries.
    ID_setBinding = 0x2B,                   //Sets an entry in the binding table.
    ID_getBinding = 0x2C,                   //Gets an entry from the binding table.

    ID_sendUnicast  = 0x34,                 //Sends a unicast message as per the ZigBee specification. The message will arrive at its destination only if there is a known route to the destination node
    ID_messageSentHandler = 0x3F,           //A callback indicating the stack has completed sending a message
    ID_incomingMessageHandler = 0x45,       //A callback indicating a message has been received.
    ID_energyScanResultHandler = 0x48,
    ID_getChildData = 0x4A,                 //Returns information about a child of the local node.
    ID_getConfigurationValue = 0x52,
    ID_setConfigurationValue = 0x53,
    ID_invalidCommand = 0x58,

    ID_incomingRouteRecordHandler = 0x59,   //Reports the arrival of a route record command frame.

    ID_lookupEui64ByNodeId = 0x61,          //Returns the EUI64 that corresponds to the specified node ID. The EUI64 is found by searching through all stack tables for the specified node ID.
    ID_incomingSenderEui64Handler = 0x62,   //A callback indicating a message has been received containing the EUI64 of the sender. This callback is called immediately
                                            //before the incomingMessageHandler callback. It is not called if the incoming message did not contain the EUI64 of the sender.

    ID_setInitialSecurityState = 0x68,      //Sets the security state that will be used by the device when it forms or joins the network.
    ID_getCurrentSecurityState = 0x69,      //Gets the current security state that is being used by a device that is joined in the network.
    ID_getKey = 0x6a,                       //Gets a Security Key based on the passed key type.

    ID_networkInitExtended  = 0x70,         //Similar to ezspNetworkInit(). Resume network operation after a reboot. This command is different in that it accepts options to control the network initialization.
    ID_broadcastNextNetworkKey = 0x73,      //This function broadcasts a new encryption key, but does not tell the nodes in the network to start using it. To tell nodes to
                                            //switch to the new key, use emberSendNetworkKeySwitch(). This is only valid for the Trust Center/Coordinator. It is up to the application
                                            //to determine how quickly to send the Switch Key after sending the alternate encryption key.
    ID_broadcastNetworkKeySwitch = 0x74,    //This function broadcasts a switch key message to tell all nodes to change to the sequence number of the previously sent Alternate Encryption Key.
    ID_findKeyTableEntry = 0x75,            //This function searches through the Key Table and tries to find the entry that matches the passed search criteria.

    ID_becomeTrustCenter = 0x77,            //This function causes a coordinator to become the Trust Center when it is operating in a network that is not using one.

    ID_neighborCount = 0x7A,                //Returns the number of active entries in the neighbor table.
    ID_getRouteTableEntry = 0x7B,           //Returns the route table entry at the given index. The route table size can be obtained using the getConfigurationValue command.
    ID_setExtendedTimeout = 0x7E,           //Tells the stack whether or not the normal interval between retransmissions of a retried unicast message should be increased by EMBER_INDIRECT_TRANSMISSION_TIMEOUT.
    ID_getExtendedTimeout = 0x7F,           //Indicates whether or not the stack will extend the normal interval between retransmissions of a retried unicast message by EMBER_INDIRECT_TRANSMISSION_TIMEOUT.
    ID_incomingRouteErrorHandler = 0x80,
    ID_Echo = 0x81,

    ID_zigbeeKeyEstablishmentHandler = 0x9B,    //This is a callback that indicates the success or failure of an attempt to establish a key with a partner device.

    ID_unicastNwkKeyUpdate = 0xA9,          // This command will send a unicast transport key message with a new NWK key to the specified device. APS encryption using the device's existing link key will be used.
    ID_getValue = 0xAA,                     // Reads a value from the NCP.
    ID_setValue = 0xAB,                     // Writes a value to the NCP.
    ID_clearKeyTable = 0xB1                 // This function clears the key table of the current network.
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
    EMBER_INVALID_CALL = 0x70,                  // The API call is not allowed given the current state of the stack.
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
    EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT = 0x13, // The maximum amount of time that an end device child can wait between polls. If no poll is heard within
                                                //this timeout, then the parent removes the end device from its tables.
    EZSP_CONFIG_MOBILE_NODE_POLL_TIMEOUT = 0x14,// The maximum amount of time that a mobile node can wait between polls. If no poll is heard within this
                                                //timeout, then the parent removes the mobile node from its tables.
    EZSP_CONFIG_RESERVED_MOBILE_CHILD_ENTRIES = 0x15, // The number of child table entries reserved for use only by mobile nodes.
    EZSP_CONFIG_TX_POWER_MODE = 0x17,           // Enables boost power mode and/or the alternate transmitter output.
    EZSP_CONFIG_DISABLE_RELAY = 0x18,           // 0: Allow this node to relay messages. 1: Prevent this node from relaying messages.
    EZSP_CONFIG_TRUST_CENTER_ADDRESS_CACHE_SIZE = 0x19, // The maximum number of EUI64 to network address associations that the Trust Center can maintain.
                                                //These address cache entries are reserved for and reused by the Trust Center when processing device
                                                //join/rejoin authentications. This cache size limits the number of overlapping joins the Trust Center can
                                                //process within a narrow time window (e.g. two seconds), and thus should be set to the maximum
                                                //number of near simultaneous joins the Trust Center is expected to accommodate. (Note, the total number
                                                //of such address associations maintained by the NCP is the sum of the value of this setting and the value of::EZSP_CONFIG_ADDRESS_TABLE_SIZE.)
    EZSP_CONFIG_SOURCE_ROUTE_TABLE_SIZE = 0x1A, // The size of the source route table.
    EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT_SHIFT = 0x1B, // The units used for timing out end devices on their parents.
    EZSP_CONFIG_FRAGMENT_WINDOW_SIZE = 0x1C,    // The number of blocks of a fragmented message that can be sent in a single window.
    EZSP_CONFIG_FRAGMENT_DELAY_MS = 0x1D,       // The time the stack will wait (in milliseconds) between sending blocks of a fragmented message.
    EZSP_CONFIG_KEY_TABLE_SIZE = 0x1E,          // The size of the Key Table used for storing individual link keys (if the device is a Trust Center) or
                                                //Application Link Keys (if the device is a normal node).
    EZSP_CONFIG_APS_ACK_TIMEOUT = 0x1F,         // The APS ACK timeout value. The stack waits this amount of time between resends of APS retried messages.
    EZSP_CONFIG_ACTIVE_SCAN_DURATION = 0x20,    // The duration of an active scan, in the units used by the 15.4 scan parameter (((1 << duration) + 1) *
                                                //15ms). This also controls the jitter used when responding to a beacon request.
    EZSP_CONFIG_END_DEVICE_BIND_TIMEOUT = 0x21, // The time the coordinator will wait (in seconds) for a second end device bind request to arrive.
    EZSP_CONFIG_PAN_ID_CONFLICT_REPORT_THRESHOLD = 0x22, //  The number of PAN id conflict reports that must be received by the network manager within one minute to trigger a PAN id change.
    EZSP_CONFIG_REQUEST_KEY_TIMEOUT = 0x24,     // The timeout value in minutes for how long the Trust Center or a normal node waits for the ZigBee
                                                //Request Key to complete. On the Trust Center this controls whether or not the device buffers the request, waiting for a matching pair of ZigBee
                                                //Request Key. If the value is non-zero, the Trust Center buffers and waits for that amount of time. If the value is zero, the Trust Center does not buffer the
                                                //request and immediately responds to the request. Zero is the most compliant behavior.
    EZSP_CONFIG_CERTIFICATE_TABLE_SIZE = 0x29,  // This value indicates the size of the runtime modifiable certificate table. Normally certificates are stored in
                                                //MFG tokens but this table can be used to field upgrade devices with new Smart Energy certificates. This value cannot be set, it can only be queried.
    EZSP_CONFIG_APPLICATION_ZDO_FLAGS = 0x2A,   //This is a bitmask that controls which incoming ZDO request messages are passed to the application. The
                                                //bits are defined in the EmberZdoConfigurationFlags enumeration. To see if the application is required to send a ZDO response in reply to an incoming
                                                //message, the application must check the APS options bit field within the incomingMessageHandler callback to see
                                                //if the EMBER_APS_OPTION_ZDO_RESPONSE_REQUIRED flag is set.
    EZSP_CONFIG_BROADCAST_TABLE_SIZE = 0x2B,    // The maximum number of broadcasts during a single broadcast timeout period.
    EZSP_CONFIG_MAC_FILTER_TABLE_SIZE = 0x2C,   // The size of the MAC filter list table.
    EZSP_CONFIG_SUPPORTED_NETWORKS = 0x2D,      // The number of supported networks.
    EZSP_CONFIG_SEND_MULTICASTS_TO_SLEEPY_ADDRESS = 0x2E,  // Whether multicasts are sent to the RxOnWhenIdle=true address (0xFFFD) or the sleepy
                                                //broadcast address (0xFFFF). The RxOnWhenIdle=true address is the ZigBee compliant destination for multicasts.
    EZSP_CONFIG_ZLL_GROUP_ADDRESSES = 0x2F,     // ZLL group address initial configuration.
    EZSP_CONFIG_ZLL_RSSI_THRESHOLD = 0x30,      // ZLL rssi threshold initial configuration.
    EZSP_CONFIG_MTORR_FLOW_CONTROL = 0x33,      // Toggles the mtorr flow control in the stack.
    EZSP_CONFIG_RETRY_QUEUE_SIZE = 0x34,        // Setting the retry queue size.
    EZSP_CONFIG_NEW_BROADCAST_ENTRY_THRESHOLD = 0x35,  // Setting the new broadcast entry threshold.
    EZSP_CONFIG_TRANSIENT_KEY_TIMEOUT_S = 0x36, // The length of time, in seconds, that a trust center will store a transient link key that a device can use to join
                                                //its network. A transient key is added with a call to emberAddTransientLinkKey. After the transient key is added,
                                                //it will be removed once this amount of time has passed. A joining device will not be able to use that key
                                                //to join until it is added again on the trust center. The default value is 300 seconds, i.e., 5 minutes.
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

enum EmberSecurityBitmaskMode : uint16_t {
    EMBER_STANDARD_SECURITY_MODE            = 0x0000,       // This denotes that the device is running in a network with ZigBee Standard Security.
    EMBER_DISTRIBUTED_TRUST_CENTER_MODE     = 0x0002,       // This denotes that the device is running in a network without a centralized Trust Center.
    EMBER_TRUST_CENTER_GLOBAL_LINK_KEY      = 0x0004,       // This denotes that the device has a Global Link Key. The Trust Center Link Key is the same across multiple nodes.
    EMBER_HAVE_TRUST_CENTER_LINK_KEY        = 0x0010,       // This denotes that the node has a Trust Center Link Key.
    EMBER_TRUST_CENTER_USES_HASHED_LINK_KEY = 0x0084,       // This denotes that the Trust Center is using a Hashed Link Key.
    EMBER_HAVE_PRECONFIGURED_KEY            = 0x0100,       // This denotes that the preconfiguredKey element has valid data that should be used to configure the initial security state.
    EMBER_HAVE_NETWORK_KEY                  = 0x0200,       // This denotes that the networkKey element has valid data that should be used to configure the initial security state.
    EMBER_GET_LINK_KEY_WHEN_JOINING         = 0x0400,       // This denotes to a joining node that it should attempt to acquire a Trust Center Link Key during joining. This is only
                                                            //  necessary if the device does not have a pre-configured key.
    EMBER_REQUIRE_ENCRYPTED_KEY             = 0x0800,       // This denotes that a joining device should only accept an encrypted network key from the Trust Center (using its pre-
                                                            //  configured key). A key sent in-the-clear by the Trust Center will be rejected and the join will fail. This option is only valid
                                                            //  when utilizing a pre-configured key.
    EMBER_NO_FRAME_COUNTER_RESET            = 0x1000,       // This denotes whether the device should NOT reset its outgoing frame counters (both NWK and APS) when
                                                            //  ::emberSetInitialSecurityState() is called. Normally it is advised to reset the frame counter before joining a new
                                                            //  network. However in cases where a device is joining to the same network a again (but not using
                                                            //  ::emberRejoinNetwork()) it should keep the NWK and APS frame counters stored in its tokens.
    EMBER_GET_PRECONFIGURED_KEY_FROM_INSTALL_CODE   = 0x2000,   // This denotes that the device should obtain its preconfigured key from an installation code stored in the
                                                                //  manufacturing token. The token contains a value that will be hashed to obtain the actual preconfigured key. If that
                                                                //  token is not valid, then the call to emberSetInitialSecurityState() will fail.
    EMBER_HAVE_TRUST_CENTER_EUI64           = 0x0040,       // This denotes that the ::EmberInitialSecurityState::preconfiguredTrustCenterEui64
                                                            //  has a value in it containing the trust center EUI64. The device will only join a network and accept commands from
                                                            //  a trust center with that EUI64. Normally this bit is NOT set, and the EUI64 of the trust center is learned during the join
                                                            //  process. When commissioning a device to join onto an existing network, which is using a trust center, and without
                                                            //  sending any messages, this bit must be set and the field ::EmberInitialSecurityState::preconfiguredTrustCenterEui64
                                                            //  must be populated with the appropriate EUI64.
};

enum EmberDeviceUpdate : uint8_t {
    EMBER_STANDARD_SECURITY_SECURED_REJOIN = 0x0,
    EMBER_STANDARD_SECURITY_UNSECURED_JOIN = 0x1,
    EMBER_DEVICE_LEFT = 0x2,
    EMBER_STANDARD_SECURITY_UNSECURED_REJOIN = 0x3,
    EMBER_UNDEFINED = 0xFF
};

enum EmberJoinDecision : uint8_t {
    EMBER_USE_PRECONFIGURED_KEY = 0x00, // Allow the node to join. The joining node should have a pre-configured key. The security data sent to it will be encrypted with that key.
    EMBER_SEND_KEY_IN_THE_CLEAR = 0x01, // Allow the node to join. Send the network key in-the-clear to the joining device.
    EMBER_DENY_JOIN = 0x02,             // Deny join.
    EMBER_NO_ACTION = 0x03              // Take no action.
};

enum EmberOutgoingMessageType : uint8_t {
    EMBER_OUTGOING_DIRECT               = 0x00, // Unicast sent directly to an EmberNodeId.
    EMBER_OUTGOING_VIA_ADDRESS_TABLE    = 0x01, // Unicast sent using an entry in the address table.
    EMBER_OUTGOING_VIA_BINDING          = 0x02, // Unicast sent using an entry in the binding table.
    EMBER_OUTGOING_MULTICAST            = 0x03, // Multicast message. This value is passed to emberMessageSentHandler() only. It may not be passed to emberSendUnicast().
    EMBER_OUTGOING_BROADCAST            = 0x04 // Broadcast message. This value is passed to emberMessageSentHandler() only. It may not be passed to emberSendUnicast().
};

// Options to use when sending a message.
enum EmberApsOption : uint16_t {
    EMBER_APS_OPTION_NONE = 0x0000,                     // No options.
    EMBER_APS_OPTION_ENCRYPTION = 0x0020,               // Send the message using APS Encryption, using the Link Key shared with the destination node to encrypt the data at the APS Level.
    EMBER_APS_OPTION_RETRY = 0x0040,                    // Resend the message using the APS retry mechanism.
    EMBER_APS_OPTION_ENABLE_ROUTE_DISCOVERY = 0x0100,   // Causes a route discovery to be initiated if no route to the destination is known.
    EMBER_APS_OPTION_FORCE_ROUTE_DISCOVERY = 0x0200,    // Causes a route discovery to be initiated even if one is known.
    EMBER_APS_OPTION_SOURCE_EUI64 = 0x0400,             // Include the source EUI64 in the network frame.
    EMBER_APS_OPTION_DESTINATION_EUI64 = 0x0800,        // Include the destination EUI64 in the network frame.
    EMBER_APS_OPTION_ENABLE_ADDRESS_DISCOVERY = 0x1000, // Send a ZDO request to discover the node ID of the destination, if it is not already know.
    EMBER_APS_OPTION_POLL_RESPONSE = 0x2000,            // Reserved.
    EMBER_APS_OPTION_ZDO_RESPONSE_REQUIRED = 0x4000,    // This incoming message is a ZDO request not handled by the EmberZNet stack, and the application is
                                                        // responsible for sending a ZDO response. This flag is used only when the ZDO is configured to have requests
                                                        // handled by the application. See the EZSP_CONFIG_APPLICATION_ZDO_FLAGS configuration parameter for more information.
    EMBER_APS_OPTION_FRAGMENT = 0x8000                  // This message is part of a fragmented message. This option may only be set for unicasts. The groupId field
                                                        // gives the index of this fragment in the low-order byte. If the low-order byte is zero this is the first fragment and
                                                        // the high-order byte contains the number of fragments in the message.
};

// Incoming message types.
enum EmberIncomingMessageType : uint8_t {
    EMBER_INCOMING_UNICAST = 0x00,                  // Unicast.
    EMBER_INCOMING_UNICAST_REPLY = 0x01,            // Unicast reply.
    EMBER_INCOMING_MULTICAST = 0x02,                // Multicast.
    EMBER_INCOMING_MULTICAST_LOOPBACK = 0x03,       // Multicast sent by the local device.
    EMBER_INCOMING_BROADCAST = 0x04,                // Broadcast.
    EMBER_INCOMING_BROADCAST_LOOPBACK = 0x05,       // Broadcast sent by the local device.
    EMBER_INCOMING_MANY_TO_ONE_ROUTE_REQUEST = 0x06 // Many to one route request.
};

enum EmberBindingType : uint8_t {
    EMBER_UNUSED_BINDING  = 0x00,       // A binding that is currently not in use.
    EMBER_UNICAST_BINDING = 0x01,       // A unicast binding whose 64-bit identifier is the destination EUI64.
    EMBER_MANY_TO_ONE_BINDING = 0x02,   // A unicast binding whose 64-bit identifier is the aggregator EUI64.
    EMBER_MULTICAST_BINDING = 0x03      // A multicast binding whose 64-bit identifier is the group address. A multicast binding
                                        // can be used to send messages to the group and to receive messages sent to the group.
};

enum EmberZdoConfigurationFlags : uint8_t {
    EMBER_APP_RECEIVES_SUPPORTED_ZDO_REQUESTS = 0x01,   //Set this flag in order to receive supported ZDO request messages via the incomingMessageHandler callback. A supported
                                                        //ZDO request is one that is handled by the EmberZNet stack. The stack will continue to handle the request and
                                                        //send the appropriate ZDO response even if this configuration option is enabled.
    EMBER_APP_HANDLES_UNSUPPORTED_ZDO_REQUESTS = 0x02,  //Set this flag in order to receive unsupported ZDO request messages via the
                                                        //incomingMessageHandler callback. An unsupported ZDO request is one that is not
                                                        //handled by the EmberZNet stack, other than to send a 'not supported' ZDO response. If this
                                                        //configuration option is enabled, the stack will no longer send any ZDO response, and it is the
                                                        //application's responsibility to do so.
    EMBER_APP_HANDLES_ZDO_ENDPOINT_REQUESTS = 0x04,     //Set this flag in order to receive the following ZDO request messages via the
                                                        //incomingMessageHandler callback: SIMPLE_DESCRIPTOR_REQUEST, MATCH_DESCRIPTORS_REQUEST, and ACTIVE_ENDPOINTS_REQUEST. If this
                                                        //configuration option is enabled, the stack will no longer send any ZDO response for these requests, and it is the application's responsibility
                                                        //to do so.
    EMBER_APP_HANDLES_ZDO_BINDING_REQUESTS = 0x08,      //Set this flag in order to receive the following ZDO request messages via the
                                                        //incomingMessageHandler callback: BINDING_TABLE_REQUEST, BIND_REQUEST, and UNBIND_REQUEST. If this configuration
                                                        //option is enabled, the stack will no longer send any ZDO response for these requests, and it is the
                                                        //application's responsibility to do so.
};

enum EzspMfgTokenId : uint8_t {
    EZSP_MFG_CUSTOM_VERSION     = 0x00, //Custom version (2 bytes).
    EZSP_MFG_STRING             = 0x01, // Manufacturing string (16 bytes).
    EZSP_MFG_BOARD_NAME         = 0x02, // Board name (16 bytes).
    EZSP_MFG_MANUF_ID           = 0x03, // Manufacturing ID (2 bytes).
    EZSP_MFG_PHY_CONFIG         = 0x04, // Radio configuration (2 bytes).
    EZSP_MFG_BOOTLOAD_AES_KEY   = 0x05, // Bootload AES key (16 bytes).
    EZSP_MFG_ASH_CONFIG         = 0x06, // ASH configuration (40 bytes).
    EZSP_MFG_EZSP_STORAGE       = 0x07, // EZSP storage (8 bytes).
    EZSP_STACK_CAL_DATA         = 0x08, // Radio calibration data (64 bytes). 4 bytes are stored for each of the 16 channels. This token is not stored in the Flash
                                        //  Information Area. It is updated by the stack each time a calibration is performed.
    EZSP_MFG_CBKE_DATA          = 0x09, // Certificate Based Key Exchange (CBKE) data (92 bytes).
    EZSP_MFG_INSTALLATION_CODE  = 0x0A, // Installation code (20 bytes).
    EZSP_STACK_CAL_FILTER       = 0x0B, // Radio channel filter calibration data (1 byte). This token is not
                                        //  stored in the Flash Information Area. It is updated by the stack each time a calibration is performed.
    EZSP_MFG_CUSTOM_EUI_64      = 0x0C, // Custom EUI64 MAC address (8 bytes).
    EZSP_MFG_CTUNE              = 0x0D  // CTUNE value (2 byte).
};

enum EmberKeyTypeEnum : uint8_t {
    EMBER_TRUST_CENTER_LINK_KEY = 0x01, // A shared key between the Trust Center and a device.
    EMBER_CURRENT_NETWORK_KEY   = 0x03, // The current active Network Key used by all devices in the network.
    EMBER_NEXT_NETWORK_KEY      = 0x04, // The alternate Network Key that was previously in use, or the newer key that will be switched to.
    EMBER_APPLICATION_LINK_KEY  = 0x05  // An Application Link Key shared with another (non-Trust Center) device.
};

/**
 * Describes the presence of valid data within the EmberKeyStruct structure.
 */
enum EmberKeyStructBitmaskMode : uint16_t {
    EMBER_KEY_HAS_SEQUENCE_NUMBER        = 0x0001, // The key has a sequence number associated with it.
    EMBER_KEY_HAS_OUTGOING_FRAME_COUNTER = 0x0002, // The key has an outgoing frame counter associated with it.
    EMBER_KEY_HAS_INCOMING_FRAME_COUNTER = 0x0004, // The key has an incoming frame counter associated with it.
    EMBER_KEY_HAS_PARTNER_EUI64          = 0x0008  // The key has a Partner IEEE address associated with it.
};

/**
 * The status of the attempt to establish a key.
 */
enum EmberKeyStatus : uint8_t {
    EMBER_APP_LINK_KEY_ESTABLISHED                      = 0x01,
    EMBER_TRUST_CENTER_LINK_KEY_ESTABLISHED             = 0x03,
    EMBER_KEY_ESTABLISHMENT_TIMEOUT                     = 0x04,
    EMBER_KEY_TABLE_FULL                                = 0x05,
    EMBER_TC_RESPONDED_TO_KEY_REQUEST                   = 0x06,
    EMBER_TC_APP_KEY_SENT_TO_REQUESTER                  = 0x07,
    EMBER_TC_RESPONSE_TO_KEY_REQUEST_FAILED             = 0x08,
    EMBER_TC_REQUEST_KEY_TYPE_NOT_SUPPORTED             = 0x09,
    EMBER_TC_NO_LINK_KEY_FOR_REQUESTER                  = 0x0A,
    EMBER_TC_REQUESTER_EUI64_UNKNOWN                    = 0x0B,
    EMBER_TC_RECEIVED_FIRST_APP_KEY_REQUEST             = 0x0C,
    EMBER_TC_TIMEOUT_WAITING_FOR_SECOND_APP_KEY_REQUEST = 0x0D,
    EMBER_TC_NON_MATCHING_APP_KEY_REQUEST_RECEIVED      = 0x0E,
    EMBER_TC_FAILED_TO_SEND_APP_KEYS                    = 0x0F,
    EMBER_TC_FAILED_TO_STORE_APP_KEY_REQUEST            = 0x10,
    EMBER_TC_REJECTED_APP_KEY_REQUEST                   = 0x11
};

}

#endif
