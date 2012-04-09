/*
 * OMMoCoNode.cpp
 *
 *  Created on: 05.04.2012
 *      Author: perepelitsa
 */

#include "OMMoCoNode.h"

static void process(uint8_t function, uint8_t ** pucFrame, unsigned short * pusLength) {


}

//declarative constructor easier to implement
OMMoCoNode::OMMoCoNode() {} //(const SerialPort& inPort)
   //: port (inPort)


OMMoCoNode::~OMMoCoNode() {
}

char OMMoCoNode::Init(uint8_t ucSlaveAddress, unsigned long ulBaudRate){
	    eOMState = STATE_NOT_INITIALIZED;
	    char    eStatus = OM_NOERR;

	    /* check preconditions */
	    if (( ucSlaveAddress < OM_ADDRESS_MIN ) || ( ucSlaveAddress > OM_ADDRESS_MAX ))
	    {
	        eStatus = OM_INVALID;
	    }
	    else
	    {
	        ucMBAddress = ucSlaveAddress;
	        eStatus = OMMoCoTransiver::Init( 0, ucMBAddress, ulBaudRate);

	        if( eStatus == OM_NOERR )
	        {
	            if( !PortEventInit(  ) )
	            {
	                /* port dependent event module initialization failed. */
	                eStatus = OM_HARDERR;
	            }
	            else
	            {
	                eOMState = STATE_DISABLED;
	            }
	        }
	    }
	    return eStatus;
}

/**
 *
 * */
char OMMoCoNode::Close( void )
{
    char    eStatus = OM_NOERR;

    if( eOMState == STATE_DISABLED )
    {
       getPort()->closePort();
    }
    else
    {
        eStatus = OM_INVALID;
    }
    return eStatus;
}

/**
 *
 * */
char OMMoCoNode::Enable( void )
{
    char    eStatus = OM_NOERR;

    if( eOMState == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        StartTransiver();
        eOMState = STATE_ENABLED;
    }
    else
    {
        eStatus = OM_INVALID;
    }
    return eStatus;
}

/**
 *
 * */
char OMMoCoNode::Disable( void )
{
    char    eStatus;

    if( eOMState == STATE_ENABLED )
    {
        StopTransiver();
        eOMState = STATE_DISABLED;
        eStatus = OM_NOERR;
    }
    else if( eOMState == STATE_DISABLED )
    {
        eStatus = OM_NOERR;
    }
    else
    {
        eStatus = OM_INVALID;
    }
    return eStatus;
}

/**
 * All in one function
 * */
char OMMoCoNode::Poll( void )
{
    int     i;
    char    eStatus = OM_NOERR;
    eOMEventType    eEvent;

    /* Check if the protocol stack is ready. */
    if( eOMState != STATE_ENABLED )
    {
        return OM_INVALID;
    }

    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if( PortEventGet( &eEvent ) == true )
    {
        switch ( eEvent )
        {
        case OM_READY:
            break;

        case OM_FRAME_RECEIVED:
            eStatus = Receive( &ucRcvAddress, &ucOMFrame, &usLength );
            if( eStatus == OM_NOERR )
            {
                /* Check if the frame is for us. If not ignore the frame. */
                if( ( ucRcvAddress == ucMBAddress ) )
                {
                    ( void )PortEventPost( OM_EXECUTE );
                }
            }
            break;

        case OM_EXECUTE:
            ucFunctionCode = ucOMFrame[OM_PDU_FUNC_OFF];

            process(ucFunctionCode, &ucOMFrame, &usLength);

            eStatus = Send( ucMBAddress, ucOMFrame, usLength );

            break;

        case OM_FRAME_SENT:
            break;
        }
    }
    return OM_NOERR;
}

