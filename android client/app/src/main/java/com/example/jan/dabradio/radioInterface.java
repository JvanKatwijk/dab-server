package com.example.jan.dabradio;
import android.bluetooth.BluetoothSocket;
import android.support.v7.app.AppCompatActivity;

import java.io.IOException;
import java. util. *;
import java.io.InputStream;
import java.io.DataInputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.DataOutputStream;
import java.io.OutputStreamWriter;
import java.io.IOException;
import java.io.OutputStream;

//
//	The "radioInterface" is the actual interface to the
//	socket of the bluetooth connector
public class radioInterface extends Thread {
//
//	keys for the outgoing messages
        private final int	Q_QUIT			= 0100;
        private	final int	Q_IF_GAIN_REDUCTION	= 0101;
        private	final int	Q_SOUND_LEVEL		= 0102;
        private	final int	Q_LNA_STATE		= 0103;
        private	final int	Q_AUTOGAIN		= 0104;
        private	final int	Q_SERVICE		= 0106;
        private	final int	Q_RESET			= 0107;
//
//	keys for incoming messages
        private	final int	Q_ENSEMBLE		= 0100;
        private	final int	Q_SERVICE_NAME		= 0101;
        private	final int	Q_TEXT_MESSAGE		= 0102;
        private	final int	Q_PROGRAM_DATA		= 0103;
        private	final int	Q_SNR	                = 0104;
        private	final int	Q_NEW_ENSEMBLE		= 0105;
        private	final int	Q_NO_SERVICE		= 0106;
        private	final int	Q_SYNCED		= 0107;
        private	final int	Q_INITIAL_LNA		= 0110;
        private	final int	Q_INITIAL_GRdB		= 0111;
        private	final int	Q_SOUND			= 0112;
        private final int	Q_SIGNAL_STEREO		= 0113;
//
//	For the connection we need

        private InputStream is;
        private OutputStream os;
        private DataOutputStream outputter;
        private DataInputStream inputter;
//
//      for changing values in the GUI we need
        private AppCompatActivity the_gui;
//
        private	boolean autoGain	= false;
//	for signaling, we need
        private final List<Signals> listener = new ArrayList<>();
        public  void    addServiceListener (Signals service) {
           listener. add (service);
        }

        public radioInterface (AppCompatActivity the_gui) {
       	this. the_gui       = the_gui;
        }

        public void startRadio (BluetoothSocket  thePlug) throws IOException {
            is               = thePlug. getInputStream  ();
            os               = thePlug. getOutputStream ();
            inputter         = new DataInputStream (is);
            outputter        = new DataOutputStream (os);
            start ();
        }

        @Override
        public void run () {
            char inBuffer[] = new char [2550];
            byte header  [] = new byte [300];
            while (true) { 
                int amount	= 0;
                int length	= 0;
                try {
                    Thread.sleep(100);
                } catch (Exception e) {}
                try {
                    for (int i = 0; i < 3; i ++)
                        header [i] = inputter. readByte ();

                    length = (int)(((header [1] & 0xFF) << 8) |
                                           (int)(header [2] & 0xFF));
         //           toaster ("Gelezen " + amount + " lengte is " + length);
                    for (int i = 0; i < length; i ++)
                        inBuffer [i] = (char) inputter. readByte ();
                    Dispatcher (header [0], length, inBuffer);
                } catch (Exception e) {
                    toaster ("length = " + length + " amount = " + amount);
                }
            }
        }
//
//	This function is called from within the thread
        public void Dispatcher (byte key, int segSize, char [] inBuffer) {
           char buffer [] = new char [segSize + 1];

           System. arraycopy (inBuffer, 0, buffer, 0, segSize);

           switch (key) {
              case Q_SOUND:  // not yet implemented
              break;

              case Q_ENSEMBLE: {
                  buffer [segSize]	= 0;
                  final String ensembleLabel = String. valueOf (buffer);
                  try {
                      the_gui. runOnUiThread (new Runnable () {
                          @Override
                          public void run () {
                             show_ensembleName (ensembleLabel, 0);
                          }});
                  } catch (Exception e) {}
               }
               break;

               case Q_SERVICE_NAME: {
                   buffer [segSize] = 0;
                   final String serviceLabel = String. valueOf (buffer);
                   try {
                       the_gui. runOnUiThread (new Runnable () {
                          @Override
                          public void run () {
                             show_serviceName (serviceLabel);
                        }});
                    } catch (Exception e) {}
                }
                break;

                case Q_TEXT_MESSAGE: {
                    buffer [segSize] = 0;
                    final String dynamicLabel = String. valueOf (buffer);
                    try {
                        the_gui. runOnUiThread (new Runnable () {
                            @Override
                            public void run () {
                                show_dynamicLabel (dynamicLabel);
                        }});
                    } catch (Exception e) {}
               }
               break;

               case Q_PROGRAM_DATA: {
                   buffer [segSize] = 0;
                   final String programData = String. valueOf (buffer);
                   try {
                       the_gui. runOnUiThread (new Runnable () {
                          @Override
                          public void run () {
                              show_programData (programData);
                       }});
                   } catch (Exception e) {}
               }
               break;

               case Q_SNR: {
                   final int snr = buffer [0];
                   try {
                       the_gui. runOnUiThread (new Runnable () {
                          @Override
                          public void run () {
                             show_snr (snr);
                        }});
                   } catch (Exception e) {}
               }
               break;

               case Q_SYNCED: {
                   final boolean synced = buffer [0] != 0;
                   try {
                       the_gui. runOnUiThread (new Runnable () {
                          @Override
                          public void run () {
                             show_synced (synced);
                       }});
                   } catch (Exception e) {}
               }
               break;

               case Q_INITIAL_LNA: {
                   final int initialLNA = buffer [0];
                   try {
                       the_gui. runOnUiThread (new Runnable () {
                          @Override
                          public void run () {
                             set_lnaState (initialLNA);
                       }});
                   } catch (Exception e) {}
               }
               break;

               case Q_INITIAL_GRdB: {
                  final int initialGRdB = buffer [0];
                  try {
                      the_gui. runOnUiThread (new Runnable () {
                         @Override
                         public void run () {
                            set_GRdB (initialGRdB);
                      }});
                  } catch (Exception e) {}
               }
               break;

               case Q_NEW_ENSEMBLE: {
                   try {
                       the_gui. runOnUiThread (new Runnable () {
                           @Override
                           public void run () {
                              clearScreen ();
                       }});
                   } catch (Exception e) {}
               }
               break;

               case Q_NO_SERVICE: {
                   final String s = "no service found";
                   try {
                       the_gui. runOnUiThread (new Runnable () {
                           @Override
                           public void run () {
                              show_dynamicLabel (s);
                       }});
                   } catch (Exception e) {}
               }
               break;

               case Q_SIGNAL_STEREO: {
                   final boolean b = buffer [0] != 0;
                   try {
                       the_gui. runOnUiThread (new Runnable () {
                           @Override
                           public void run () {
                              set_stereoIndicator (b);
                       }});
                   } catch (Exception e) {}
               }
               break;

               default:;
           }
        }

        public  void show_ensembleName (String Name, int Sid) {
            int i;
            for (i = 0; i < listener. size (); i ++) {
                Signals handler = listener. get (i);
                handler. show_ensembleName (Name, Sid);
            };
        }

        public void show_serviceName (String s) {
            int i;
            for (i = 0; i < listener. size (); i ++) {
                Signals handler = listener. get (i);
                handler. show_serviceName (s);
            };
        }

        public	void	show_dynamicLabel (String s) {
            int i;
            for (i = 0; i < listener. size (); i ++) {
                Signals handler = listener. get (i);
                handler. show_dynamicLabel (s);
            };
        }

        public	void	show_programData (String s) {
            int i;
            for (i = 0; i < listener. size (); i ++) {
                Signals handler = listener. get (i);
                handler. show_programData (s);
            };
        }

        public	void	clearScreen () {
            int i;
            for (i = 0; i < listener. size (); i ++) {
                Signals handler = listener. get (i);
                handler. clearScreen ();
            };
        }

        public	void	show_signalQuality (int q) {
            int i;
            for (i = 0; i < listener. size (); i ++) {
                Signals handler = listener. get (i);
                handler. show_signalQuality (q);
            };
        }

        public	void	show_synced	(boolean b) {
            int i;
            for (i = 0; i < listener. size (); i ++) {
                Signals handler = listener. get (i);
                handler. show_synced (b);
            };
        }

        public	void	set_lnaState	(int initialLNA) {
            int i;
            for (i = 0; i < listener. size (); i ++) {
                Signals handler = listener. get (i);
                handler. set_lnaState	(initialLNA);
            };
        }

        public	void	set_GRdB	(int initialGRdB) {
            int i;
            for (i = 0; i < listener. size (); i ++) {
                Signals handler = listener. get (i);
                handler. set_GRdB	(initialGRdB);
            };
        }

        public	void	set_stereoIndicator	(boolean b) {
            int i;
            for (i = 0; i < listener. size (); i ++) {
                Signals handler = listener. get (i);
                handler. set_stereoIndicator (b);
            };
        }

        public  void  toaster (String s) {
            final String data = s;
            try {
                the_gui.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        the_toaster (data);
                    }
                });
            } catch (Exception e) {
            }
        }

         public void the_toaster (String s) {
              for (int i = 0; i < listener. size (); i ++) {
                  Signals handler = listener. get (i);
                  handler. toaster (s);
              }
         }
//
//	and the transmitters
        public	void	setService (String s) {
           byte data [] = new byte [s. length () + 4];
           data [0] = (byte)Q_SERVICE;
           data [1] = (byte)0;
           data [2] = (byte) (s. length () + 1);
           for (int i = 0; i < s. length (); i ++)
              data [3 + i] = (byte) s. charAt (i);
           data [3 + s. length ()] = 0;
           toaster ("Setting service for " + s);
           try {
              outputter. write (data, 0, s. length () + 4);
              outputter. flush ();
           } catch (Exception e) {}
        }

        public	void	doReset	() {
           byte data [] = new byte [4];
           data [0] = (byte)Q_RESET;
           data [1] = (byte)0;
           data [2] = (byte) (1);
           data [3] = (byte)0;
           try {
              outputter. write (data, 0, 4);
              outputter. flush ();
           } catch (Exception e) {}
        }

        public	void	setLnaState	(int lnaState) {
           byte data [] = new byte [3 + 2];
           data [0] = (byte)Q_LNA_STATE;
           data [1] = (byte)0;
           data [2] = (byte)2;
           data [3] = (byte)lnaState;
           try {
              outputter. write (data, 0, 5);
              outputter. flush ();
           } catch (Exception e) {}
        }

        public	void	gainReduction	(int GRdB) {
           byte data [] = new byte [3 + 2];
           data [0] = (byte)Q_IF_GAIN_REDUCTION;
           data [1] = (byte)0;
           data [2] = (byte)2;
           data [3] = (byte)GRdB;
           try {
              outputter. write (data, 0, 5);
              outputter. flush ();
           } catch (Exception e) {}
        }

        public	void	set_soundLevel	(int soundLevel) {
           byte data [] = new byte [3 + 2];
           data [0] = (byte)Q_SOUND_LEVEL;
           data [1] = (byte)0;
           data [2] = (byte)2;
           data [3] = (byte)soundLevel;
           try {
              outputter. write (data, 0, 5);
              outputter. flush ();
           } catch (Exception e) {}
        }

        public	void	set_autoGain	() {
           byte data [] = new byte [3 + 2];
           data [0] = (byte)Q_AUTOGAIN;
           data [1] = (byte)0;
           data [2] = (byte)2;
           autoGain	= !autoGain;
           data [3] = (byte)(autoGain ? 1 : 0);
           try {
              outputter. write (data, 0, 5);
              outputter. flush ();
           } catch (Exception e) {}
        }

};
