/*
 *    Copyright (C) 2015, 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DAB-server
 *
 *    DAB-server is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DAB-server is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DAB-library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef	__DAB_DECODER__
#define	__DAB_DECODER__

#include        <unistd.h>
#include        <stdio.h>
#include        <stdbool.h>
#include        <stdint.h>
#include        <signal.h>
#include        <getopt.h>
#include        <cstdio>
#include        <iostream>
#include        <complex>
#include        <vector>
#include        <map>
#include        <pthread.h>
#include        "audiosink.h"
#include        "config.h"
#include        "band-handler.h"
#include        "protocol.h"
#include        "radiodata.h"
#include        "radiodata.h"
#include	"device-handler.h"
#ifdef  HAVE_SDRPLAY
#include        "sdrplay-handler.h"
#elif defined(HAVE_AIRSPY)
#include        "airspy-handler.h"
#elif   defined (HAVE_RTLSDR)
#include        "rtlsdr-handler.h"
#elif	defined (HAVE_HACKRF)
#include	"hackrf-handler.h"
#endif
//
//	The one who is doing the work:
#include        "dab-processor.h"

#define	CONFIG	"/usr/local/src/.dab-server.ini"

using std::cerr;
using std::endl;
typedef void (*listener_p)(uint8_t *, int);
typedef std::pair <std::string, std::string> mapElement;

class dabDecoder {
public:
		dabDecoder	(listener_p);
		~dabDecoder	(void);
	void	setGainSlider	(int);
	void	setLnaState	(int);
	void	setAutoGain	(bool);
	void	selectService	(std::string);
	void	reset		(void);
	void	stop		(void);
	void	showServices	(void);
	void	showDevicename	(void);

        std::string		ensembleName;
	std::atomic<bool>	ensembleRecognized;
	std::atomic<bool>	scanning;
	std::atomic<bool>	syncFlag;

	bool		signalStereo;
	bool		isStarted;
	std::map<std::string, std::string> serviceMap;
	audioSink       *soundOut;
	void	string_Writer 	(uint8_t code, std::string theText);
	void	vector_Writer	(uint8_t code, uint8_t *v, int l);
	bandHandler	theBandHandler;
	int		old_snr;
	std::atomic<bool>	timeSynced;
	std::atomic<bool>	timesyncSet;
private:
	void	buildServiceList	(bool);
	void	handleSettings	(config *, radioData *);
	bool	searchable	(std::string);
	void	markChannels	(bool, std::string);
	std::string	findChannelfor	(std::string);

	listener_p	theListener;
	config		my_config;
	radioData	my_radioData;
	deviceHandler	*theDevice;
	dabProcessor	*theRadio;
};
#endif

