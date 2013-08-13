/*
  OMMoCoPinsAT90.h
  

 Created by Chris Church on 8/9/13.
  Copyright 2013 dynamic perception llc. 


This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.All rights reserved.
 
*/

#ifndef _OMMoCoPinsAT90_h
#define _OMMoCoPinsAT90_h


#ifndef OMB_DEPIN
    #define OMB_DEPIN 28
#endif
#ifndef OMB_DEREG
    #define OMB_DEREG   PORTD
#endif
#ifndef OMB_DEPFLAG
    #define OMB_DEPFLAG PORTD4
#endif

#ifndef OMB_SRDREG
    #define OMB_SRDREG  UDR1
#endif
#ifndef OMB_SRRFLAG
    #define OMB_SRRFLAG UDRE1
#endif
#ifndef OMB_SRREG
    #define OMB_SRSREG   UCSR1A
#endif
#ifndef OMB_SRCLRFLAG
    #define OMB_SRTXFLAG   TXC1
#endif

#endif // _OMMoCoPinsAT90_h
