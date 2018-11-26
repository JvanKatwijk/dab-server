#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the DAB library
 *
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
 *
 *	We have to create a simple virtual class here, since we
 *	want the interface with different devices (including  filehandling)
 *	to be transparent
 */
#ifndef	__DEVICE_HANDLER__
#define	__DEVICE_HANDLER__

#include	<stdint.h>
#include	<complex>
#include	<thread>
using namespace std;

#define	S_SDRPLAY	001
#define	S_DABSTICK	002
#define	S_AIRSPY	003
#define	S_NOBODY	000

class	deviceHandler {
public:
			deviceHandler 	(void);
virtual			~deviceHandler 	(void);
virtual		bool	restartReader	(int32_t);
virtual		void	stopReader	(void);
virtual		int32_t	getSamples	(std::complex<float> *, int32_t);
virtual		int32_t	Samples		(void);
virtual		void	set_autogain	(bool);
virtual		std::string deviceName	(void);
virtual		int	whoamI		(void);
//
//	for the sdrplay
virtual		void	set_ifgainReduction (int);
virtual		void	set_lnaState	(int);
//
//	for the dabsticks and the airspy
virtual		void	set_gain	(int);
protected:
		int32_t	lastFrequency;
	        int32_t	vfoOffset;
	        int	theGain;
};
#endif

