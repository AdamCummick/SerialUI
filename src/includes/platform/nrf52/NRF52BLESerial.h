/*
 * NRF52BLESerial.h
 *
 *  Created on: May 12, 2019
 *      Author: malcalypse
 */

#ifndef SERIALUI_SRC_INCLUDES_PLATFORM_NRF52_NRF52BLESERIAL_H_
#define SERIALUI_SRC_INCLUDES_PLATFORM_NRF52_NRF52BLESERIAL_H_

#include "includes/SerialUIExtIncludes.h"

#include <BLEPeripheral.h>
#include <BLESerial.h>

#include "includes/RingBuffer.h"

namespace SerialUI {

class NRF52BLESerial : public Stream {
public:
	NRF52BLESerial();

    void begin(...);
    void poll();
    void end();
    void setLocalName(const char * nm);


    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);
    virtual size_t write(uint8_t byte);
    using Print::write;
    virtual operator bool();

private:
    BLESerial _bleser;
    RingBuffer<uint8_t> rx_buf;
    RingBuffer<uint8_t> tx_buf;
    void fillRXFromBLESerial();
    uint32_t last_fill_ms;



};


} /* namespace SerialUI */


extern SerialUI::NRF52BLESerial	SUIBLESerialDev;

#endif /* SERIALUI_SRC_INCLUDES_PLATFORM_NRF52_NRF52BLESERIAL_H_ */