/*
 * ActionScreen.h
 *
 *  Created on: 05.10.2012
 *      Author: perepelitsa
 */

#ifndef ACTIONSCREEN_H_
#define ACTIONSCREEN_H_

#include <inttypes.h>
#include "MenuContext.h"
#include "Commander.h"

#define ACTIONS_MAX 2

/**
 *
 * */
class ActionScreen {

	uint8_t moveDir;
	uint8_t markerPos;
	Commander& cmd;
	MenuContext& status;
	/*callbacks for special actions*/
	//typedef void (*actionCB)(uint8_t param);
	//actionCB actions[ACTIONS_MAX];
	//actionCB action;

public:
	ActionScreen(Commander & _cmd);

	/* */
	void openAction(const uint8_t idxProc, const uint8_t idxParam);
	/*  */
	void closeAction(bool closeFlag);

	/* */
	void Left(void);

	/*  */
	void Right(void);

    /* */
	bool isAction() const {return status.isParamEdit();}

	/* */
	uint8_t getMarkerPos() const {return markerPos;}

    /* */
    uint8_t getMoveDir() const {return moveDir;}
};

#endif /* ACTIONSCREEN_H_ */
