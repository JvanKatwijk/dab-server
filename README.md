-----------------------------------------------------------------------
	STAND ALONE DAB-SERVER with an android client as remote control
---------------------------------------------------------------------

![android client for dab server](/android-client.png?raw=true)

The RPI2 and 3 are excellent devices for use as a specialized
server, they are small but powerfull enough to do all kinds of 
computations.

In this project a DAB server, running on an headless RPI2/3 (under
Stretch) is being developed together with an "app" for use on an android tablet
acting as remote control (see the picture).

The server is implemented as a "service" on the RPI, i.e it is
one of the services that on booting up the RPI will be started.
As such, it will run without any further
interaction from a user, i.e. the RPI can run headless, even without WiFi enabled.

The server, after being started, will wait for a user to connect through
a bluetooth connection. Once connected, the user can actually
start the dab "engine" by touching the "Reset" button.

The dab "engine" will scan a set of preselected channels for
DAB data, and will collect the names of the services found in ensembles
in these channels.
After the scan, the server wil send a list with service names
to the client.

The user may select a service by touching the name,
the server then will tune to
the right channel, will select the service for processing and will
instruct the dab "engine"
to process data for the selected service.

---------------------------------------------------------------------------
---------------------------------------------------------------------------

An "ini" file contains a list of channels that - as seen in a last scan -
contain DAB data. Since most of the channels do not carry DAB data, the
server is  instruct to "remember" the channels in which it found an ensemble
when doing a full scan, so a next scan will be short in time.

Touching the reset button during operation, i.e. after the DAB server
is "operational"  will instruct the server to do the extended scan,
i.e. scan over all channels in the given band - of course the results are
recorded by the server and used the next invocation
to scan the band.

Note that the reset button behaves differently when touched for the first time
or another time. The first time it will scan the preselected channels,
when touched further, it will scan all channels and - apart from
collecting Dab data - will record which channels carry this Dab data.

---------------------------------------------------------------------------
---------------------------------------------------------------------------

We use bluetooth as communication medium between
client and server, the server announces its service with a fancy UUID.

-----------------------------------------------------------------------------
Android clients
-----------------------------------------------------------------------------

A simple android client is being exercised on my tablet.
This client is still pretty simple, and android is subject
to further study, but it is working pretty well.

The picture above shows the client. 
To experiment a little with android, there are two versions, identical
though for different screen formats.

Note that one the user has selected the service of choice, there is
no real need for the app to remain connected.
If the use want to change the selection, just connect, the server
is - operating and not connected - always listening.


---------------------------------------------------------------------
The server
---------------------------------------------------------------------

A number of libraries has to be installed. For stretch on the RPI
one might want to execute the lines

	sudo apt-get update
	sudo apt-get install build-essential g++
	sudo apt-get install libsndfile1-dev libfftw3-dev portaudio19-dev
	sudo apt-get install libfaad-dev zlib1g-dev libusb-1.0-0-dev mesa-common-dev
	sudo apt-get install libgl1-mesa-dev libsamplerate-dev
	sudo apt-get install libbluetooth-dev bluez
	sudo apt-get install cmake

The SDRplay devices, AIRspy devices, "dabsticks" (RTLSDR based devices)
and  Hackrf devices are supported.
When building an executable, ensure that the corresponding
support library for the selected device is installed.

The server - currently - can be configured to support one of
these devices, creating the server - assuming all required libraries are
installed - is by

	mkdir build
	cd build
	cmake .. -DXXX=ON (XXX is either SDRPLAY, AIRSPY, RTLSDR or HACKRF)
	make


The server stores some results - i.e. the channel names where DAB
data night be found, gain setting etc - in an ini file, which
is - by default - kept in /usr/local/src. The name can be changed
in the include file in the "dab-decoder.h" file.

The server will send the audio output for a selected service
to the soundcard of the machine it runs on.

----------------------------------------------------------------------
Running the server as a systemd service
----------------------------------------------------------------------

In my setting the server runs stand alone as a "service", instantiated
on starting up the RPI. Communication - if any - is using bluetooth. 
Obviously, one should have enabled bluetooth on the RPI,
how to instal the bluez software is described in many places
on the internet, and the required libraries are available on the repositories for Stretch.
In order to allow the server to register its "service", and to make the bluetooth 
discoverable, one should add "--compat" to the line

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
section contains an "apk" file, the package is built by android studio and
is the one running on my tablet.

The android remote control has a start button, touching it will instruct the client
to look at bluetooth devices in the neighbourhood. The result is a list
of devices. After touching the name of the device with the server in this list
the client will (try to) establish the connection, assuming a
server runs on the selected device.
.
As soon as a client is connected to a device running the server,
the names of the services are
transmitted to the client, and the server is ready to receive commands.
This is important, since the basic idea is to have short  periods being connected
with the server. After all, having selected a program, there is no reason whatsoever
to remain connected.

Typical commands are selecting a service or changing the
gain setting of the device. Note that different devices require
different methods for gain setting.The client "knows" which device
is used at the server side, and it adapts its GUI wrt to gain setting.

For the SDRplay, the state of the lna and the gain reduction (20 .. 59)
can be set (as well as the autogain).  For the AIRspy the
gain slider is constrained to values from 1 .. 21, and both the spinner
for the lnaState and the button for the autogain are hidden.
For the rtlsdr based devices, the gain setting allows values from
0 .. 100, they will be translated by the device driver software
to appropriate gain values. For the HackRF device, the lna gain can be set
throught he spinner, and the vga gain through the gain slider.

The top slider on the remote control can be used to control the sound setting of the server.
One might have to adapt the code in the server, controlling the sound level
is by using the "amixer" function of alsa on the server device.

If the user is not satisfied with the amount of services shown on the GUI,
and detected by the last scan, a "reset" button on the GUI can be touched that
instructs the server to scan all channels in the old TV band III
and record which channels carry useful data.

To close down the connection, a the user can touch the "quit" button, the server
will continue to do what it was doing (i.e. if the server was decoding a service, it
will continue to do so) and is available for another client.
To change any setting in the server, restart the remote control, connect and instruct.

---------------------------------------------------------------------------
Shutting down the server
----------------------------------------------------------------------------

One thing that imemediately comes to mind is that just pulling out the plug
to disconnect the RPI from its powers source is not advisable, however,
the server runs headless.
Rather than having to establish a TCP connection to instruct the RPI to shut down,
the GUI contains a button to completely shut down the server,
note that this is different from the button to just quit the program,
touching that button will leave the server running.
Note furthermore, that shutting down the server just means shutting down, to
wake up the server (or better: the RPI) one has to restart the RPI.

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
rights gratefully acknowledged. The android client is a first attempt to create some application for android
and leaves quite some room for improvement. Suggestions are welcome/



