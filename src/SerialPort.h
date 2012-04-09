/*
 * SerialPort.h
 *
 *  Created on: 04.04.2012
 *      Author: perepelitsa
 */

#ifndef SERIALPORT_H_
#define SERIALPORT_H_

#define RTS_PIN         PB0
#define RTS_DDR         DDRB
#define RTS_PORT        PORTB

class SerialPort {
public:
	SerialPort();
	virtual ~SerialPort();

public:
	bool initPort( uint8_t ucPort, unsigned long ulBaudRate);
	void closePort( void );
	void enableRxTx( bool xRxEnable, bool xTxEnable );
	bool getByte( uint8_t * pucByte );
	bool putByte( uint8_t ucByte );

private:
	inline void RTS_INIT() { RTS_DDR |= _BV( RTS_PIN ); RTS_PORT &= ~( _BV( RTS_PIN ) ); }
	inline void RTS_HIGH() { RTS_PORT |= _BV( RTS_PIN ); }
	inline void RTS_LOW()  { RTS_PORT &= ~( _BV( RTS_PIN ) );}

};

#endif /* SERIALPORT_H_ */
