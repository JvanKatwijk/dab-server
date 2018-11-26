
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
 *	recoding, taking parts and extending for the airspyHandler interface
 *	for the SDR-J-DAB receiver.
 *	jan van Katwijk
 *	Lazy Chair Computing
 */

#ifdef	__MINGW32__
#define	GETPROCADDRESS	GetProcAddress
#else
#define	GETPROCADDRESS	dlsym
#endif

#include "airspy-handler.h"

static
const	int	EXTIO_NS	=  8192;
static
const	int	EXTIO_BASE_TYPE_SIZE = sizeof (float);

static
std::complex<float> cmul (std::complex<float> x, float y) {
	return std::complex<float> (real (x) * y, imag (x) * y);
}

	airspyHandler::airspyHandler (int32_t	frequency,
	                              int16_t	ppmCorrection,
	                              int16_t	theGain) {
int	result, i;
int	distance	= 10000000;
uint32_t myBuffer [20];
uint32_t samplerate_count;

	this	-> frequency	= frequency;
	this	-> ppmCorrection = ppmCorrection;
//
	device			= 0;
	serialNumber		= 0;
	theBuffer		= NULL;
	strcpy (serial,"");
	result = airspy_init ();
	if (result != AIRSPY_SUCCESS) {
	   throw (21);
	}
	   
	result = airspy_open (&device);
	if (result != AIRSPY_SUCCESS) {
	   throw (43);
	}

	(void) airspy_set_sample_type (device, AIRSPY_SAMPLE_INT16_IQ);
	(void) airspy_get_samplerates (device, &samplerate_count, 0);
	(void) airspy_get_samplerates (device, myBuffer, samplerate_count);

	selectedRate	= 0;
	for (i = 0; i < (int)samplerate_count; i ++) {
	   if (abs ((int)myBuffer [i] - 2048000) < distance) {
	      distance	= abs ((int)myBuffer [i] - 2048000);
	      selectedRate = myBuffer [i];
	   }
	}

	if (selectedRate == 0) {
	   throw (44);
	}
	else
	   fprintf (stderr, "selected samplerate = %d\n", selectedRate);

	result = airspy_set_samplerate (device, selectedRate);
	if (result != AIRSPY_SUCCESS) {
	   throw (45);
	}

//	The sizes of the mapTable and the convTable are
//	predefined and follow from the input and output rate
//	(selectedRate / 1000) vs (2048000 / 1000)
	convBufferSize		= selectedRate / 1000;
	for (i = 0; i < 2048; i ++) {
	   float inVal	= float (selectedRate / 1000);
	   mapTable_int [i] =  int (floor (i * (inVal / 2048.0)));
	   mapTable_float [i] = i * (inVal / 2048.0) - mapTable_int [i];
	}

	convIndex		= 0;
	convBuffer		= new std::complex<float> [convBufferSize + 1];

	theBuffer		=
	               new RingBuffer<std::complex<float>> (512 *1024);
	running		= false;
//
//	Here we set the gain and frequency
	(void)airspy_set_freq (device, frequency);
	(void)airspy_set_sensitivity_gain (device, theGain);
}

	airspyHandler::~airspyHandler (void) {
	if (device) {
	   int result = airspy_stop_rx (device);
	   result = airspy_close (device);
	}

	airspy_exit ();
	if (theBuffer != NULL)
	   delete theBuffer;
}

bool	airspyHandler::restartReader	(int32_t newFrequency) {
int	result;
int32_t	bufSize	= EXTIO_NS * EXTIO_BASE_TYPE_SIZE * 2;

	if (running)
	   return true;

	theBuffer	-> FlushRingBuffer ();
	result = airspy_set_sample_type (device, AIRSPY_SAMPLE_INT16_IQ);
	result = airspy_set_freq (device, newFrequency);
	result = airspy_set_sensitivity_gain (device, gain);
	
	result = airspy_start_rx (device,
	            (airspy_sample_block_cb_fn)callback, this);
//
//	finally:
	buffer = new uint8_t [bufSize];
	bs_ = bufSize;
	bl_ = 0;
	running	= true;
 	if (airspy_is_streaming (device) == AIRSPY_TRUE) 
	   fprintf (stderr, "restarted\n");
	
	return true;
}

void	airspyHandler::stopReader (void) {
	if (!running)
	   return;
int result = airspy_stop_rx (device);

	delete [] buffer;
	bs_ = bl_ = 0 ;
	running	= false;
}
//
//	Directly copied from the airspy extio dll from Andrea Montefusco
int airspyHandler::callback (airspy_transfer* transfer) {
airspyHandler *p;

	if (!transfer)
	   return 0;		// should not happen

	uint32_t bytes_to_write = transfer -> sample_count * sizeof (int16_t) * 2; 
	p = static_cast<airspyHandler *> (transfer -> ctx);
	uint8_t *pt_rx_buffer   = (uint8_t *)transfer -> samples;
	
	while (bytes_to_write > 0) {
	   int spaceleft = p -> bs_ - p -> bl_ ;
	   int to_copy = std::min ((int)spaceleft, (int)bytes_to_write);
	   ::memcpy (p -> buffer + p -> bl_, pt_rx_buffer, to_copy);
	   bytes_to_write -= to_copy;
	   pt_rx_buffer   += to_copy;
//
//	   bs (i.e. buffersize) in bytes
	   if (p -> bl_ == p -> bs_) {
	      p -> data_available ((void *)p -> buffer, p -> bl_);
	      p -> bl_ = 0;
	   }
	   p -> bl_ += to_copy;
	}
	return 0;
}

//	called from AIRSPY data callback
//	this method is declared in airspyHandler class
//	The buffer received from hardware contains
//	32-bit floating point IQ samples (8 bytes per sample)
//
//	recoded for the sdr-j framework
//	2*2 = 4 bytes for sample, as per AirSpy USB data stream format
//	we do the rate conversion here, read in groups of 2 * 625 samples
//	and transform them into groups of 2 * 512 samples
int 	airspyHandler::data_available (void *buf, int buf_size) {	
int16_t	*sbuf	= (int16_t *)buf;
int nSamples	= buf_size / (sizeof (int16_t) * 2);
std::complex<float> temp [2048];
int32_t  i, j;

	for (i = 0; i < nSamples; i ++) {
	   convBuffer [convIndex ++] = std::complex<float> (sbuf [2 * i] / (float)2048,
	                                           sbuf [2 * i + 1] / (float)2048);
	   if (convIndex > convBufferSize) {
	      for (j = 0; j < 2048; j ++) {
	         int16_t  inpBase	= mapTable_int [j];
	         float    inpRatio	= mapTable_float [j];
	         temp [j]	= cmul (convBuffer [inpBase + 1], inpRatio) + 
	                          cmul (convBuffer [inpBase], 1 - inpRatio);
	      }

	      theBuffer	-> putDataIntoBuffer (temp, 2048);
//
//	shift the sample at the end to the beginning, it is needed
//	as the starting sample for the next time
	      convBuffer [0] = convBuffer [convBufferSize];
	      convIndex = 1;
	   }
	}
	return 0;
}
//
const char *airspyHandler::getSerial (void) {
airspy_read_partid_serialno_t read_partid_serialno;
int result = airspy_board_partid_serialno_read (device,
	                                          &read_partid_serialno);
	if (result != AIRSPY_SUCCESS) {
	   return "UNKNOWN";
	} else {
	   snprintf (serial, sizeof(serial), "%08X%08X", 
	             read_partid_serialno. serial_no [2],
	             read_partid_serialno. serial_no [3]);
	}
	return serial;
}
//
int32_t	airspyHandler::getSamples (std::complex<float> *v, int32_t size) {

	return theBuffer	-> getDataFromBuffer (v, size);
}

int32_t	airspyHandler::Samples	(void) {
	return theBuffer	-> GetRingBufferReadAvailable ();
}
//

std::string airspyHandler::deviceName (void) {
uint8_t bid;

	if (airspy_board_id_read (device, &bid) == AIRSPY_SUCCESS)
	   return std::string (airspy_board_id_name ((airspy_board_id)bid));
	else
	   return std::string ("UNKNOWN");
}
//
//
int	airspyHandler::whoamI		(void) {
	return S_AIRSPY;
}

void	airspyHandler::setGain		(int value) {
	(void)airspy_set_sensitivity_gain (device, value);
}

