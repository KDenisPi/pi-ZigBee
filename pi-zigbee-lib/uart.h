/**
 * uart.h
 *
 * UART support
 *
 *  Created on: Apr 1, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_UART_H_
#define PI_ZIGBEE_LIB_UART_H_

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <string>

#include "logger.h"
#include "uart_frame.h"

namespace zb_uart {

/**
 *
 */

class ZBUart {
public:
    /**
     *
     */
    ZBUart() : _fd(-1) {
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__));
    }

    /**
     *
     */
    ~ZBUart(){
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__));

        disconnect();
    }

    /**
     *
     */
    bool connect(const std::string& port, const int bitrate){
        bool result = false;

        if(_fd>0){
            return true;
        }

        _fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
        if(_fd < 0){
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Could not open device: " + std::to_string(errno));
            return false;
        }

        /**
         * Configure speed
         */
        struct termios tio;
        int res = tcgetattr(_fd, &tio);
        if(res < 0){
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Could not get device attributes: " + std::to_string(errno));
        }
        else{
            if (cfsetospeed(&tio, B57600)  < 0){
                logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Could not set speed: " + std::to_string(errno));
            }
            else{
                if (tcsetattr(_fd, TCSAFLUSH, &tio) < 0){
                    logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Could not set attribute: " + std::to_string(errno));
                }
                else{
                    result = true;
                    logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " UART configured");
                }
            }
        }

       if(!result){
           disconnect();
       }
       else
           get_info();

        return result;
    }

    /**
     *
     */
    void disconnect(){
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__));

        if(_fd >= 0){
            int res = close(_fd);
            _fd = -1;
        }
    }

    /**
     *
     * Copy and move is not allowed
    */
    ZBUart(const ZBUart&) = delete;
    ZBUart(ZBUart&&) = delete;
    ZBUart& operator=(const ZBUart&) = delete;
    ZBUart& operator=(ZBUart&&) = delete;

private:



    /**
     * Print terminal configurationinformation
     */
    void get_info() const {
        struct termios tio;

        int res = tcgetattr(_fd, &tio);
        if(res != 0){
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Could not get device attributes: " + std::to_string(errno));
            return;
        }

        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Input flags: " + std::to_string(tio.c_iflag));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IGNBRK  " + std::to_string(tio.c_iflag&IGNBRK));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " BRKINT  " + std::to_string(tio.c_iflag&BRKINT));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IGNPAR  " + std::to_string(tio.c_iflag&IGNPAR));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " PARMRK  " + std::to_string(tio.c_iflag&PARMRK));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " INPCK   " + std::to_string(tio.c_iflag&INPCK));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " ISTRIP  " + std::to_string(tio.c_iflag&ISTRIP));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " INLCR   " + std::to_string(tio.c_iflag&INLCR));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IGNCR   " + std::to_string(tio.c_iflag&IGNCR));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " ICRNL   " + std::to_string(tio.c_iflag&ICRNL));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IXON    " + std::to_string(tio.c_iflag&IXON));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IXANY   " + std::to_string(tio.c_iflag&IXANY));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IXOFF   " + std::to_string(tio.c_iflag&IXOFF));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IMAXBEL " + std::to_string(tio.c_iflag&IMAXBEL));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IUTF8   " + std::to_string(tio.c_iflag&IUTF8));

        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Output flags: " + std::to_string(tio.c_oflag));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " OPOST  " + std::to_string(tio.c_oflag&OPOST));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " OLCUC  " + std::to_string(tio.c_oflag&OLCUC));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " OLCUC  " + std::to_string(tio.c_oflag&OLCUC));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " OCRNL  " + std::to_string(tio.c_oflag&OCRNL));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " ONOCR  " + std::to_string(tio.c_oflag&ONOCR));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " ONLRET " + std::to_string(tio.c_oflag&ONLRET));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " OFILL  " + std::to_string(tio.c_oflag&OFILL));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " OFDEL  " + std::to_string(tio.c_oflag&OFDEL));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " NLDLY  " + std::to_string(tio.c_oflag&NLDLY));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " CRDLY  " + std::to_string(tio.c_oflag&CRDLY));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " TABDLY " + std::to_string(tio.c_oflag&TABDLY));

        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Control flags: " + std::to_string(tio.c_cflag));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Local flags: " + std::to_string(tio.c_lflag));

        //Print speed
        speed_t irate, orate;
        irate = cfgetispeed(&tio);
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Input speed: " + std::to_string(irate));
        orate = cfgetospeed(&tio);
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Output speed:" + std::to_string(orate));
    }


private:
    int _fd;
};

} //end namespace zb_uart

#endif
