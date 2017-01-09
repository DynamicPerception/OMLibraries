/*
 * ByteBuffer.cpp
 *
 *  Created on: 18.04.2012
 *      Author: perepelitsa
 */

#include "OMByteBuffer.h"

OMByteBuffer::OMByteBuffer() {

}

/**
 *
 * */
void OMByteBuffer::assign(uint8_t* in_buf, uint16_t in_size) {
    buf = in_buf;
    rpos = 0;
    wpos = 0;
    max_size = in_size;
}

OMByteBuffer::~OMByteBuffer() {
    // TODO Auto-generated destructor stub
}

