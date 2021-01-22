/**
 * ezsp.cpp
 *
 *  EZSP Implementation
 *
 *  Created on: Apr 27, 2020
 *      Author: Denis Kudia
 */

#include "ezsp.h"

namespace zb_ezsp {

uint8_t EzspVersion::_ver = 7; //4;

std::map<id_type, std::string> Ezsp::_frameId2String = {
    {ID_version, std::string("Version")},
    {ID_callback, std::string("Callback")},
    {ID_noCallbacks, std::string("No Callback")},
    {ID_getMfgToken, std::string("getMfgToken")},
    {ID_stackTokenChangedHandler, std::string("stackTokenChangedHandler")},
    {ID_networkInit, std::string("networkInit")},

    {ID_networkState, std::string("networkState")},
    {ID_stackStatusHandler, std::string("stackStatusHandler")},

    {ID_startScan, std::string("startScan")},
    {ID_networkFoundHandler, std::string("networkFoundHandler")},
    {ID_scanCompleteHandler, std::string("scanCompleteHandler")},
    {ID_stopScan, std::string("stopScan")},
    {ID_formNetwork, std::string("formNetwork")},
    {ID_leaveNetwork, std::string("leaveNetwork")},

    {ID_permitJoining, std::string("permitJoining")},
    {ID_childJoinHandler, std::string("childJoinHandler")},
    {ID_trustCenterJoinHandler, std::string("trustCenterJoinHandler")},

    {ID_getEui64, std::string("getEui64")},
    {ID_getNodeId, std::string("getNodeId")},
    {ID_getNetworkParameters, std::string("getNetworkParameters")},
    {ID_getParentChildParameters, std::string("getParentChildParameters")},

    {ID_clearBindingTable, std::string("clearBindingTable")},
    {ID_setBinding, std::string("setBinding")},
    {ID_getBinding, std::string("getBinding")},

    {ID_sendUnicast, std::string("sendUnicast")},
    {ID_messageSentHandler, std::string("messageSentHandler")},
    {ID_incomingMessageHandler, std::string("incomingMessageHandler")},
    {ID_energyScanResultHandler, std::string("energyScanResultHandler")},
    {ID_getChildData, std::string("getChildData")},
    {ID_getConfigurationValue, std::string("getConfigurationValue")},
    {ID_setConfigurationValue, std::string("setConfigurationValue")},
    {ID_invalidCommand, std::string("invalidCommand")},

    {ID_lookupEui64ByNodeId, std::string("lookupEui64ByNodeId")},

    {ID_setInitialSecurityState, std::string("setInitialSecurityState")},
    {ID_getCurrentSecurityState, std::string("getCurrentSecurityState")},
    {ID_getKey, std::string("getKey")},
    {ID_becomeTrustCenter, std::string("becomeTrustCenter")},

    {ID_networkInitExtended, std::string("networkInitExtended")},
    {ID_neighborCount, std::string("neighborCount")},
    {ID_setExtendedTimeout, std::string("setExtendedTimeout")},
    {ID_getExtendedTimeout, std::string("getExtendedTimeout")},
    {ID_incomingRouteErrorHandler, std::string("incomingRouteErrorHandler")},
    {ID_Echo, std::string("Echo")},
    {ID_unicastNwkKeyUpdate, std::string("unicastNwkKeyUpdate")},
    {ID_getValue , std::string("getValue")},
    {ID_setValue, std::string("setValue")},
    {ID_broadcastNextNetworkKey, std::string("broadcastNextNetworkKey")},
    {ID_broadcastNetworkKeySwitch, std::string("broadcastNetworkKeySwitch")},
    {ID_clearKeyTable, std::string("ID_clearKeyTable")}
};

}