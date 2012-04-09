/*
 * OMMoCoTransiver.cpp
 *
 *  Created on: 05.04.2012
 *      Author: perepelitsa
 */

#include "OMMoCoTransiver.h"

//ToDo implement 3 funcs
static char PortTimersInit(unsigned short) {
    return 0;
}

static void PortTimersEnable () {

}

static void PortTimersDisable(  ){


}


OMMoCoTransiver::OMMoCoTransiver() {
}

OMMoCoTransiver::~OMMoCoTransiver() {
}

/**
 *
 * */
char OMMoCoTransiver::Init( uint8_t ucPort, uint8_t ucBusAddress, unsigned long ulBaudRate )
{
    char            eStatus = OM_NOERR;
    unsigned short  usTimerT35_50us;


    DISABLE(  );

    if( portSerial.initPort( ucPort, ulBaudRate ) != RESULT_OK )
    {
        eStatus = OM_HARDERR;
    }
    else
    {
        /* If baudrate > 19200 then we should use the fixed timer values
         * t35 = 1750us. Otherwise t35 must be 3.5 times the character time.
         */
        if( ulBaudRate > 19200 )
        {
            usTimerT35_50us = 35;       /* 1800us. */
        }
        else
        {
            /* The timer reload value for a character is given by:
             *
             * ChTimeValue = Ticks_per_1s / ( Baudrate / 11 )
             *             = 11 * Ticks_per_1s / Baudrate
             *             = 220000 / Baudrate
             * The reload for t3.5 is 1.5 times this value and similary
             * for t3.5.
             */
            usTimerT35_50us = ( 7UL * 220000UL ) / ( 2UL * ulBaudRate );
        }
        if( PortTimersInit( ( unsigned short ) usTimerT35_50us ) != RESULT_OK )
        {
            eStatus = OM_HARDERR;
        }
    }
    ENABLE(  );

    return eStatus;
}

/**
 *
 * */
void OMMoCoTransiver::StartTransiver( void )
{
    DISABLE(  );
    /* Initially the receiver is in the state STATE_RX_INIT. we start
     * the timer and if no character is received within t3.5 we change
     * to STATE_RX_IDLE. This makes sure that we delay startup of the
     * OM protocol stack until the bus is free.
     */
    eRcvState = STATE_RX_INIT;
    portSerial.enableRxTx( true, false );
    PortTimersEnable(  );

    ENABLE(  );
}

/**
 *
 * */
void OMMoCoTransiver::StopTransiver( void )
{
    DISABLE(  );
    portSerial.enableRxTx( false, false );
    PortTimersDisable(  );
    ENABLE(  );
}

/**
 * Should first function called to set begin frame pointer.
 * Frame pointer should have some free bytes in buffer before pointing address.
 * */
char OMMoCoTransiver::Receive( uint8_t * pucRcvAddress, uint8_t ** pucFrame, unsigned short * pusLength )
{
    bool    xFrameReceived = false;
    char    eStatus = OM_NOERR;

    DISABLE(  );
    //assert( usRcvBufferPos < OM_PDU_SIZE_MAX );

    /* Length check (later check for CRC)*/
    if( ( usRcvBufferPos >= OM_PDU_SIZE_MIN ))
        //&& ( usMBCRC16( ( UCHAR * ) ucRTUBuf, usRcvBufferPos ) == 0 ) )
    {
        /* Save the address field. All frames are passed to the upper layed
         * and the decision if a frame is used is done there.
         */
        *pucRcvAddress = ucRTUBuf[OM_PDU_ADDR_OFF];

        /* Total length of PDU is OM-PDU minus
         * size of address field (and later CRC checksum).
         */
        *pusLength = ( unsigned short )( usRcvBufferPos - OM_PDU_PDU_OFF - OM_PDU_SIZE_CRC );

        /* Return the start of the PDU to the caller. */
        *pucFrame = ( uint8_t * ) & ucRTUBuf[OM_PDU_PDU_OFF];
        xFrameReceived = true;
    }
    else
    {
        eStatus = OM_EIO;
    }

    ENABLE(  );
    return eStatus;
}

/**
 *
 * */
char OMMoCoTransiver::Send( uint8_t ucSlaveAddress, const uint8_t * pucFrame, unsigned short usLength )
{
    char    eStatus = OM_NOERR;
    unsigned short usCRC16;

    DISABLE(  );

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if( eRcvState == STATE_RX_IDLE )
    {
        /* First byte before the PDU is the slave address. */
        pucSndBufferCur = ( uint8_t * ) pucFrame - 1;
        usSndBufferCount = 1;

        /* Now copy the OM-PDU into the OM-Serial-Line-PDU. */
        pucSndBufferCur[OM_PDU_ADDR_OFF] = ucSlaveAddress;
        usSndBufferCount += usLength;

        /* Calculate CRC16 checksum for OM-Serial-Line-PDU. */
        //usCRC16 = usMBCRC16( ( uint8_t * ) pucSndBufferCur, usSndBufferCount );
        //ucRTUBuf[usSndBufferCount++] = ( uint8_t )( usCRC16 & 0xFF );
        //ucRTUBuf[usSndBufferCount++] = ( uint8_t )( usCRC16 >> 8 );

        /* Activate the transmitter. */
        eSndState = STATE_TX_XMIT;
        portSerial.enableRxTx( false, true );
    }
    else
    {
        eStatus = OM_EIO;
    }
    ENABLE(  );
    return eStatus;
}

/**
 *
 * */
bool OMMoCoTransiver::UpdateReceiveFSM( void )
{
    bool            xTaskNeedSwitch = false;
    uint8_t         ucByte;

    if ( eSndState != STATE_TX_IDLE ){
    	return xTaskNeedSwitch;
    }

    /* Always read the character. */
    ( void )portSerial.getByte( ( uint8_t * ) & ucByte );

    switch ( eRcvState )
    {
        /* If we have received a character in the init state we have to
         * wait until the frame is finished.
         */
    case STATE_RX_INIT:
        PortTimersEnable(  );
        break;

        /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         */
    case STATE_RX_ERROR:
        PortTimersEnable(  );
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVCE.
         */
    case STATE_RX_IDLE:
        usRcvBufferPos = 0;
        ucRTUBuf[usRcvBufferPos++] = ucByte;
        eRcvState = STATE_RX_RCV;

        /* Enable t3.5 timers. */
        PortTimersEnable(  );
        break;

        /* We are currently receiving a frame. Reset the timer after
         * every character received. If more than the maximum possible
         * number of bytes in a OM protocol frame is received the frame is
         * ignored.
         */
    case STATE_RX_RCV:
        if( usRcvBufferPos < OM_PDU_SIZE_MAX )
        {
            ucRTUBuf[usRcvBufferPos++] = ucByte;
        }
        else
        {
            eRcvState = STATE_RX_ERROR;
        }
        PortTimersEnable(  );
        break;
    }
    return xTaskNeedSwitch;
}

/**
 *
 * */
bool OMMoCoTransiver::UpdateTransmitFSM( void )
{
    bool xNeedPoll = false;

   if ( eRcvState != STATE_RX_IDLE ){
	   return xNeedPoll;
   }

    switch ( eSndState )
    {
        /* We should not get a transmitter event if the transmitter is in
         * idle state.  */
    case STATE_TX_IDLE:
        /* enable receiver/disable transmitter. */
        portSerial.enableRxTx( true, false );
        break;

    case STATE_TX_XMIT:
        /* check if we are finished. */
        if( usSndBufferCount != 0 )
        {
            portSerial.putByte( ( uint8_t )*pucSndBufferCur );
            pucSndBufferCur++;  /* next byte in sendbuffer. */
            usSndBufferCount--;
        }
        else
        {
            xNeedPoll = PortEventPost( OM_FRAME_SENT );
            /* Disable transmitter. This prevents another transmit buffer
             * empty interrupt. */
            portSerial.enableRxTx( true, false );
            eSndState = STATE_TX_IDLE;
        }
        break;
    }

    return xNeedPoll;
}


