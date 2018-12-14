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
#include	<unistd.h>
#include        <stdio.h>
#include        <stdbool.h>
#include        <stdint.h>
#include        <sys/socket.h>
#include        <bluetooth/bluetooth.h>
#include        <bluetooth/rfcomm.h>
#include        <bluetooth/sdp.h>
#include        <bluetooth/sdp_lib.h>
#include	<signal.h>
#include	<getopt.h>
#include        <cstdio>
#include        <iostream>
#include        <pthread.h>
#include	<string>
#include	"protocol.h"
#include	"dab-decoder.h"

using std::cerr;
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
 *    along with DAB-server; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	<unistd.h>
#include        <stdio.h>
#include        <stdbool.h>
#include        <stdint.h>
#include        <sys/socket.h>
#include        <bluetooth/bluetooth.h>
#include        <bluetooth/rfcomm.h>
#include        <bluetooth/sdp.h>
#include        <bluetooth/sdp_lib.h>
#include	<signal.h>
#include	<getopt.h>
#include        <cstdio>
#include        <iostream>
#include        <pthread.h>
#include	<string>
#include	"protocol.h"
#include	"dab-decoder.h"

using std::cerr;
using std::endl;

static
int	client	= 0;

static
dabDecoder	*theDecoder	= NULL;
static
int    handleRequest	(char *buf, int bufLen);

static
sdp_session_t *register_service (void);

static
void    listener        (uint8_t *lbuf, int size);

int	main (int argc, char **argv) {
struct sigaction sigact;
bool	err;
struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
char buf [1024] = { 0 };
int s;
socklen_t opt = sizeof (rem_addr);
bdaddr_t tmp	= (bdaddr_t) {{0, 0, 0, 0, 0, 0}};
//
//	prepare the communication
//
        register_service ();
//      allocate socket
        s = socket (AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
//      bind socket to port 1 of the first available

//      local bluetooth adapter
        loc_addr.rc_family = AF_BLUETOOTH;
        loc_addr.rc_bdaddr = tmp;
//	loc_addr.rc_bdaddr = *BDADDR_ANY;
	loc_addr.rc_channel = (uint8_t) 1;
        bind (s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
//
//
//	The eternal server loop
	while (true) {
//      put socket into listening mode
           fprintf (stderr, "server is listening\n");
           listen (s, 1);
//      accept one connection
           client = accept (s, (struct sockaddr *)&rem_addr, &opt);
//
//      reset the state of the receiver here
           ba2str (&rem_addr. rc_bdaddr, buf);
           fprintf (stderr, "accepted connection from %s\n", buf);
           memset (buf, 0, sizeof (buf));
//
//	after the client restarts itself, we might end up here,
//	so, we need to give some info for the GUI
	   if (theDecoder != NULL) {
	      theDecoder -> showSettings ();
	      theDecoder -> showServices ();
	      theDecoder -> showDevicename ();
	
	   }
//	The server-loop itself is quite simple
           while (true) {
              char buf [1024];
              int bytes_read = read (client, buf, sizeof (buf));
              if (bytes_read > 0) {
                 if (handleRequest (buf, bytes_read) == 0)
	            break;
	      } else {
                 fprintf (stderr, "read gives back %d\n", bytes_read);
                 break;
	      }
	   }
	}

	close (client);
	close (s);
}

static
void	set_audioLevel	(int);

static
sdp_session_t *register_service (void) {
//uint8_t svc_uuid_int [] =
//{0x04, 0xc6, 0x09, 0x3b, 0x00, 0x00, 0x10, 0x00,
// 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb};
uint32_t svc_uuid_int[] = {0x01110000, 0x00100000, 0x80000080, 0xFB349B5F};


uint8_t rfcomm_channel		= 1;
const char *service_name	= "Jan's dab handler";
const char *svc_dsc		= "dab handler";
const char *service_prov	= "Lazy Chair computing";

uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid, svc_class_uuid;
sdp_list_t *l2cap_list		= 0, 
	   *rfcomm_list		= 0,
	   *root_list		= 0,
	   *proto_list		= 0, 
	   *access_proto_list	= 0,
	   *svc_class_list	= 0,
	   *profile_list	= 0;
sdp_data_t *channel		= 0;
sdp_profile_desc_t profile;
sdp_record_t record		= { 0 };
sdp_session_t	*session	= 0;

//	set the general service ID
//	sdp_uuid16_create (&svc_uuid, 0xabcd);
	sdp_uuid128_create (&svc_uuid, &svc_uuid_int);
	sdp_set_service_id (&record, svc_uuid);

        char str [256] = "";
	sdp_uuid2strn (&svc_uuid, str, 256);
	fprintf (stderr, "registering UUID %s\n", str);
//	set the service class
	sdp_uuid16_create (&svc_class_uuid, SERIAL_PORT_SVCLASS_ID);
	svc_class_list	= sdp_list_append (0, &svc_class_uuid);
	sdp_set_service_classes (&record, svc_class_list);
//
//	set the Bluetooth profile information
	sdp_uuid16_create (&profile. uuid, SERIAL_PORT_PROFILE_ID);
	profile. version	= 0x100;
	profile_list		= sdp_list_append (0, &profile);
	sdp_set_profile_descs (&record, profile_list);

//	make the service record publicly browsable
	sdp_uuid16_create (&root_uuid, PUBLIC_BROWSE_GROUP);
	root_list		= sdp_list_append (0, &root_uuid);
	sdp_set_browse_groups (&record, root_list);

//	set l2cap information
	sdp_uuid16_create (&l2cap_uuid, L2CAP_UUID);
	l2cap_list		= sdp_list_append (0, &l2cap_uuid);
	proto_list		= sdp_list_append (0, l2cap_list);
 
//	set rfcomm information
	sdp_uuid16_create (&rfcomm_uuid, RFCOMM_UUID);
	channel			= sdp_data_alloc (SDP_UINT8, &rfcomm_channel);
	rfcomm_list		= sdp_list_append (0, &rfcomm_uuid);
	sdp_list_append (rfcomm_list, channel);
	sdp_list_append (proto_list, rfcomm_list);

//	attach protocol information to service record
	access_proto_list	= sdp_list_append (0, proto_list);
	sdp_set_access_protos (&record, access_proto_list);
//
//
	sdp_set_info_attr (&record, service_name, service_prov, svc_dsc);

//	connect to the local SDP server, register the service record, and 
//	disconnect
//	session = sdp_connect (BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY );

//	C++ compiler does not like BDADDR_ANY and BDADDR_LOCAL
//	so we had to reinvent a number of wheels
	bdaddr_t tmp_1	= (bdaddr_t) {{0, 0, 0, 0, 0, 0}};
	bdaddr_t tmp_2	= (bdaddr_t) {{0, 0, 0, 0xff, 0xff, 0xff}};
	session = sdp_connect (&tmp_1, &tmp_2, SDP_RETRY_IF_BUSY );
	sdp_record_register (session, &record, 0);

//	cleanup
	sdp_data_free (channel);
	sdp_list_free (l2cap_list, 0);
	sdp_list_free (rfcomm_list, 0);
	sdp_list_free (root_list, 0);
	sdp_list_free (access_proto_list, 0);
	sdp_list_free (svc_class_list, 0);
	sdp_list_free (profile_list, 0);

	return session;
}

static
void	set_audioLevel (int offset) {
char	command [255];
	if (offset == 0)
	   return;
//
//	change this for your configuration
	sprintf (command, "amixer set Master -- %d%%", offset);
//	sprintf (command, "amixer set PCM -- %d%%", (offset + 10) * 5);
	system (command);
}

static
void	createMessage (std::string s) {
	fprintf (stderr, "%s \n", s. c_str ());
}

static
int    handleRequest (char *lbuf, int bufLen) {
int     starter = 0;
        while (starter < bufLen) {
           switch (lbuf [starter + 0]) {
	      case Q_RESET:
	         if (theDecoder == NULL)	// first time
	         try {
	            theDecoder = new dabDecoder (listener);
	            createMessage (std::string ("DAB decoder is being created"));
	         } catch (int e) {
	            createMessage (std::string ("DAB decoder could not be created"));
	         }
	         else {
	            theDecoder -> reset ();
	            createMessage (std::string ("DAB decoder is being reset"));
	         }
	         break;

	      case Q_SOUND_LEVEL:
	         set_audioLevel (lbuf [starter + 3]);
	         break;

	      case Q_GAIN_SLIDER:
	         if (theDecoder != NULL)
	            theDecoder -> setGainSlider (lbuf [starter + 3]);
	         break;

	      case Q_LNA_STATE:
	         if (theDecoder != NULL)
	            theDecoder -> setLnaState (lbuf [starter + 3]);
	         break;

	      case Q_AUTOGAIN:
	         if (theDecoder != NULL)
	            theDecoder -> setAutoGain (lbuf [starter + 3] != 0);
	         break;

	      case Q_SYSTEM_EXIT:
	         if (theDecoder != NULL) {
	            theDecoder	-> stop ();
	            delete theDecoder;
	         }
	         fprintf (stderr, "request to stop the RPI\n");
	         system ("halt");
	         break;

	      case Q_SERVICE:
	         if (theDecoder != NULL) {
	            std::string s = std::string (&lbuf [starter + 3]);
	            theDecoder	-> selectService (s);
	         }
	         break;

	      default:		// it is for the client
	         break;
	   }
           starter += 3 + lbuf [2];
	}
	return 1;
}
//
//	messages from the client, does not interpret
//	anything, just passes it on
static
void	listener	(uint8_t *lbuf, int size) {
	if (client != 0)
	   write (client, lbuf, size);
}

