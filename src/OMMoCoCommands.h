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
#define CMD_PC_NOOP              0x00u //0
#define CMD_PC_START             0x01u //1
#define CMD_PC_PAUSE             0x02u //2
#define CMD_PC_STOP              0x03u //3
#define CMD_PC_CAM_ENABLE        0x04u //4
#define CMD_PC_CAM_DISABLE       0x05u //5
#define CMD_PC_MOVE_MOTOR_SMPL   0x06ul //6
#define CMD_PC_SEND_MOTOR_HOME   0x07u //7
#define CMD_PC_EXPOSE_NOW        0x08u //8
#define CMD_PC_MOTOR_SLEEP       0x09u //9
#define CMD_PC_MOTOR_MAX_RATE    0x0Au //10
#define CMD_PC_MOTOR_MAX_SPEED   0x0Bu //11
//...
#define CMD_PC_DEBUG_LED         0x0Fu //15
#define CMD_PC_STOP_MOTOR_NOW    0x10u //16
#define CMD_PC_MICROSTEP_LVL     0x11u //17
#define CMD_PC_TIMING_MASTER     0x12u //18
#define CMD_PC_MOVE_MOTOR_CPLX   0x13u //19
#define CMD_PC_PLAN_INTER_TRAVEL 0x14u //20
#define CMD_PC_CLEAR_PLAN_TRAVEL 0x15u //21
#define CMD_PC_STATUS_REQ        100u

//#define CMD_PC_
//#define CMD_PC_


//command constants group 3 (2 bytes)
#define CMD_CD_INTERVAL        0x0101ul // 1_1
#define CMD_CD_EXPOSURE        0x0102ul // 1_2
#define CMD_CD_FOCUS           0x0103ul // 1_3
#define CMD_CD_MAX_SHOTS       0x0104ul // 1_4
#define CMD_CD_EXPOSURE_DLY    0x0105ul // 1_5
#define CMD_CD_FOCUS_WSHUTTER  0x0106ul // 1_6
#define CMD_CD_REPEAT_CYCLES   0x0107ul // 1_7

//command constants group 3 (2 bytes)
#define CMD_MD_STEPS_EACH       0x0201ul // 2_1
#define CMD_MD_NOOP             0x0202ul // 2_2
#define CMD_MD_DIRECTION        0x0203ul // 2_3
#define CMD_MD_MAX_TRAVEL_STEPS 0x0204ul // 2_4
#define CMD_MD_MOTOR_ENABLE     0x0205ul // 2_5
#define CMD_MD_MOTOR_DISABLE    0x0206ul // 2_6
#define CMD_MD_SET_HOME_HERE    0x0207ul // 2_7
#define CMD_MD_BACKSLASH_AMO    0x0208ul // 2_8
#define CMD_MD_EASING_MODE      0x0209ul // 2_9
#define CMD_MD_CONTINUOUS_MODE  0x020Aul // 2_10

#define htonl(A) ((((uint32_t)(A) & 0xff000000) >> 24) | \
(((uint32_t)(A) & 0x00ff0000) >> 8) | \
(((uint32_t)(A) & 0x0000ff00) << 8) | \
(((uint32_t)(A) & 0x000000ff) << 24))

#endif /* OMMOCOCOMMANDS_H_ */
