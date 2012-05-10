/*
 * OMMoCoCommands.h
 *
 *  Created on: 26.04.2012
 *      Author: perepelitsa
 */

#ifndef OMMOCOCOMMANDS_H_
#define OMMOCOCOMMANDS_H_

#define OM_PCODE_PC   2
#define OM_PCODE_PDS  3
#define OM_PDU_ESC    0x3E
//command constants group 2 (1 bytes)
#define CMD_PC_NOOP              0x00 //0
#define CMD_PC_START             0x01 //1
#define CMD_PC_PAUSE             0x02 //2
#define CMD_PC_STOP              0x03 //3
#define CMD_PC_CAM_ENABLE        0x04 //4
#define CMD_PC_CAM_DISABLE       0x05 //5
#define CMD_PC_MOVE_MOTOR_SMPL   0x06 //6
#define CMD_PC_SEND_MOTOR_HOME   0x07 //7
#define CMD_PC_EXPOSE_NOW        0x08 //8
#define CMD_PC_MOTOR_SLEEP       0x09 //9
#define CMD_PC_MOTOR_MAX_RATE    0x0A //10
#define CMD_PC_MOTOR_MAX_SPEED   0x0B //11
//...
#define CMD_PC_DEBUG_LED         0x0F //15
#define CMD_PC_STOP_MOTOR_NOW    0x10 //16
#define CMD_PC_MICROSTEP_LVL     0x11 //17
#define CMD_PC_TIMING_MASTER     0x12 //18
#define CMD_PC_MOVE_MOTOR_CPLX   0x13 //19
#define CMD_PC_PLAN_INTER_TRAVEL 0x14 //20
#define CMD_PC_CLEAR_PLAN_TRAVEL 0x15 //21
#define CMD_PC_STATUS_REQ        100

//#define CMD_PC_
//#define CMD_PC_


//command constants group 3 (2 bytes)
#define CMD_CD_INTERVAL        0x0101 // 1_1
#define CMD_CD_EXPOSURE        0x0102 // 1_2
#define CMD_CD_FOCUS           0x0103 // 1_3
#define CMD_CD_MAX_SHOTS           0x0104 // 1_4
#define CMD_CD_EXPOSURE_DLY    0x0105 // 1_5
#define CMD_CD_FOCUS_WSHUTTER  0x0106 // 1_6
#define CMD_CD_REPEAT_CYCLES   0x0107 // 1_7

//command constants group 3 (2 bytes)
#define CMD_MD_STEPS_EACH       0x0201 // 2_1
#define CMD_MD_NOOP             0x0202 // 2_2
#define CMD_MD_DIRECTION        0x0203 // 2_3
#define CMD_MD_MAX_TRAVEL_STEPS 0x0204 // 2_4
#define CMD_MD_MOTOR_ENABLE     0x0205 // 2_5
#define CMD_MD_MOTOR_DISABLE    0x0206 // 2_6
#define CMD_MD_SET_HOME_HERE    0x0207 // 2_7
#define CMD_MD_BACKSLASH_AMO    0x0208 // 2_8
#define CMD_MD_EASING_MODE      0x0209 // 2_9
#define CMD_MD_CONTINUOUS_MODE  0x020A // 2_10

#define htonl(A) ((((uint32_t)(A) & 0xff000000) >> 24) | \
(((uint32_t)(A) & 0x00ff0000) >> 8) | \
(((uint32_t)(A) & 0x0000ff00) << 8) | \
(((uint32_t)(A) & 0x000000ff) << 24))

#endif /* OMMOCOCOMMANDS_H_ */
