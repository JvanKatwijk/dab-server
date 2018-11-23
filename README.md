
	DAB-SERVER (bluetooth)

---------------------------------------------------------------------

![java client for dab server](/java-client.png?raw=true)

A simple DAB server, for use with a bluetooth client.

The project I am working on aims at creating a simple remote control for
a DAB server, the latter being a modified (simplified) version of
the DAB library. The server then will run on something like an RPI 2/3 and a bluetooth
device - currently my laptop - will connect using bluetooth, show the
services within reach and controls the radio.
The picture shows a simple java client, running on my laptop.
Current work is on creating a client on android, so that the control
is using your tablet or phone.

Currently ONLY the sdrplay device is supported. It is pretty simple though
to change that, although that will most likely lead to changes in the client's
GUI (Note that the current GUI shows controls for both the if gain reduction and
the lnaState, while for e.g. DABsticks a single slider, setting the gain, might
suffice)


Current status is that a java client is able to connect - using bluetooth -,
The client might set/change the ifgain and lna state of the device.
The client might set/change the audio loudness setting of the server
(current set using an alsa setting). Of course the client might select
a service. On selecting a service, the server might have to change the
selected channel.
Finally, the client can force a rescan over all channels in band III.

One of the major issues is setting up something useful over bluetooth,
while it works, it needs additional attention.

For proper installing bluetooth see 
	https://askubuntu.com/questions/775303/unified-remote-bluetooth-could-not-connect-to-sdp

----------------------------------------------------------------------------
Android client
----------------------------------------------------------------------------

Most of the current activities are related to creating an android client
Since it is my first activity on Android, it will take some time

-------------------------------------------------------------------------
Copyrights
-------------------------------------------------------------------------
	
	Copyright (C)  2017, 2018
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Programming

The dab-library software is made available under the GPL-2.0. The dab-library uses a number of GPL-ed libraries, all
rigfhts gratefully acknowledged. The DAB client is available under GPL-2.0. The Android client is for private use only.



