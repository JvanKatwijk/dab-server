#
/*
 *    Copyright (C) 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DAB library of the SDR-J software
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
#
#ifndef	__DATA_PROCESSOR__
#define	__DATA_PROCESSOR__

#include	<stdio.h>
#include	<string.h>
#include	<vector>
#include	"dab-constants.h"
#include	"backend-base.h"

class	virtual_dataHandler;

class	dataProcessor:public backendBase {
public:
	dataProcessor	(int16_t	bitRate,
	                 packetdata	*pd,
	                 bytesOut_t     bytesOut,
	                 motdata_t	motdataHandler,
	                 void		*ctx);
	~dataProcessor	(void);
void	addtoFrame	(uint8_t *);
private:
	int16_t		bitRate;
	uint8_t		DSCTy;
	int16_t		appType;
	int16_t		packetAddress;
	uint8_t		DGflag;
	int16_t		FEC_scheme;
	bytesOut_t	bytesOut;
	void		*ctx;
	int16_t		crcErrors;
	int16_t		handledPackets;
	std::vector<uint8_t> series;
	uint8_t		packetState;
//
//	result handlers
	void		handleTDCAsyncstream 	(uint8_t *, int16_t);
	void		handlePackets		(uint8_t *, int16_t);
	void		handlePacket		(uint8_t *);
	virtual_dataHandler *my_dataHandler;
};

#endif

