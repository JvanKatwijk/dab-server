
	DAB-SERVER (bluetooth), android client as remote control

---------------------------------------------------------------------

![android client for dab server](/android-client.png?raw=true)

A simple DAB server - supporting SDRplay devices, AIRspy devices and RTLSDR
devices - for use with an android client as remote control using Bluetooth,

The project I am working on aims at extending the "dab-cmdline" library and
providing DAB functionality through a server with a (reasonably) well defined interface.
In this project I am using Bluetooth as means for communication for an android
remote controller.

The **server** runs on a Linux box, in my environment it is usually
running on an RPI 2/3 under Stretch.

The server, when started, collects services in ensembles
found in the different channels in Band III, and - when this is finished -
is waiting for someone to call.

Since most of the channels do not carry DAB data, the server will "remember"
in which channels it found an ensemble.
We use bluetooth as communication medium between
client and server, the server announces its service with a fancy UUID.

Two clients - remote controls - are being worked on

 a. a simple Java client is being exercised on my laptop, it has some problems
in identifying the server;

 b. a simple android client, being exercised on my tablet. The client is still pretty simple, and android is subject
to further study, but it is working pretty well.

---------------------------------------------------------------------

The server

---------------------------------------------------------------------

The server - currently - can be configured to support one of
the usual set (SDRplay, AIRspy, RTLSDR) devices.
Creating the server is by

	mkdir build
	cd build
	cmake .. -DXXX=ON (XXX is either SDRPLAY, AIRSPY or RTLSDR)
	make

Running the server is simply by executing "./dab-server"

The server might be called with some parameters for setting the
(initial) gain

The settings of the gain as well as the status of the different channels
are stored in a file ".dab-server.ini", kept in the home directory
of the user.

The server will send its audio output to the soundcard of the machine it runs on.

----------------------------------------------------------------------

The Android remote control.

-----------------------------------------------------------------------
The android remote control is - as can be expected - under development. The releases
section contains an "apk" file, the package built by android studio and
the one running on my tablet.

The android remote control has a start button, touching it will instruct the client
to look at bluetooth devices in the neighbourhood.
The client will display the names of the devices found on the GUI,
after touching the name of the device in this list
the client will (try to) establish the connection, assuming a
server runs on the selected device.
.
As soon as a client is connected to a device running the server,
the names of the services are
transmitted to the client, and the server is ready to receive commands.

Typical commands are selecting a service or changing the
gain setting of the device.
The client "knows" which device is used at the server side,
and it adapts its GUI wrt to gain setting.
I.e. for the SDRplay, the state of the lna and the gain reduction
can be set (as well as the autogain), while for the AIRspy the
gain slider allows values from 1 .. 21, and both the spinner
for the lnaState and the button for thr autogain are hidden.
For the rtlsdr based devices, the gain setting allows values from
0 .. 100, they will be translated by the device driver software
to appropriate gain values.

If the user is not satisfied with the amount of services,
there is a "reset" button on the GUI that - when touched -instructs the
server to scan all channels again
and record which channels carry useful data.

To close down the connection, a the user can touch the "quit" button, the server
will then be available for another client.
If the connection is broken, the server will just continue. To change the
server settings, restart the remote control, connect and instruct.


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


The SDRplay devices, AIRspy devices and "dabsticks" (RTLSDR based devices)
are supported. Although quite obvious, ensure that the corresponding
support library is installed.

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



