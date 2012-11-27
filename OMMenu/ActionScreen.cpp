/*
 * ActionScreen.cpp
 *
 *  Created on: 05.10.2012
 *      Author: perepelitsa
 */
#include <string.h>
#include "ActionScreen.h"

/**
 *
 * */
ActionScreen::ActionScreen(Commander& _cmd)
: cmd(_cmd), status(_cmd.GetWrContext())
{
	markerPos = 0;
	moveDir = 0;

}

/**
 *
 **/
void ActionScreen::openAction(const uint8_t idxProc, const uint8_t idxParam)
{
	status.openParameter(idxParam);
}

/**
 *
 **/
void ActionScreen::closeAction(bool closeFlag)
{
  status.closeParameter(closeFlag);
  if (closeFlag) {
	moveDir = 2;
	cmd.StepMotor(moveDir);
  }
}

/**
 *
 **/
void ActionScreen::Left(void)
{
	//uint32_t param = status.readLiveParameter(curIdx);
	markerPos--;
	markerPos = (markerPos & 0x0F);
	moveDir = 0;
	cmd.StepMotor(moveDir);
	status.setContextBits(TRANSFER_DISPLAY_ENABLE);
}

/**
 *
 **/
void ActionScreen::Right(void)
{
	//uint32_t param = status.readLiveParameter(curIdx);
	markerPos++;
	markerPos = (markerPos & 0x0F);
	moveDir = 1;
	cmd.StepMotor(moveDir);
	status.setContextBits(TRANSFER_DISPLAY_ENABLE);
}


