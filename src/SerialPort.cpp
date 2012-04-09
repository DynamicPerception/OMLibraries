/*
 * SerialPort.cpp
 *
 *  Created on: 04.04.2012
 *      Author: perepelitsa
 */
#include <avr/io.h>
#include "SerialPort.h"

#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) \
    ( ( F_OSC ) / ( ( UART_BAUD_RATE ) * 16UL ) - 1 )

SerialPort::SerialPort() {


}

SerialPort::~SerialPort() {

}

/**
 * Make changes for another AVR
 * */
bool SerialPort::initPort( uint8_t ucPort, unsigned long ulBaudRate) {

	UBRRH = UART_BAUD_CALC( ulBaudRate, F_CPU );

	uint8_t ucUCSRC = 0;
	ucUCSRC |= _BV( UCSZ0 ) | _BV( UCSZ1 );

	UCSRC |= ucUCSRC;

	RTS_INIT();

	return true;
}

void SerialPort::closePort( void ){

}

void SerialPort::enableRxTx( bool xRxEnable, bool xTxEnable ){

    UCSRB |= _BV( TXEN ) | _BV(TXCIE);
    if( xRxEnable )
    {
        UCSRB |= _BV( RXEN ) | _BV( RXCIE );
    }
    else
    {
        UCSRB &= ~( _BV( RXEN ) | _BV( RXCIE ) );
    }

    if( xTxEnable )
    {
        UCSRB |= _BV( TXEN ) | _BV( UDRE );
        RTS_HIGH();
    }
    else
    {
        UCSRB &= ~( _BV( UDRE ) );
    }
}

/**
 *
 * */
bool SerialPort::getByte( uint8_t * pucByte ){
	 *pucByte = UDR;
	  return true;
}

/**
 *
 * */
bool SerialPort::putByte( uint8_t ucByte ){
	 UDR = ucByte;
	 return true;
}
