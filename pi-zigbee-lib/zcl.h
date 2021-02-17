/**
 * zcl.h
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

#include "ezsp_util.h"

namespace zb_ezsp {
namespace zcl {

using ProfileId = uint16_t;
using DeviceId = uint16_t;
using ClusterId = uint16_t;

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
    DiscoverAttrExtResp     = 0x16      // Discover Attributes Extended Response
};

enum ZclFrameCtrl_Type : uint8_t {
    FType_Global = 0x00,
    FType_Specific = 0x01
};

#define ZclFrameCtrl_MnfSpec                (1<<2)  //Manufacturer specific
#define ZclFrameCtrl_Dir_ServerClient       (1<<3)  //Direction (0 - Client to Server, 1 - Server to Client)
#define ZclFrameCtrl_DefResp_Disable        (1<<4)  //Disable Default Response

/**
 * Device attribute type
 */
enum AttrType : uint8_t {
    No_data     = 0x00,
    Data8       = 0x08, //8-bit data
    Data16      = 0x09, //16-bit data
    Data24      = 0x0a, //24-bit data
    Data32      = 0x0b, //32-bit data
    Data40      = 0x0c, //16-bit data
    Data48      = 0x0d, //24-bit data
    Data56      = 0x0e, //32-bit data
    Data64      = 0x0f, //32-bit data

    Bool        = 0x10, //Bool

    Bitmap8     = 0x18, //8-bit bitmap
    Bitmap16    = 0x19, //16-bit bitmap
    Bitmap24    = 0x1a, //24-bit bitmap
    Bitmap32    = 0x1b, //32-bit bitmap
    Bitmap40    = 0x1c, //16-bit bitmap
    Bitmap48    = 0x1d, //24-bit bitmap
    Bitmap56    = 0x1e, //32-bit bitmap
    Bitmap64    = 0x1f, //32-bit bitmap

    Uint8       = 0x20, //Unsigned 8-bit
    Uint16      = 0x21, //Unsigned 16-bit
    Uint24      = 0x22, //Unsigned 24-bit
    Uint32      = 0x23, //Unsigned 32-bit
    Uint40      = 0x24, //Unsigned 40-bit
    Uint48      = 0x25, //Unsigned 48-bit
    Uint56      = 0x26, //Unsigned 56-bit
    Uint64      = 0x27, //Unsigned 64-bit

    Int8        = 0x28, //signed 8-bit
    Int16       = 0x29, //signed 16-bit
    Int24       = 0x2a, //signed 24-bit
    Int32       = 0x2b, //signed 32-bit
    Int40       = 0x2c, //signed 40-bit
    Int48       = 0x2d, //signed 48-bit
    Int56       = 0x2e, //signed 56-bit
    Int64       = 0x2f, //signed 64-bit

    Enum8       = 0x30, //8-bit enumeration
    Enum16      = 0x31, //16-bit enumeration

    //Floating point analog
    Semi        = 0x38, //Semi-precision
    Single      = 0x39, //Single-precision
    Double      = 0x3a, //Double-precision

    //String
    Octstr      = 0x41, //Octet string, Length Defined in first octet
    String      = 0x42, //Character string, Length Defined in first octet
    Octstr16    = 0x43, //Long octet string , Length Defined in first two octets
    String16    = 0x44, //Long character string , Length Defined in first two octets

    //Ordered sequence
    Array       = 0x48, //Array, length 2 + sum of lengths of contents ,0xffff in first 2 octets
    Structure   = 0x4c, //Structure, length 2 + sum of lengths of contents 0xffff in first 2 octets

    //Collection
    Set         = 0x50, //Set, Length Sum of lengths of contents, Number of elements returned as 0xffff
    Bag         = 0x51, //Bag, Length Sum of lengths of contents, Number of elements returned as 0xffff

    //Time Analog
    ToD         = 0xe0, //Time of day
    Date        = 0xe1, //Date
    UTC         = 0xe2, //UTCTime

    //Identifier
    Cluster_Id  = 0xe8, //Cluster ID
    AttribId    = 0xe9, //Attribute ID
    BacOID      = 0xea, //BACnet OID

    //Miscellaneous
    EUI64       = 0xf0, //IEEE address
    Key128      = 0xf1, //128-bit security key

    Unk         = 0xff  //Unknown
};

/**
 * Device attribute
 */
class AttrManager;

class Attribute {
public:
    friend AttrManager;
protected:
    Attribute(const AttrType atype) : _len(0), _type(atype) {

    }

    AttrType _type;
    uint16_t _len;
};

class AttrManager {
public:
    Attribute& allocate(const AttrType atype){

    }

};

class ZclFrame {
public:
    ZclFrame() {}
    virtual ~ZclFrame() {}

    /**
     * Genarate frame for specified command
     *
     * Control: 0 - Genaral, No manufacturer specific, Client to Server, Enable Default response
     */
    ZclFrame(const GeneralCmd cmd, const uint8_t seq_num = 1) : _control(0), _mnf_code(0) {
        _seq_num = seq_num;
        _cmd = cmd;
    }

public:
    uint8_t _control;
    uint16_t _mnf_code;
    uint8_t _seq_num;
    uint8_t _cmd;

    const bool is_MnfSpec() const {
        return (_control & ZclFrameCtrl_MnfSpec);
    }

    void MnfSpec(){
        _control |= ZclFrameCtrl_MnfSpec;
    }

    const bool is_Serv2Cln() const {
        return (_control & ZclFrameCtrl_Dir_ServerClient);
    }

    void Serv2Cln(){
        _control |= ZclFrameCtrl_Dir_ServerClient;
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

    /**
     * Put Frame to the buffer
     */
    size_t put(uint8_t* buff, const size_t mx_len){

        size_t len = 0;
        len = Conv::put(buff, len, _control);
        if(is_MnfSpec())
            len = Conv::put(buff, len, _mnf_code);
        len = Conv::put(buff, len, _seq_num);
        len = Conv::put(buff, len, _cmd);

        switch(_cmd){
            case DiscoverAttr:
                uint16_t attr_id_start = 0x0000;
                len = Conv::put(buff, len, attr_id_start);
                len = Conv::put(buff, len, (uint8_t)AttrType::Unk);
            break;
        };

        return len;
    }
};



} //namespace ZCL
}

#endif