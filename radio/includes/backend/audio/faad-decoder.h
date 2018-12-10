#
/*
 *    Copyright (C) 2014 .. 2017
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
*/
#
#ifndef	__FAAD_DECODER__
#define	__FAAD_DECODER__
#include	"neaacdec.h"
#include	"ringbuffer.h"
#include	"dab-constants.h"

typedef struct {
        int rfa;
        int dacRate;
        int sbrFlag;
        int psFlag;
        int aacChannelMode;
        int mpegSurround;
} stream_parms;

class	faadDecoder {
public:
		faadDecoder	(audioOut_t soundOut,
	                         void	*userData);
		~faadDecoder	(void);
	int16_t	MP42PCM (stream_parms *sp,
	                 uint8_t buffer [], int16_t bufferLength);
private:
	bool    initialize      (stream_parms *);
	bool			processorOK;
	bool			aacInitialized;
	uint32_t		aacCap;
	NeAACDecHandle		aacHandle;
	NeAACDecConfigurationPtr	aacConf;
	NeAACDecFrameInfo	hInfo;
	int32_t			baudRate;
	void			*userData;
	audioOut_t		soundOut;
	void			output (int16_t *buffer,
	                                int	size,
	                                bool	isStereo,
	                                int	rate);
};
#endif

