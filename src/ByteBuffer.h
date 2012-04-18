/*
 * ByteBuffer.h
 *
 *  Created on: 18.04.2012
 *      Author: perepelitsa
 */

#ifndef BYTEBUFFER_H_
#define BYTEBUFFER_H_

#include <inttypes.h>
#include <string.h>

class ByteBuffer {
   uint8_t* buf;
   uint16_t rpos, wpos;
   uint16_t max_size;

public:
	ByteBuffer();
	virtual ~ByteBuffer();

    void assign  (uint8_t* buf, uint16_t max_size);

	uint8_t size() const {return rpos - wpos;};

	// writing raw data (command or response contents)
	void write(uint8_t);
	void write(unsigned int);
	void write(int);
	void write(unsigned long);
	void write(long);


    template <typename T> void append(T data) {
            unsigned int s = sizeof(data);
            if ((wpos + s) > max_size)
              return;
            memcpy(&buf[wpos], (uint8_t*)&data, s);
            wpos += s;
    }


	void setReadPos(uint8_t r) { rpos = r;}

	uint8_t getReadPos() const  {return rpos;}

	void setWritePos(uint8_t w) { wpos = w; }

	uint8_t getWritePos() const {return wpos;}


};

#endif /* BYTEBUFFER_H_ */
