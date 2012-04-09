/*
 * OMMoCoTransiver.h
 *
 *  Created on: 05.04.2012
 *      Author: perepelitsa
 */

#include <avr/interrupt.h>

#include "OMMocobus.h"
#include "SerialPort.h"

#ifndef OMMOCOTRANSIVER_H_
#define OMMOCOTRANSIVER_H_

#define RESULT_OK 0


#define DISABLE( )   cli()
#define ENABLE( )    sei()

#define OM_PDU_SIZE_MAX     256     /*!< Maximum size of frame. */
#define OM_PDU_SIZE_MIN     6
#define OM_PDU_PDU_OFF      5
#define OM_PDU_SIZE_CRC     2
#define OM_PDU_ADDR_OFF     4
#define OM_PDU_FUNC_OFF     5

class OMMoCoTransiver {
	/* ----------------------- Type definitions ---------------------------------*/
	typedef enum
	{
	    STATE_RX_INIT,              /*!< Receiver is in initial state. */
	    STATE_RX_IDLE,              /*!< Receiver is in idle state. */
	    STATE_RX_RCV,               /*!< Frame is beeing received. */
	    STATE_RX_ERROR              /*!< If the frame is invalid. */
	} eMBRcvState;

	typedef enum
	{
	    STATE_TX_IDLE,              /*!< Transmitter is in idle state. */
	    STATE_TX_XMIT               /*!< Transmitter is in transfer state. */
	} eMBSndState;

	typedef enum
	{
	    OM_READY,                   /*!< Startup finished. */
	    OM_FRAME_RECEIVED,          /*!< Frame received. */
	    OM_EXECUTE,                 /*!< Execute function. */
	    OM_FRAME_SENT               /*!< Frame sent. */
	} eOMEventType;

	volatile eMBSndState eSndState;
	volatile eMBRcvState eRcvState;

    //shared buffer for receive/transmit
	volatile uint8_t  ucRTUBuf[OM_PDU_SIZE_MAX];

	volatile uint8_t *pucSndBufferCur;
	volatile unsigned short usSndBufferCount;

	volatile unsigned short usRcvBufferPos;

	SerialPort portSerial;

public:
	OMMoCoTransiver();
	virtual ~OMMoCoTransiver();

	char Init(uint8_t ucPort, uint8_t ucBusAddress, unsigned long ulBaudRate);
	void StartTransiver (void);
	void StopTransiver (void);
	char Receive(uint8_t * pucRcvAddress, uint8_t ** pucFrame, unsigned short * pusLength);
	char Send(uint8_t ucSlaveAddress, const uint8_t * pucFrame, unsigned short usLength);

	//synched events interface
	bool UpdateReceiveFSM( void );
	bool UpdateTransmitFSM( void );

protected:
	bool PortEventInit(void);
	bool PortEventGet(eOMEventType* event);
	bool PortEventPost(eOMEventType event);

	SerialPort* getPort() {return &portSerial;}

};

#endif /* OMMOCOTRANSIVER_H_ */
