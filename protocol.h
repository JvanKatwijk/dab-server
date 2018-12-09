#
/*
 *    Copyright (C) 2015, 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DAB-library
 *
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

//
//	talking to the dab-xxx-6 program is through a port (default BASE_PORT)

#ifndef	__PROTOCOL__
#define	__PROTOCOL__

//	The radio will react on commands
//	The commands consist of a package with two components
//	the command identifier and a value.
//	Value is either a null-terminated string or a char value
//	e.g. Q_GAIN 70 or Q_CHANNEL "11C"

#define Q_QUIT		0100
#define Q_GAIN_SLIDER	0101
#define Q_SOUND_LEVEL	0102
#define Q_LNA_STATE	0103
#define	Q_AUTOGAIN	0104
#define Q_SERVICE       0106
#define	Q_RESET		0107
#define	Q_SYSTEM_EXIT	0110

//	The radio sends messages to the (connected) client
#define	Q_INITIALS	0100
#define	Q_DEVICE_NAME	0101
#define Q_ENSEMBLE      0102
#define Q_SERVICE_NAME  0103
#define Q_TEXT_MESSAGE  0104
#define	Q_STATE		0105
#define	Q_STEREO	0106
#define	Q_NEW_ENSEMBLE	0107
#define	Q_SOUND		0110
//
//	packetstructure
//	byte 0	key
//	byte 1	high order byte of 16 bit data length 
//	byte 2  low order byte of 16 bit data length
//	byte 3 .. 3 + length payload
//
//	for Q_INITIALS the payload is the encoding of a record
//	for DEVICE_NAME, ENSEMBLE, SERVICE_NAME and TEXT_MESSAGE the
//	payload is a null terminated string
//	for STATE the payload consists of the values snr and synced 
//	for STEREO the payload is a single value 1 or 0
//	for NEW_ENSEMBLE, a trigger, the payload is empty

#endif
