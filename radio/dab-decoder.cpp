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

#include	"dab-decoder.h"

//      This function is called from - most likely -
//      different threads from within the library
void    dabDecoder::string_Writer (uint8_t code, std::string theText) {
int     len     = strlen (theText. c_str ());
int     i;
uint8_t	message [len + 3 + 1];
        message [0]     = code;
        message [1]     = ((len + 1) >> 8) & 0xFF;
        message [2]     = (len + 1) & 0xFF;
        for (i = 0; i < len; i ++)
           message [3 + i] = theText [i];
        message [len + 3] = (uint8_t)0;
        theListener (message, len + 3 + 1);
}
//
//	for some keys we only send some data in a vector
void	dabDecoder::vector_Writer (uint8_t code, uint8_t *v, int l) {
uint8_t	message [3 + l + 1];
	message [0]	= code;
	message [1]	= 0;
	message [2]	= l + 1;
	for (int i = 0; i < l; i ++)
	   message [3 + i]	= v [i];
	theListener (message, 3 + l + 1);
}

static
void	syncsignalHandler (bool b, void *userData) {
dabDecoder *ctx = static_cast<dabDecoder *>(userData);
	ctx -> timeSynced.  store (b);
	ctx -> timesyncSet. store (true);
}
//
//	This function is called whenever the dab engine has taken
//	some time to gather information from the FIC blocks.
//	the Boolean b tells whether or not an ensemble has been
//	recognized, the names of the programs are in the 
//	ensemble
static
void	ensemblenameHandler (std::string name, int Id, void *userData) {
dabDecoder *ctx = static_cast<dabDecoder *>(userData);
	ctx -> ensembleRecognized. store (true);
	ctx -> ensembleName	= name;
}

static
void	programnameHandler (std::string s, int SId, void *userData) {
dabDecoder *ctx = static_cast<dabDecoder *>(userData);
	if (!ctx -> scanning. load ())
	   return;
	for (std::map<std::string, std::string>
	                           ::iterator it = ctx -> serviceMap. begin ();
	             it != ctx -> serviceMap. end(); ++it)
	   if (it -> first == s)
	      return;

	ctx -> serviceMap. insert (mapElement (s,
	                        ctx -> theBandHandler. currentChannel ()));
}

static
void	programdataHandler (audiodata *d, void *userData) {
}

static
void	dataOut_Handler (std::string dynamicLabel, void *userData) {
dabDecoder *ctx = static_cast<dabDecoder *>(userData);
	if (ctx -> scanning. load ())
	   return;
	ctx -> string_Writer (Q_TEXT_MESSAGE, dynamicLabel);
}
//
static
void	bytesOut_Handler (uint8_t *data, int16_t amount,
	                  uint8_t type, void *userData) {
dabDecoder *ctx = static_cast<dabDecoder *>(userData);
	(void)data; (void)amount; (void)type; (void)ctx;
}
//
static
void	pcmHandler (int16_t *buffer, int size, int rate,
	                              bool isStereo, void *userData) {
dabDecoder *ctx = static_cast<dabDecoder *>(userData);
uint8_t stereoFlag	= isStereo ? 1 : 0;

//	we have to think here what to do if the rate has to be changed
	if (isStereo !=  ctx -> signalStereo) {
	   ctx -> vector_Writer (Q_STEREO, &stereoFlag, 1);
	   ctx -> signalStereo	= isStereo;
	}

	if (!ctx -> isStarted) {
	   ctx -> soundOut	-> restart ();
	   ctx -> isStarted	= true;
	}
	ctx -> soundOut	-> audioOut (buffer, size, rate);
}

static
void	systemData (bool syncedFlag, int16_t snr,
	                         int32_t freqOff, void *userData) {
dabDecoder *ctx = static_cast<dabDecoder *>(userData);
//	fprintf (stderr, "synced = %s, snr = %d, offset = %d\n",
//	                    flag? "on":"off", snr, freqOff);
uint8_t v [10];
	v [0] = syncedFlag ? 1 : 0;
        v [1] = (uint8_t) snr;
	
	if (ctx -> scanning. load ())
	   return;
	if ((syncedFlag != ctx -> syncFlag)  || (snr != ctx -> old_snr)){
	   ctx -> vector_Writer (Q_STATE, v, 2);
	   ctx -> syncFlag	= syncedFlag;
	   ctx -> old_snr	= snr;
	}
}

static
void	fibQuality	(int16_t q, void *userData) {
dabDecoder *ctx = static_cast<dabDecoder *>(userData);
	(void)q; (void)ctx;
}

static
void	mscQuality	(int16_t fe, int16_t rsE,
	                                int16_t aacE, void *userData) {
dabDecoder *ctx = static_cast<dabDecoder *>(userData);
	(void)fe; (void)rsE; (void)aacE; (void)ctx;
}
//
//
////////////////////////////////////////////////////////////////////////
//

	dabDecoder::dabDecoder (listener_p listener):
	               my_config (std::string (CONFIG)) {

	theListener	= listener;
	handleSettings (&my_config, &my_radioData);
	my_radioData. soundOut	= false;
	int32_t frequency	= 220000000;	// default

#ifdef	HAVE_SDRPLAY
	try {
	   theDevice	=  new sdrplayHandler (frequency,
	                                       my_radioData. ppmCorrection,
	                                       my_radioData. GRdB,
	                                       my_radioData. lnaState,
	                                       my_radioData. autoGain);
	}
	catch (int e) {
	}
#elif	defined(HAVE_AIRSPY)
	try {
	   theDevice	=  new airspyHandler (frequency,
	                                      my_radioData. ppmCorrection,
	                                      my_radioData. airspyGain);
	}
	catch (int e) {
	}
#elif	defined(HAVE_RTLSDR)
	try {
	   theDevice = new rtlsdrHandler   (frequency,
                                            my_radioData. ppmCorrection,
                                            my_radioData. dabstickGain,
                                            my_radioData. autoGain);
	} catch (int e) {
	}
#elif	defined (HAVE_HACKRF)
	try {
	   theDevice = new hackrfHandler (frequency,
	                                  my_radioData. ppmCorrection,
	                                  my_radioData. hackrf_lnaGain,
	                                  my_radioData. hackrf_vgaGain);
	} catch (int e) {
	}
#endif

	if (theDevice == NULL) {
	   throw (42);
	}

	bool	err;
	soundOut	= new audioSink	(my_radioData. latency,
	                                 my_radioData. soundChannel, &err);
	if (err) {
	   throw (43);
	}
//
//	and with a sound device we can create a "backend"
	theRadio	= new dabProcessor (theDevice,
	                                    my_radioData. theMode,
	                                    syncsignalHandler,
	                                    systemData,
	                                    ensemblenameHandler,
	                                    programnameHandler,
	                                    fibQuality,
	                                    pcmHandler,
	                                    bytesOut_Handler,
	                                    dataOut_Handler,
	                                    programdataHandler,
	                                    mscQuality,
	                                    nullptr,	// no mot slides
	                                    this	// ctx
	                                   );
	if (theRadio == nullptr) {
	   throw (44);
	}
//
//	on program startup, we create a servicelist based on
//	previous experiences
	scanning. store (true);
	buildServiceList (false);
	scanning. store (false);
//
//	pass on the initial data
	string_Writer (Q_DEVICE_NAME, theDevice -> deviceName ());
	showSettings ();
	showServices ();
}

void	dabDecoder::showSettings (void) {
uint8_t startData [128];
	switch (theDevice -> whoamI ()) {
	   case S_DABSTICK:
	      startData [0] = S_DABSTICK;
	      startData [1] = 0;	// false for lnaState setting
	      startData [2] = 0;	// begin gain range
	      startData [3] = 100;	// end gain range
	      startData [4] = 1;	// true for autogain
	      startData [5] = my_radioData. dabstickGain;
	      vector_Writer (Q_INITIALS, startData, 6);
	      break;

	   case S_AIRSPY:
	      startData [0] = S_AIRSPY;
	      startData [1] = 0;	// false
	      startData [2] = 0;	// begin gain range
	      startData [3] = 21;	// end gain range
	      startData [4] = 0;	// true for autogain, here false
	      startData [5] = my_radioData. airspyGain;
	      vector_Writer (Q_INITIALS, startData, 6);
	      break;

	   default:
	   case S_SDRPLAY:
	      startData [0] = S_SDRPLAY;
	      startData [1] = 1;	// true for lnaState setting
	      startData [2] = 20;	// begin GRdB range
	      startData [3] = 59;	// end GRdB range
	      startData [4] = 1;	// true for autogain
	      startData [5] = 9;	// range for lna state
	      startData [6] = my_radioData. lnaState;
	      startData [7] = my_radioData. GRdB;
	      vector_Writer (Q_INITIALS, startData, 8);
	      break;

	   case S_HACKRF:
	      startData [0] = S_HACKRF;
	      startData [1] = 0;	// lower bound for lna range
	      startData [2] = 40;	// upperbound for lna range
	      startData [3] = 0;	// false for autogain
	      startData [4] = 0;	// lower bound for vga gain
	      startData [5] = 62;	// upperbound for vga gain
	      startData [6] = my_radioData. hackrf_lnaGain;
	      startData [7] = my_radioData. hackrf_vgaGain;
	      vector_Writer (Q_INITIALS, startData, 8);
	      break;
	}
}

	dabDecoder::~dabDecoder	(void) {
}

void	dabDecoder::stop (void) {
}

void	dabDecoder::setGainSlider (int v) {
#ifdef	HAVE_SDRPLAY
	((sdrplayHandler *)theDevice)	-> set_ifgainReduction (v);
	my_radioData. GRdB = v;
	my_config. update ("GRdB", std::to_string (v));
#elif   HAVE_AIRSPY
	((airspyHandler *)theDevice)	-> set_gain (v);
	my_radioData. airspyGain = v;
	my_config. update ("airspyGain", std::to_string (v));
#elif	HAVE_DABSTICK
	((rtlsdrHandler *)theDevice)	-> set_gain (v);
	my_radioData. dabstickGain = v;
	my_config. update ("dabstickGain", std:;to_string (v));
#elif	HAVE_HACKRF
	((hackrfHandler *)theDevice) -> set_vgaGain (v);
        my_radioData. hackrf_lnaGain = v;
	my_config. update ("hackrf_lnaGain", std::to_string (v));
#endif
}

void	dabDecoder::setLnaState	(int v) {
#ifdef	HAVE_SDRPLAY
	((sdrplayHandler *)theDevice) -> set_lnaState (v);
	my_radioData. lnaState = v;
	my_config. update ("lnaState", std::to_string (v));
#elif	HAVE_HACKRF
	((hackrfHandler *)theDevice) -> set_lnaGain (v);
        my_radioData. hackrf_vgaGain = v;  
	my_config. update ("hackrf_vgaGain", std::to_string (v));
#endif
}

void	dabDecoder::setAutoGain (bool b) {
#ifndef	HAVE_AIRSPY
	theDevice -> set_autogain (b);
#else
	(void)b;
#endif
}

void	dabDecoder::selectService (std::string serviceName) {
	my_radioData. serviceName = serviceName;
	fprintf (stderr, "service request for %s\n",
	                              my_radioData. serviceName. c_str ());
	            
	std::string channel	= findChannelfor (my_radioData. serviceName);
	if (channel == "") {
	   return;
	}

	if (channel != theBandHandler. currentChannel ()) {
	   char temp [255];
	   theDevice -> stopReader ();
	   theRadio	-> stop ();
	   theBandHandler. setChannel (channel);
	   int32_t frequency = theBandHandler. Frequency ();
	   sprintf (temp, "switching to channel %s", channel. c_str ());
	   string_Writer (Q_TEXT_MESSAGE, temp);
	   theDevice	-> restartReader (frequency);
	   timesyncSet.         store (false);
	   timeSynced.          store (false);
	   int timeSyncTime         = 5;
	   ensembleRecognized.       store (false);
	   theRadio		-> start ();
	   while (!timeSynced. load () && (--timeSyncTime >= 0)) {
	      sleep (1);
	   }

	   if (timeSynced. load ())  {
	      timeSyncTime = 5;
	      while (timeSynced. load () && (--timeSyncTime > 0))
	         sleep (1);
	   }

	   if (!ensembleRecognized. load ()) {
	      return;
	   }
	}

	if (theRadio -> kindofService
	              (my_radioData. serviceName) == AUDIO_SERVICE) {
	   audiodata ad;
	   theRadio -> dataforAudioService
	                   (my_radioData. serviceName, &ad);
	   if (!ad. defined) {
	      std::cerr << "sorry  we cannot handle service " <<
	                                    my_radioData. serviceName << "\n";
	   }
	   else {
	      theRadio	-> reset ();
	      theRadio	-> set_audioChannel (&ad);
	      string_Writer (Q_ENSEMBLE, ensembleName);
	   }
	}
}

void	dabDecoder::reset	(void) {
	theDevice	-> stopReader ();
	theRadio	-> stop ();
	vector_Writer (Q_NEW_ENSEMBLE, NULL, 0);
	scanning. store (true);
	buildServiceList (true);
	showServices ();
	scanning. store (false);
}

void	dabDecoder::showServices	(void) {
	for (std::map<std::string, std::string>
	                              ::iterator it = serviceMap. begin ();
	                 it != serviceMap. end(); ++it) {
	   string_Writer (Q_SERVICE_NAME, it -> first);
	   usleep (100);
	}
	string_Writer (Q_TEXT_MESSAGE, std::string ("All known stations are listed"));
}

void	dabDecoder::showDevicename	(void) {
	string_Writer (Q_DEVICE_NAME, theDevice -> deviceName ());
}

void	dabDecoder::handleSettings (config *my_config, radioData *rd) {
std::string value;

//	The defaults
	rd	-> theMode		= 1;
	rd	-> theChannel		= "11C";
	rd	-> theBand		= BAND_III;
	rd	-> ppmCorrection	= 0;
	rd	-> GRdB			= 30;
	rd	-> lnaState		= 4;
	rd	-> airspyGain		= 19;
        rd	-> dabstickGain		= 60;
        rd	-> hackrf_lnaGain	= 30;
	rd	-> hackrf_vgaGain	= 40;
	rd	-> autoGain		= false;
	rd	-> soundChannel		= "default";
	rd	-> latency		= 10;
	rd	-> waitingTime		= 10;

	value	= my_config -> getValue ("Mode");
	if (value != std::string (""))
	   rd	-> theMode	= stoi (value);
	value	= my_config	-> getValue ("Band");
	if (value != std::string (""))
	   rd	-> theBand	= stoi (value);
#ifdef	HAVE_SDRPLAY
	value	= my_config	-> getValue ("lnaState");
	if (value != std::string (""))
	   rd	-> lnaState	= stoi (value);
	value	= my_config	-> getValue  ("GRdB");
	if (value != std::string (""))
	   rd	-> GRdB		= stoi (value);
	value	= my_config	-> getValue ("autoGain");
	if (value != std::string (""))
	   rd	-> autoGain	= stoi (value) != 0;
#elif	HAVE_AIRSPY
	value	= my_config	-> getValue  ("airspyGain");
	if (value != std::string (""))
	   rd	-> airspyGain		= stoi (value);
#elif	HAVE_DABSTICK
	value	= my_config	-> getValue  ("dabstickGain");
	if (value != std::string (""))
	   rd	-> dabstickGain		= stoi (value);
	value	= my_config	-> getValue ("autoGain");
	if (value != std::string (""))
	   rd	-> autoGain	= stoi (value) != 0;
#elif	HAVE_HACKRF
	value	= my_config	-> getValue ("hackrf_lnaGain");
	if (value != std::string (""))
	   rd -> hackrf_lnaGain	= stoi (value);
	value	= my_config	-> getValue ("hackrf_vgaGain");
	if (value != std::string (""))
	   rd -> hackrf_vgaGain	= stoi (value);
#endif
}

bool	dabDecoder::searchable (std::string channel) {
	return (my_config. getValue (channel) == "on");
}

void	dabDecoder::markChannels (bool flag, std::string Channel) {
	my_config. update (Channel, flag ? "on": "no data");
}

void	dabDecoder::buildServiceList (bool fresh) {
std::string	theChannel	= "5A";
std::string	startChannel	= "5A";

//	be sure that the serviceList is empty
        serviceMap. clear ();

	theBandHandler. setChannel (theChannel);
//	here we really start
	while (true) {
	   if (fresh || searchable (theChannel)) {
	      theDevice		-> stopReader ();
	      theRadio		-> stop ();
              theDevice		-> restartReader
	                                (theBandHandler. Frequency ());
	      timesyncSet.	store (false);
	      timeSynced.	store (false);
	      int timeSyncTime	= 2;
	      ensembleRecognized.	store (false);
	      std::string s = "scanning channel ";
	      s = s. append (theChannel);
	      string_Writer (Q_TEXT_MESSAGE, s);
	      theRadio	-> start ();
//
//	The approach is to look for a short time for a kind of DAB signal
//	if not, continue with the next channel. If a timesync could
//	be made (should not take more than 10 to 20 frames), 
//	continue for a longer period

	      while (!timesyncSet. load () && (--timeSyncTime >= 0)) {
	         sleep (1);
	      }
	      if (timeSynced. load ())	 {
	         fprintf (stderr, "%s might have DAB data\n",
	                                  theChannel. c_str ());
	         timeSyncTime = my_radioData. waitingTime;
	         while (timeSynced. load () && (--timeSyncTime > 0))
	            sleep (1);
	      }
	      markChannels (ensembleRecognized. load (), theChannel);
	      theDevice -> stopReader ();
	   }
           theChannel = theBandHandler. nextChannel ();
           if (theChannel == startChannel)
              break;
	   theBandHandler. setChannel (theChannel);
	}
	theRadio	-> stop ();
}

std::string	dabDecoder::findChannelfor (std::string s) {
	for (std::map<std::string, std::string>
	                         ::iterator it = serviceMap. begin ();
	   it != serviceMap. end (); ++it) {
	   if (it -> first == my_radioData. serviceName) 
	      return it -> second;
	}
	return "";
}



