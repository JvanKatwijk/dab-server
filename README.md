
	DAB-SERVER (bluetooth)

---------------------------------------------------------------------

![java client for dab server](/java-client.png?raw=true)

A simple DAB server, for use with a bluetooth client.
The project I am working on aims at creating a simple remote control for
a DAB server. The latter then will run on e.g. a RPI 2/3 and a bluetooth
device - currently my laptop - will connect using bluetooth, show the
services within reach and controls the radio.
Currently ONLY the sdrplay device is supported. It is not difficult though
to chgange that, although that will most likely lead to changes in the client's
GUI


-------------------------------------------------------------------------
 Definitely work in progress.
--------------------------------------------------------------------------

Current status is that a java client is able to connect - using bluetooth -,
The client might set/change the ifgain and lna state of the device.
The client might set/change the audio loudness setting of the server
(current set using an alsa setting). Of course the client might select
a service. On selecting a service, the server might have to change the
selected channel.
Finally, the client can dorce a rescan over all channels in band III.

One of the major issues is setting up something useful over bluetooth,
while it works, it needs additional attention.

For proper installing bluetooth see 
	https://askubuntu.com/questions/775303/unified-remote-bluetooth-could-not-connect-to-sdp


-------------------------------------------------------------------------
Copyrights
-------------------------------------------------------------------------
	
	Copyright (C)  2016, 2017
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Programming

The dab-library software is made available under the GPL-2.0. The dab-library uses a number of GPL-ed libraries, all
rigfhts gratefully acknowledged.
All SDR-J software, among which dab-library is one - is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 	GNU General Public License for more details.

