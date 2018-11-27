#
/*
 *    Copyright (C) 2012 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the DAB library
 *    DAB library is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DAB library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DAB library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __RTLSDR_HANDLER__
#define	__RTLSDR_HANDLER__

#include	"ringbuffer.h"
#include	"device-handler.h"
#include	<thread>
class	dll_driver;
//
//	This class is a simple wrapper around the
//	rtlsdr library that is read is as dll
//	It does not do any processing
class	rtlsdrHandler: public deviceHandler {
public:
			rtlsdrHandler	(int32_t	frequency,
	                                 int16_t	ppmCorrection,
	                                 int16_t	gain,
	                                 bool		autogain,
	                                 uint16_t	deviceIndex = 0);
			~rtlsdrHandler	(void);
//	interface to the reader
	bool		restartReader	(int32_t);
	void		stopReader	(void);
	int32_t		getSamples	(std::complex<float> *, int32_t);
	int32_t		Samples		(void);
	void		resetBuffer	(void);
	int16_t		maxGain		(void);
	std::string	deviceName	(void);
	int		whoamI		(void);
//
//	These need to be visible for the separate usb handling thread
	RingBuffer<uint8_t>	*_I_Buffer;
	struct rtlsdr_dev	*device;
	int32_t		sampleCounter;
private:
	int32_t		inputRate;
	int32_t		deviceCount;
	uint16_t	deviceIndex;
	bool		autogain;
	int16_t		ppmCorrection;
	std::thread     workerHandle;
	int		*gains;
	int16_t		gainsCount;
	int		gain;
	bool		running;
	bool		open;
	int		frequency;
};
#endif

