/*
 * main.cpp
 *
 *  Created on: 27.04.2012
 *      Author: perepelitsa
 */
#include "Arduino.h"
#include "./OMMoCoMaster/SimpleDevMaster.h"

extern HardwareSerial Serial1;

int main (void) {

    Serial1.begin(9600);

	SimpleDevMaster master(Serial1, 0);
    master.cdExposure(345567);


  return 0;
}

