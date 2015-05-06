#include <inttypes.h>
 // must load before wconstants to prevent issues
#include <math.h>
#include <Arduino.h>
#include <OMMotorFunctions.h>



 // DEFAULT PIN ASSIGNMENTS


/*  ================================
    Motor 1 Pins
    ===============================*/
#ifndef OM_MOT1_DSTEP
    #define OM_MOT1_DSTEP	45
#endif
#ifndef OM_MOT1_DDIR
    #define OM_MOT1_DDIR	16
#endif
#ifndef OM_MOT1_DSLP
    #define OM_MOT1_DSLP	32
#endif
#ifndef OM_MOT1_DMS1
    #define OM_MOT1_DMS1	17
#endif
#ifndef OM_MOT1_DMS2
    #define OM_MOT1_DMS2	18
#endif
#ifndef OM_MOT1_DMS3
    #define OM_MOT1_DMS3	19
#endif
#ifndef OM_MOT1_STPREG
    #define OM_MOT1_STPREG     PORTF
#endif
#ifndef OM_MOT1_STPFLAG
    #define OM_MOT1_STPFLAG      PORTF5
#endif

/*  ================================
    Motor 2 Pins
    ===============================*/
#ifndef OM_MOT2_DSTEP
    #define OM_MOT2_DSTEP	46
#endif
#ifndef OM_MOT2_DDIR
    #define OM_MOT2_DDIR	21
#endif
#ifndef OM_MOT2_DSLP
    #define OM_MOT2_DSLP	20
#endif
#ifndef OM_MOT2_DMS1
    #define OM_MOT2_DMS1	29
#endif
#ifndef OM_MOT2_DMS2
    #define OM_MOT2_DMS2	30
#endif
#ifndef OM_MOT2_DMS3
    #define OM_MOT2_DMS3	31
#endif
#ifndef OM_MOT2_STPREG
    #define OM_MOT2_STPREG     PORTF
#endif
#ifndef OM_MOT2_STPFLAG
    #define OM_MOT2_STPFLAG      PORTF6
#endif

/*  ================================
    Motor 3 Pins
    ===============================*/
#ifndef OM_MOT3_DSTEP
    #define OM_MOT3_DSTEP	47
#endif
#ifndef OM_MOT3_DDIR
    #define OM_MOT3_DDIR	34
#endif
#ifndef OM_MOT3_DSLP
    #define OM_MOT3_DSLP	33
#endif
#ifndef OM_MOT3_DMS1
    #define OM_MOT3_DMS1	15
#endif
#ifndef OM_MOT3_DMS2
    #define OM_MOT3_DMS2	43
#endif
#ifndef OM_MOT3_DMS3
    #define OM_MOT3_DMS3	44
#endif
#ifndef OM_MOT3_STPREG
    #define OM_MOT3_STPREG     PORTF
#endif
#ifndef OM_MOT3_STPFLAG
    #define OM_MOT3_STPFLAG      PORTF7
#endif


#define OM_MOT_SSTATE	HIGH
#define OM_MOT_SAFE	10

#define OM_MOT_DONE 	1
#define OM_MOT_MOVING 	2
#define OM_MOT_BEGIN	3

#define OM_MOT_LINEAR 	1
#define OM_MOT_QUAD		2
#define OM_MOT_QUADINV	3
