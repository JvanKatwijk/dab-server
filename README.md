-----------------------------------------------------------------------
	STAND ALONE DAB-SERVER with an android client as remote control
---------------------------------------------------------------------

![android client for dab server](/android-client.png?raw=true)

The RPI2 and 3 are excellent devices to use as a specialized
server, they are small but powerfull enough to do all kinds of 
computations.

In this project a DAB server, running on an headless RPI2/3 (under
Stretch) is being developed.
The server is started on the RPI (as a "service"),
it will run without any further
interaction from a user, i.e. the RPI can run headless.
The user interacts with the server using an android client, using a bluetooth
connection. 

The server, when started, will wait for a user to connect through
a bluetooth connection. Once connected, the user can actually
start the dab "engine" by touching the "Reset" button.

The dab engine will scan to a set of preselected channels for
DAB data, and will collect the names of the services.
After the scan, the server wil send a list with service names
to the client.

The client may select a service, the server then will tune to
the right channel, and will select the service for processing.

Since most of the channels do not carry DAB data, the server is  instruct to "remember"
in which channels it found an ensemble when doing a full scan.

Touching the reset button again will instruct the server to scan
all channels in the given band - of course the results are
recorded by the server and used the next invocation
to scan the band.

We use bluetooth as communication medium between
client and server, the server announces its service with a fancy UUID.

A simple android client is being exercised on my tablet.
The client is still pretty simple, and android is subject
to further study, but it is working pretty well.

To experiment a little with anroid, there are two versions, identical
though for different screen formats.

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

The server stores some results - i.e. the channel names where DAB
data night be found, gain setting etc - in an ini file, which
is - by default - kept in /usr/local/src. The name can be changed
in the include file in the _dab-decoder.h" file.

The server will send its audio output to the soundcard of
the machine it runs on.

----------------------------------------------------------------------
Running the server as a systemd service
----------------------------------------------------------------------

In my setting the server runs stand alone. Communication - if any-
is using bluetooth. Obviously, one should have enabled bluetooth on the RPI,
how to instal the bluez software is described in many places
on the internet. In order to allow the server to register
its "service", and to make the bluetooth discoverable, one should
add "--compat" to the line

	ExecStart=/usr/lib/bluetooth/bluetoothd --compat

and add the lines

	ExecStartPost=/bin/chmod 777 /var/run/sdp
	ExecStartPost=/bin/hciconfig hci0 piscan

all in the file

	/etc/systemd/system/dbus-org.bluez.service

To have the dab-server start as a service, one should create a
file (here named /etc/systemd/system/dab-server.service), with as content

	[Unit]
	Description=dab-server
	After=systemd-udev.service
	After=dbus-org.bluez.service
	
	[Service]
	ExecStart= /bin/sh /home/pi/dab-server/command.sh
	
	[Install]
	WantedBy=default.target

The registration of the service is by

	systemctl daemon-reload
	systemctl enable dab-server.service


The service will execute - after the bluetooth service was enabled -
the file "command.sh", which is stored in the dab-server folder
in the directory /home/pi.

The command file just contains a 2 line shell script to instantiate
the dab server.


Note that these files are still subject to further development

----------------------------------------------------------------------
The Android remote control.
-----------------------------------------------------------------------

The android remote control is - as can be expected - under development. The releases
section contains an "apk" file, the package built by android studio and
the one running on my tablet.

The android remote control has a start button, touching it will instruct the client
to look at bluetooth devices in the neighbourhood.
After touching the name of the device in this list
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

For the SDRplay, the state of the lna and the gain reduction (20 .. 59)
can be set (as well as the autogain).  For the AIRspy the
gain slider is constrained to values from 1 .. 21, and both the spinner
for the lnaState and the button for the autogain are hidden.
For the rtlsdr based devices, the gain setting allows values from
0 .. 100, they will be translated by the device driver software
to appropriate gain values.

The top slider on the remote control can be used to control the sound setting of the server.
One might have to adapt the code in the server, controlling the sound level
is by using the "amixer" function of alsa.

The next-to-top slider can be used to control the gain setting of the device.
As device shown on the picture is an SDRplay device, the spinner to the left
of the slider is used to set the lna state and the slider sets
the gain reduction. If e.g. an RTLSDR based device
is configured, this spinner will not be visible and the
slider will set the gain.

If the user is not satisfied with the amount of services,
there is a "reset" button on the GUI that - when touched -instructs the
server to scan all channels again
and record which channels carry useful data.

To close down the connection, a the user can touch the "quit" button, the server
will then be available for another client.
If the connection is broken by stopping the remore control program, 
the server will just continue. To change the
server settings, restart the remote control, connect and instruct.

---------------------------------------------------------------------------
Possible extension
----------------------------------------------------------------------------

Having used the device now for a couple of days, it seems one thing is missing:
closing down the RPI from the remore control. Just pulling out the plug is not
good for the health of the card.


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
rigfhts gratefully acknowledged. The android client is a first attempt to create some application for android
and leaves quite some room for improvement.



