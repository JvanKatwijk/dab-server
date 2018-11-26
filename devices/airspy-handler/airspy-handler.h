
/**
 *  IW0HDV Extio
 *
 *  Copyright 2015 by Andrea Montefusco IW0HDV
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *	recoding and taking parts for the airspyRadio interface
 *	for the DAB library
 *	jan van Katwijk
 *	Lazy Chair Computing
 */
#ifndef __AIRSPY_HANDLER__
#define	__AIRSPY_HANDLER__

#include	"ringbuffer.h"
#include	"device-handler.h"
#include	<complex>

#ifdef  __MINGW32__
#include        "windows.h"
#else
#ifndef __FREEBSD__
#include        "alloca.h"
#endif
#include        "dlfcn.h"
typedef void    *HINSTANCE;
#endif

#ifndef	__MINGW32__
#include	"libairspy/airspy.h"
#else
#include	"airspy.h"
#endif

class airspyHandler: public deviceHandler {
public:
			airspyHandler		(int32_t, int16_t, int16_t);
			~airspyHandler		(void);
	bool		restartReader		(int32_t);
	void		stopReader		(void);
	int32_t		getSamples		(std::complex<float> *v,
	                                                     int32_t size);
	int32_t		Samples			(void);
	void		setGain			(int32_t);
	std::string	deviceName		(void);
	int		whoamI			(void);
private:
	int32_t		frequency;
	int16_t		ppmCorrection;
	int16_t		gain;
	bool		success;
	bool		running;
	int32_t		selectedRate;
	std::complex<float>	*convBuffer;
	int16_t		convBufferSize;
	int16_t		convIndex;
	int16_t		mapTable_int   [4 * 512];
	float		mapTable_float [4 * 512];
	RingBuffer<std::complex<float>> *theBuffer;
	int32_t		inputRate;
	struct airspy_device* device;
	uint64_t 	serialNumber;
	char		serial[128];
    // callback buffer	
	int 		bs_;
	uint8_t 	*buffer;
	int		bl_;
static
	int		callback(airspy_transfer_t *);
	int		data_available (void *buf, int buf_size);
const	char *		getSerial (void);
	int	open (void);
};

#endif
