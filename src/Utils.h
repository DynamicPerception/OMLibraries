/*
 * Utils.h
 *
 *  Created on: 05.04.2012
 *      Author: perepelitsa
 */

#ifndef UTILS_H_
#define UTILS_H_

class Utils {
public:
	static int ntoi(uint8_t*);
	static unsigned int ntoui(uint8_t*);
	static long ntol(uint8_t*);
	static unsigned long ntoul(uint8_t*);
	static float ntof(uint8_t*);
};

#endif /* UTILS_H_ */
