package model;
import java.io.IOException;
import java. util. *;
import java.io.InputStream;
import java.io.DataInputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.DataOutputStream;
import java.io.OutputStreamWriter;
import javax.bluetooth.*;
import javax.microedition.io. *;

//
//	The "Model" is the actual interface to the bluetooth connector
//	The start is - obviously - building a connection, and
//	once enabled, the run function will read the incoming
//	data and pass that on to the external world.
public class ClientModel extends Thread {
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
	private	final int	Q_INITIALS		= 0100;
	private final int	Q_DEVICE_NAME		= 0101;
	private	final int	Q_ENSEMBLE		= 0102;
	private	final int	Q_SERVICE_NAME		= 0103;
	private	final int	Q_TEXT_MESSAGE		= 0104;
	private final int	Q_STATE			= 0105;
	private final int	Q_STEREO		= 0106;
	private	final int	Q_NEW_ENSEMBLE		= 0107;
	private	final int	Q_SOUND			= 0110;
//
//	For the connection we need
	private final StreamConnection thePlug;
	private final DataOutputStream outputter;
	OutputStreamWriter bWriter;
	private final DataInputStream inputter;
	private final InputStreamReader bReader;
//

	private	boolean autoGain	= false;
//	for signaling, we need
	private final List<modelSignals> listener = new ArrayList<>();
        public  void    addServiceListener (modelSignals service) {
           listener. add (service);
        }

	public ClientModel (StreamConnection thePlug) throws IOException {
	   this. thePlug	= thePlug;
	   outputter		= thePlug. openDataOutputStream ();
           bWriter		= new OutputStreamWriter (outputter);
           inputter		= thePlug. openDataInputStream ();
           bReader		= new InputStreamReader (inputter);
	}

	public	void startRadio () {
	   start ();
	}

	@Override 
	public void run () {
           char inBuffer[] = new char [255];
	   char header  [] = new char [3];
	   while (true) { 
	      int amount	= 0;
	      int length	= 0;
	      try {
	         amount =  bReader. read (header, 0, 3);
	         length = (int)(((header [1] & 0xFF) << 8) |
	                                  (int)(header [2] & 0xFF));
	         amount = bReader. read (inBuffer, 0, length);
                 Dispatcher (header [0], length, inBuffer);
	      } catch (Exception e) {
	      System. out. println ("length = " + length +
	                                      " amount = " + amount);
	      }
           }
	}

//
//	This function is called from within the thread
	public void Dispatcher (char key, int segSize, char [] inBuffer) {
	   char buffer [] = new char [segSize + 1];
	   System. arraycopy (inBuffer, 0, buffer, 0, segSize);

	   switch (key) {
	      case Q_SOUND:	// not yet implemented
	         break;

	      case Q_INITIALS: {
	         final char sysdata [] = new char [segSize + 1];
	         for (int i = 0; i < segSize + 1; i ++)
	            sysdata [i] = inBuffer [i];
//	         System. arraycopy (inBuffer, 0, sysdata, 0);
	         try {
	            javax. swing. SwingUtilities.
	                         invokeLater (new Runnable () {
                          @Override
                          public void run () {
	                     set_initialValues (sysdata);
	            }});
                 } catch (Exception e) {}
	      }
	      break;

	      case Q_DEVICE_NAME: {
	         buffer [segSize]	= 0;
	         final String deviceName = String. valueOf (buffer);
	         try {
	             javax. swing. SwingUtilities.
	                         invokeLater (new Runnable () {
                         @Override
                         public void run () {
	                    show_deviceName (deviceName);
	             }});
                 } catch (Exception e) {}
	      }
	      break;
	                         
	      case Q_ENSEMBLE: {
	         buffer [segSize]	= 0;
	         final String ensembleLabel = String. valueOf (buffer);
	         try {
	             javax. swing. SwingUtilities.
	                         invokeLater (new Runnable () {
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
	             javax. swing. SwingUtilities.
	                         invokeLater (new Runnable () {
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
	             javax. swing. SwingUtilities.
	                         invokeLater (new Runnable () {
                         @Override
                         public void run () {
	                     show_dynamicLabel (dynamicLabel);
	             }});
                 } catch (Exception e) {}
	      }
	         break;

	      case  Q_STATE: {
	         final char vector [] = new char [2];
	         vector [0] = buffer [0];
	         vector [1] = buffer [1];
	         try {
	             javax. swing. SwingUtilities.
	                            invokeLater (new Runnable () {
                         @Override
                         public void run () {
	                     show_state (vector);
	             }});
                 } catch (Exception e) {}
	      }
	      break;

	      case Q_NEW_ENSEMBLE:
	         try {
	            javax. swing. SwingUtilities.
	                          invokeLater (new Runnable () {
	               @Override
	               public void run () {
	                  clearScreen ();
	               }});
	         } catch (Exception e) {}
	         break;

	      case Q_STEREO:
	         try {
	              final boolean b = buffer [0] != 0;
	              javax. swing. SwingUtilities.
	                         invokeLater (new Runnable () {
                          @Override
                          public void run () {
	                     set_stereoIndicator (b);
	              }});
                 } catch (Exception e) {}
	         break;

	      default:;
	   }
	}

	public	void	show_deviceName (String name) {
	   listener. forEach ((hl) -> {
	         hl. show_deviceName (name);
	   });
	}

	public	void	set_initialValues (char [] v) {
	   listener. forEach ((hl) -> {
	        hl. set_initialValues (v);
	   });
	}

	public  void show_ensembleName (String Name, int Sid) {
           listener. forEach((hl) -> {
                hl. show_ensembleName (Name, Sid);
            });
        }

	public void show_serviceName (String s) {
           listener.forEach((hl) -> {
               hl. show_serviceName (s);
            });
        }

	public	void	show_dynamicLabel (String s) {
	   listener. forEach ((hl) -> {
	       hl. show_dynamicLabel (s);
	   });
	}

	public	void	show_state (char [] s) {
	   listener. forEach ((hl) -> {
	      hl. show_state (s);
	   });
	}

	public	void	clearScreen () {
	   listener. forEach ((hl) -> {
	      hl. clearScreen ();
	   });
	}

	public	void	set_stereoIndicator	(boolean b) {
	   listener. forEach ((hl) -> {
	      hl. set_stereoIndicator (b);
	   });
	}

	public	void	setService (String s) {
	   char data [] = new char [s. length () + 4];
	   data [0] = Q_SERVICE;
	   data [1] = 0;
	   data [2] =  (char) (s. length () + 1);
	   for (int i = 0; i < s. length (); i ++)
	      data [3 + i] = s. charAt (i);
	   data [3 + s. length ()] = 0;
	   try {
	      bWriter. write (data, 0, s. length () + 4);
	      bWriter. flush ();
	   } catch (Exception e) {}
	}

	public	void	reset	() {
	   char data [] = new char [4];
	   data [0] = Q_RESET;
	   data [1] = 0;
	   data [2] = (char) (1);
	   data [3] = 0;
	   try {
	      bWriter. write (data, 0, 4);
	      bWriter. flush ();
	   } catch (Exception e) {}
	}

	public	void	setLnaState	(int lnaState) {
	   char data [] = new char [3 + 2];
	   data [0] = Q_LNA_STATE;
	   data [1] = 0;
	   data [2] = 2;
	   data [3] = (char)lnaState;
	   try {
	      bWriter. write (data, 0, 5);
	      bWriter. flush ();
	   } catch (Exception e) {}
	}

	public	void	gainReduction	(int GRdB) {
	   char data [] = new char [3 + 2];
	   data [0] = 0101;
	   data [1] = 0;
	   data [2] = 2;
	   data [3] = (char)GRdB;
	   try {
	      bWriter. write (data, 0, 5);
	      bWriter. flush ();
	   } catch (Exception e) {}
	}

	public	void	set_soundLevel	(int soundLevel) {
	   char data [] = new char [3 + 2];
	   data [0] = Q_SOUND_LEVEL;
	   data [1] = 0;
	   data [2] = 2;
	   data [3] = (char)soundLevel;
	   try {
	      bWriter. write (data, 0, 5);
	      bWriter. flush ();
	   } catch (Exception e) {}
	}

	public	void	set_autoGain	() {
	   char data [] = new char [3 + 2];
	   data [0] = Q_AUTOGAIN;
	   data [1] = 0;
	   data [2] = 2;
	   autoGain	= !autoGain;
	   data [3] = (char)(autoGain ? 1 : 0);
	   try {
	      bWriter. write (data, 0, 5);
	      bWriter. flush ();
	   } catch (Exception e) {}
	}

};
