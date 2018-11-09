#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the DAB-library
 *    DAB-library is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DAB-library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DAB-library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	Once the bits are "in", interpretation and manipulation
 *	should reconstruct the data blocks.
 *	Ofdm_decoder is called once every Ts samples, and
 *	its invocation results in 2 * Tu bits
 */
#include	"ofdm-decoder.h"
#include	"phasetable.h"
#include	"freq-interleaver.h"
#include	"dab-params.h"
#include	"fft_handler.h"

/**
  *	\brief ofdmDecoder
  *	The class ofdmDecoder is - when implemented in a separate thread -
  *	taking the data from the ofdmProcessor class in, and
  *	will extract the Tu samples, do an FFT and extract the
  *	carriers and map them on (soft) bits
  */
	ofdmDecoder::ofdmDecoder	(uint8_t	dabMode,
                                         RingBuffer<std::complex<float>> *iqBuffer):
	                                     params (dabMode),
	                                     my_fftHandler (dabMode),
	                                     myMapper    (dabMode) {

        this    -> iqBuffer             = iqBuffer;
	this	-> T_s			= params. get_T_s ();
	this	-> T_u			= params. get_T_u ();
	this	-> nrBlocks		= params. get_L ();
	this	-> carriers		= params. get_carriers ();
	this	-> T_g			= T_s - T_u;
	fft_buffer			= my_fftHandler. getVector ();
	phaseReference. resize (T_u);
//
	current_snr			= 0;	
	cnt				= 0;
}

	ofdmDecoder::~ofdmDecoder	(void) {
}

void	ofdmDecoder::processBlock_0 (std::complex<float> *buffer) {
	memcpy (fft_buffer, buffer,
	                      T_u * sizeof (std::complex<float>));

	my_fftHandler. do_FFT (fft_handler::fftForward);
/**
  *	The SNR is determined by looking at a segment of bins
  *	within the signal region and bits outside.
  *	It is just an indication
  */
	current_snr	= 0.7 * current_snr + 0.3 * get_snr (fft_buffer);
/**
  *	we are now in the frequency domain, and we keep the carriers
  *	as coming from the FFT as phase reference.
  */
	memcpy (phaseReference. data (),
	               fft_buffer, T_u * sizeof (std::complex<float>));
}

void	ofdmDecoder::decode (std::complex<float> *buffer,
	                             int32_t blkno, int16_t *ibits) {
int16_t	i;
      memcpy (fft_buffer, &(buffer[T_g]),
                                       T_u * sizeof (std::complex<float>));
std::complex<float> conjVector [T_u];

fftlabel:
/**
  *	first step: do the FFT
  */
	my_fftHandler. do_FFT (fft_handler::fftForward);
/**
  *	a little optimization: we do not interchange the
  *	positive/negative frequencies to their right positions.
  *	The de-interleaving understands this
  */
toBitsLabel:
/**
  *	Note that from here on, we are only interested in the
  *	"carriers" useful carriers of the FFT output
  */
	for (i = 0; i < carriers; i ++) {
	   int16_t	index	= myMapper. mapIn (i);
	   if (index < 0) 
	      index += T_u;
/**
  *	decoding is computing the phase difference between
  *	carriers with the same index in subsequent blocks.
  *	The carrier of a block is the reference for the carrier
  *	on the same position in the next block
  */
	   std::complex<float>	r1 = fft_buffer [index] * conj (phaseReference [index]);
           conjVector [index] = r1;
//	The viterbi decoder expects values in the range 0 .. 255,
//	we present values -127 .. 127 (easy with depuncturing)
	   float ab1		= jan_abs (r1);
	   ibits [i]		= - real (r1) / ab1 * 127.0;
	   ibits [carriers + i] = - imag (r1) / ab1 * 127.0;
	}

	memcpy (phaseReference. data (),
	          fft_buffer, T_u * sizeof (std::complex<float>));
//	From time to time we show the constellation of block 2.
//	Note that we do it in two steps since the
//	fftbuffer contained low and high at the ends
//	and we maintain that format
	if ((blkno == 2) && (iqBuffer != nullptr)) {
	   if (++cnt > 7) {
	      iqBuffer	-> putDataIntoBuffer (&conjVector [0],
	                                      carriers / 2);
	      iqBuffer	-> putDataIntoBuffer (&conjVector [T_u - 1 - carriers / 2],
	                                      carriers / 2);
	      cnt = 0;
	   }
	}
}
//
/**
  *	for the snr we have a full T_u wide vector, with in the middle
  *	K carriers.
  *	Just get the strength from the selected carriers compared
  *	to the strength of the carriers outside that region
  */
int16_t	ofdmDecoder::get_snr (std::complex<float> *v) {
int16_t	i;
float	noise 	= 0;
float	signal	= 0;
int16_t	low	= T_u / 2 -  carriers / 2;
int16_t	high	= low + carriers;

	for (i = 10; i < low - 20; i ++)
	   noise += abs (v [(T_u / 2 + i) % T_u]);

	for (i = high + 20; i < T_u - 10; i ++)
	   noise += abs (v [(T_u / 2 + i) % T_u]);

	noise	/= (low - 30 + T_u - high - 30);
	for (i = T_u / 2 - carriers / 4;  i < T_u / 2 + carriers / 4; i ++)
	   signal += abs (v [(T_u / 2 + i) % T_u]);
	return get_db (signal / (carriers / 2)) - get_db (noise);
}

int16_t	ofdmDecoder::get_snr	(void) {
	return (int16_t)current_snr;
}

