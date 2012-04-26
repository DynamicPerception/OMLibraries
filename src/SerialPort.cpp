/*
 * SerialPort.cpp
 *
 *  Created on: 04.04.2012
 *      Author: perepelitsa
 */

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
bool SerialPort::initPort(uint8_t ucPort, unsigned long ulBaudRate) {

	unsigned short UBRR_VALUE = UART_BAUD_CALC( ulBaudRate, F_CPU );

	// Set baud rate
	UBRR0H = (uint8_t) (UBRR_VALUE >> 8);
	UBRR0L = (uint8_t) UBRR_VALUE;

	uint8_t ucUCSRC = 0;
	ucUCSRC |= _BV( UCSZ0 ) | _BV( UCSZ1 );

	UCSR0C |= ucUCSRC;

	RTS_INIT();

	return true;
}

void SerialPort::closePort(void) {

}

void SerialPort::enableRxTx(bool xRxEnable, bool xTxEnable) {

	UCSR0B |= _BV( TXEN0 ) | _BV(TXCIE0);
	if (xRxEnable) {
		UCSR0B |= _BV( RXEN ) | _BV( RXCIE );
	} else {
		UCSR0B &= ~(_BV( RXEN ) | _BV( RXCIE ));
	}

	if (xTxEnable) {
		UCSR0B |= _BV( TXEN ) | _BV( UDRE );
		RTS_HIGH();
	} else {
		UCSR0B &= ~(_BV( UDRE ));
	}
}

/**
 *
 * */
bool SerialPort::getByte(uint8_t * pucByte) {
	*pucByte = UDR0;
	return true;
}

/**
 *
 * */
bool SerialPort::putByte(uint8_t ucByte) {
	UDR0 = ucByte;
	return true;
}
