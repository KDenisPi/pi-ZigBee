/**
 * ezsp_zcl.h
 *
 *  ZigBee Cluster Library.  Data structure definition
 *
 *  Created on: Jul 23, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_EZSP_ZCL_H_
#define PI_ZIGBEE_LIB_EZSP_ZCL_H_

#include <cstdint>
#include <stddef.h>
#include <string.h>
#include <algorithm>

namespace zb_ezsp {
namespace zcl {

enum AttributeReportingStatus : uint8_t {
    Pending = 0x00, //Pending
    Complete = 0x01 // Attribute Reporting Complete
};


enum GeneralCmd : uint8_t {
    AttrRead                = 0x00,     // Read Attributes
    AttrReadResp            = 0x01,     // Read Attributes Response
    AttrWrite               = 0x02,     // Write Attributes
    AttrWriteUndiv          = 0x03,     // Write Attributes Undivided
    AttrWriteResp           = 0x04,     // Write Attributes Response
    AttrWriteNoResp         = 0x05,     // Write Attributes No Response
    CfgReporting            = 0x06,     // Configure Reporting
    CfgReportingResp        = 0x07,     // Configure Reporting Response
    CfgReportingRead        = 0x08,     // Read Reporting Configuration
    CfgReportingReadReap    = 0x09,     // Read Reporting Configuration Response
    ReportAttr              = 0x0a,     // Report attributes
    DefaultResponse         = 0x0b,     // Default Response
    DiscoverAttr            = 0x0c,     // Discover Attributes
    DiscoverAttrResp        = 0x0d,     // Discover Attributes Response
    AttrReadStruct          = 0x0e,     // Read Attributes Structured
    AttrWriteStruct         = 0x0f,     // Write Attributes Structured
    AttrWriteStructResp     = 0x10,     // Write Attributes Structured response
    DiscoverCmdReceived     = 0x11,     // Discover Commands Received
    DiscoverCmdRecivedResp  = 0x12,     // Discover Commands Received Response
    DiscoverCmdGen          = 0x13,     // Discover Commands Generated
    DiscoverCmdGenResp      = 0x14,     // Discover Commands Generated Response
    DiscoverAttrExt         = 0x15,     // Discover Attributes Extended
    DiscoverAttrExtResp     = 0x16      //Discover Attributes Extended Response
};

enum ZclFrameCtrl_Type : uint8_t {
    FType_Global = 0x00,
    FType_Specific = 0x01
};

#define ZclFrameCtrl_MnfSpec                (1<<2)  //Manufacturer specific
#define ZclFrameCtrl_Dir_ServerClient       (1<<3)  //Direction (0 - Client to Server, 1 - Server to Client)
#define ZclFrameCtrl_DefResp_Disable        (1<<4)  //Disable Default Response

class ZclFrame {
public:
    uint8_t _control;
    uint16_t _mnf_code;
    uint8_t _seq_num;
    uint8_t _cmd;

    size_t _psize;            //payload size
    uint8_t _pyaload[130];     //payload

    const bool is_MnfSpec() const {
        return (_control & ZclFrameCtrl_MnfSpec);
    }

    void MnfSpec(){
        _control |= ZclFrameCtrl_MnfSpec;
    }

    const uint16_t SeqNum() const {
        return _seq_num;
    }

    void set_SeqNum(const uint16_t seq_num){
        _seq_num = seq_num;
    }

    const uint8_t cmd() const {
        return _cmd;
    }

    void set_cmd(const uint8_t cmd){
        _cmd = cmd;
    }

    void set_payload(const size_t psize, const uint8_t* buff){
        memcpy(_pyaload, buff, std::min(psize, sizeof(_pyaload)));
    }

};

} //namespace ZCL
}

#endif