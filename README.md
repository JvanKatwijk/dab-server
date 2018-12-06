
	STAND ALONE DAB-SERVER with an android client as remote control

---------------------------------------------------------------------

![android client for dab server](/android-client.png?raw=true)

The RPI2 and 3 are excellent devices to use as a specialized
server, they are small but powerfull enough to do all kinds of 
computations.

In this project a DAB server, running on an headless RPI2/3 (under
Stretch) is being developed. The DAB server is started when turning
on the RPI 2 and is controlled by an android client, using a bluetooth
connection. When properly installed, the server will run without
any further action from the user, i.e the RPI can run headless, even without
an ssh connection.

The server, when started, collects services in ensembles
found in the different channels in Band III, and - when this is finished -
is waiting for someone to call.

Since most of the channels do not carry DAB data, the server is  instruct to "remember"
in which channels it found an ensemble whn doing a full scan. The scan done on start-up
is then limited to those channels that previously have shown to contain DAB data.

The clients contain a "reset" button, instructing the server to scan all channels in the
given band.

We use bluetooth as communication medium between
client and server, the server announces its service with a fancy UUID.

A simple android client is being exercised on my tablet.
The client is still pretty simple, and android is subject
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

The server will send its audio output to the soundcard of
the machine it runs on.

----------------------------------------------------------------------

Running the server as a systemd service

----------------------------------------------------------------------

Obviously, one should have enabled bluetooth on the RPI,
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
in the directory /home/pi


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



