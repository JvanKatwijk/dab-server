package bluetooth;

import java.io.IOException;
import java.util.Enumeration;
import java.util.Vector;
import javax.bluetooth.*;
import javax.microedition.io. *;
/**
 *
 * Minimal Services Search example.
 */
public class ServicesSearch {

	final RemoteDeviceDiscovery mySearcher =
	                              new RemoteDeviceDiscovery ();
	final Object serviceSearchCompletedEvent = new Object ();
	UUID radioId;
	final UUID serviceUUID	= new UUID (0x0003);	// RFComm
	final Vector<RemoteDevice> devicesDiscovered =
                                                   new Vector <RemoteDevice> ();
	final Vector<String> serviceFound = new Vector <String> ();

	public StreamConnection findService ()
	                 throws IOException, InterruptedException {

	radioId				= 
                        new UUID ("9ce8f756e97b11e89f32f2801f1b9fd1", false);

//	First run RemoteDeviceDiscovery and use discovered device
	   serviceFound.      clear ();
	   devicesDiscovered. clear ();
	   mySearcher. findDevices (devicesDiscovered);
	   if (devicesDiscovered. size () == 0) {
	      System. out. println ("no devices found, fatal");
	      return null;
	   }

	   for (int i = 0; i < devicesDiscovered. size (); i ++)
	      System. out. println ("Device " + i + " is " +
	                     devicesDiscovered. get (i). getFriendlyName (true));

//	OK, we have device(s), look which one provides the "radio"
//	and RFComm
	   DiscoveryListener listener = new DiscoveryListener () {
	      public void deviceDiscovered (RemoteDevice btDevice,
	                                    DeviceClass cod) {
	      }

	      public void inquiryCompleted (int discType) {
	      }

	      public void servicesDiscovered (int transID,
	                                      ServiceRecord [] servRecord) {
	
	         for (int i = 0; i < servRecord.length; i++) {
	            String url = servRecord [i].
	                           getConnectionURL (ServiceRecord.NOAUTHENTICATE_NOENCRYPT, false);
	            if (url == null) {
	               continue;
	            }

	            serviceFound. add (servRecord [i].
	                                  getHostDevice ().
	                                       getBluetoothAddress ());
	         }
	      }

	      public void serviceSearchCompleted (int transID, int respCode) {
	         System. out. println ("service search completed!");
	         if (serviceFound. size () > 0)
	            System. out. println ("bt address " +
	                       serviceFound. get (0) + " provides radio service");
	         else
	            System. out. println ("no service(s) found"); 
	             
	         synchronized (serviceSearchCompletedEvent){
	            serviceSearchCompletedEvent. notifyAll ();
	         }
	      }
	   };

	   UUID [] searchUuidSet = new UUID[] {radioId};
//	   UUID [] searchUuidSet = new UUID[] {serviceUUID, radioId};
	   int[] attrIDs =  new int[] {
                0x0100 // Service name
	   };

//	   for (Enumeration en = devicesDiscovered. elements ();
//	                                       en.hasMoreElements ();) {
	   for (int i = 0; i < devicesDiscovered. size (); i ++) {
	      RemoteDevice btDevice =
	                     devicesDiscovered. get (i);
	      if (btDevice. getFriendlyName (true). equals ("raspberrypi")) {
	         synchronized (serviceSearchCompletedEvent) {
	            LocalDevice. getLocalDevice ().
	                         getDiscoveryAgent().
	                          searchServices (attrIDs,
	                                          searchUuidSet,
	                                          btDevice, listener);
	            serviceSearchCompletedEvent. wait ();
	         }
	      }
	   }

	   if (serviceFound. size () == 0) {
	      System. out. println ("no radio found, fatal");
	      return null;
	   }
	   String url	= serviceFound. firstElement ();

	   url	= "btspp://" + url + ":1;master=false;encrypt=false;authenticate=false";
	   System. out. println ("Radio Service provided by " + url);
	   StreamConnection clientSession =
	                  (StreamConnection)Connector. open (url);
	   return clientSession;
	}

	public static void Dispatcher (char [] buffer, int filled) {
	   int	starter = 0;
	   while (starter < filled) {
	      int segSize	= buffer [starter + 2];
	      int mType		= buffer [starter];
	      char message []	= new char [segSize + 1];
	      for (int i = 0; i < segSize; i ++)
	         message [i] = buffer [starter + 3 + i];
	      message [segSize] = 0;
	      String m = String. valueOf (message);
	      starter += segSize + 3;
	      switch (mType) {
	         case 0100:
	            System. out. println ("Ensemble " + m);
	            break;
	         case 0101:
	            System. out. println ("Servicename " + m);
	            break;
	         case 0102:
	            System. out. println ("Text message " + m);
	            break;
	         default:
	            System. out. println (m);
	      }
	   }
	}
}
