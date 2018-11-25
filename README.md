
	DAB-SERVER (bluetooth), java and android clients

---------------------------------------------------------------------

![android client for dab server](/android-client.png?raw=true)

A simple DAB server, for use with an android client using Bluetooth.

The project I am working on aims at extending the "dab-cmdline" library and
providing DAB functionality through a server with a (reasonably) well defined interface.
In this project I am experimenting with Bluetooth as means for cummunication.

The **server** runs on a Linux box, it is usually running on an RPI 2/3
under Stretch.
The server, when started, collects services in ensembles  found in the different channels
in Band III, and - when this is finished - is waiting for someone to call.

In this experiment, we use bluetooth as communication medium between
client and server, the server announces its service with a fancy UUID.

Two clients are being worked on

a. a simple Java client is being exercised on my laptop
b. a simple android client, being exercised on my tablet. The client is still pretty simple, and android is subject
to further study.

----------------------------------------------------------------------

The Android client

-----------------------------------------------------------------------

The android client has a start button, touching it will instruct the client
to look at bluetooth devices in the neighbourhood.
The server will display the names of the devices found,
after touching the name of the device in this list
the server will (try to) establish the connection.

As soon as a client is connected the names of the services are
transmitted to the client, and the server is ready to receive commands.

Typical commands are selecting a service or changing the gain setting of the device.
If the user is not satisfied with the amount of services, there is a "reset" button
that instructs the server to scan all channels again.

To close down the connection, a the uer can touch the "quit" button, the server
will then be available for another client.

----------------------------------------------------------------------------

Java Client

----------------------------------------------------------------------------

![java client for dab server](/java-client.png?raw=true)

The java client will try to connect directly to the server that reports
to have the right service.

Operation is more or less the same as with the android client,
the server will - once the connection is established - transmit
the names of the services.

Selecting a service is by clicking on the name.
Here also, a reset button is available and a slider and spinbox for setting
the gain of the device connected to the server.

---------------------------------------------------------------------------


Issues to be considered

--------------------------------------------------------------------------

One of the open issues is where the sound should go, in the current approach the server
will have to make it audible, the client just being the remore control.
The alternative - obviously - would be to send the audio back to the client.

---------------------------------------------------------------------------

Supported devices

---------------------------------------------------------------------------

Currently the ONLY supported device is the sdrplay device. It is pretty simple though
to change that, although that will most likely lead to changes in the client's
GUI (Note that the current GUI shows controls for both the if gain reduction and
the lnaState, while for e.g. DABsticks a single slider, setting the gain, might
suffice)

----------------------------------------------------------------------------

Bluetooth

-------------------------------------------------------------------------------

Installing bluetooth on Linux required some searching, for proper installing bluetooth see 
	https://askubuntu.com/questions/775303/unified-remote-bluetooth-could-not-connect-to-sdp
For Java, the bluecove libraries implement the required functionality and
Android provides a quite extensive library for Bluetooth support.


-------------------------------------------------------------------------
Copyrights
-------------------------------------------------------------------------
	
	Copyright (C)  2017, 2018
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Programming

The dab-library software is made available under the GPL-2.0. The dab-library uses a number of GPL-ed libraries, all
rigfhts gratefully acknowledged. The DAB-java client is available under GPL-2.0. The Android client is for private use only.



