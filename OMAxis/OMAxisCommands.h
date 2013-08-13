/*
 
 OMAxis
 
 OpenMoco MoCoBus Core Libraries 
 
 See www.dynamicperception.com for more information
 
 (c) 2012-2013 Dynamic Perception LLC
 
 Authors: Stanislav Pereplitsa, C. A. Church
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */


//==============================
// Core Protocol Codes
//==============================


#ifndef OMMOCOCOMMANDS_H_
#define OMMOCOCOMMANDS_H_


    // break byte for complex commands
const uint8_t OM_PDU_ESC   = 31;

    // program control
const uint8_t OM_PCODE_PC  = 2;

    // program control codes
const uint8_t CMD_PC_NOOP             = 0;
const uint8_t CMD_PC_START            = 1;
const uint8_t CMD_PC_PAUSE            = 2;
const uint8_t CMD_PC_STOP             = 3;
const uint8_t CMD_PC_CAM_ENABLE       = 4;
const uint8_t CMD_PC_CAM_DISABLE      = 5;
const uint8_t CMD_PC_MOVE_MOTOR_SMPL  = 6;
const uint8_t CMD_PC_SEND_MOTOR_HOME  = 7;
const uint8_t CMD_PC_EXPOSE_NOW       = 8;
const uint8_t CMD_PC_MOTOR_SLEEP      = 9;
const uint8_t CMD_PC_MOTOR_MAX_RATE   = 10;
const uint8_t CMD_PC_MOTOR_MAX_SPEED  = 11;
//...
const uint8_t CMD_PC_DEBUG_LED         = 15;
const uint8_t CMD_PC_STOP_MOTOR_NOW    = 16;
const uint8_t CMD_PC_MICROSTEP_LVL     = 17;
const uint8_t CMD_PC_TIMING_MASTER     = 18;
const uint8_t CMD_PC_MOVE_COMPLEX      = 19;
const uint8_t CMD_PC_PLAN              = 20;
const uint8_t CMD_PC_CLEAR_PLAN        = 21;
const uint8_t CMD_PC_MAX_RUN           = 22;
const uint8_t CMD_PC_NAME              = 23;
const uint8_t CMD_PC_COMLINE           = 24;

const uint8_t CMD_PC_STATUS_REQ        = 100;

    // status request codes

const uint8_t OM_STAT_FWVER     = 1;
const uint8_t OM_STAT_RUN       = 2;
const uint8_t OM_STAT_RUNTIME   = 3;
const uint8_t OM_STAT_CAMEN     = 4;
const uint8_t OM_STAT_SHOTS     = 5;
const uint8_t OM_STAT_INTERVAL  = 6;
const uint8_t OM_STAT_EXPTM     = 7;
const uint8_t OM_STAT_EXPOSING  = 8;
const uint8_t OM_STAT_MOTOREN   = 9;
const uint8_t OM_STAT_MOTORDIR  = 10;
const uint8_t OM_STAT_MOVED     = 11;
const uint8_t OM_STAT_HOMEDIST  = 12;
const uint8_t OM_STAT_MAXSTEPS  = 14;
const uint8_t OM_STAT_BACKLASH  = 16;
const uint8_t OM_STAT_STEPS     = 17;
const uint8_t OM_STAT_MASTER    = 22;

    // data setting

const uint8_t OM_PCODE_PDS = 3;

    // camera data sub-command
const uint8_t OM_PCODE_CAM    = 1;

const uint8_t SC_CAM_INTERVAL = 1;
const uint8_t SC_CAM_EXPOSURE = 2;
const uint8_t SC_CAM_FOCUS    = 3;
const uint8_t SC_CAM_MAXSHOT  = 4;
const uint8_t SC_CAM_DELAY    = 5;
const uint8_t SC_CAM_TIE      = 6;
const uint8_t SC_CAM_REPEAT   = 7;

    // motor data sub-command
const uint8_t OM_PCODE_MOTOR  = 2; 

const uint8_t SC_MOT_STEPS    = 1;
const uint8_t SC_MOT_DIR      = 3;
const uint8_t SC_MOT_MAXSTEP  = 4;
const uint8_t SC_MOT_ENABLE   = 5;
const uint8_t SC_MOT_DISABLE  = 6;
const uint8_t SC_MOT_SETHOME  = 7;
const uint8_t SC_MOT_BACKLASH = 8;
const uint8_t SC_MOT_CONTSPD  = 9;
const uint8_t SC_MOT_EASE     = 10;
const uint8_t SC_MOT_CONTEN   = 11;
const uint8_t SC_MOT_DELAY    = 12;



#endif /* OMMOCOCOMMANDS_H_ */
