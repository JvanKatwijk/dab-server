package com.example.jan.dabradio;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.os.CountDownTimer;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.util.ArrayList;
import java.util.UUID;

public class MainActivity extends AppCompatActivity implements Signals {
    private final   int     S_SDRPLAY       = 001;
    private final   int     S_DABSTICK      = 002;
    private final   int     S_AIRSPY        = 003;
    private final   int     S_NOBODY        = 000;

    private TextView            gainLabel;
    private TextView            audioLabel;
    private TextView            syncLabel;
    private TextView            stereoLabel;
    private TextView            dynamicLabel;
    private TextView            ensembleLabel;
    private TextView            tStatus;
    private TextView            snrLabel;
    private TextView            deviceLabel;
    private TextView            serviceLabel;
    private ArrayAdapter<String> BTArrayAdapter;
    private ArrayList<Object>   BTResultMac;
    private CountDownTimer      scanTimer;
    private Button              startButton;
    private Button              resetButton;
    private Button              autogainButton;
    private Button              quitButton;
    private SeekBar             gainSlider;
    private SeekBar             audioGain;
    private Spinner             lnaState;
    private ListView            services;
    private ArrayList<String>   theServices;
    static ArrayAdapter<String> serviceAdapter;
    private ListView            lResult;
    private BluetoothAdapter    myBluetoothAdapter;
    private BluetoothDevice     device;
    private BluetoothSocket     btSocket;
    private radioInterface      my_radioInterface;
    public  MainActivity        the_gui;
    private static final int REQUEST_ENABLE_BT = 1;
//
//  for now we assume that we have an RSP II
    Integer [] lnastateValues = new Integer [] {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    static ArrayAdapter<Integer> lnaAdapter;
    static ArrayList<String> serviceItems;
    private static UUID my_uuid = UUID.fromString ("00001101-0000-1000-8000-00805f9b34fb");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate (savedInstanceState);
        setContentView (R.layout.activity_main);
//
// just save the context
        the_gui         = this;
        gainLabel       = (TextView)findViewById (R. id. gainLabel);
        syncLabel       = (TextView)findViewById (R. id. syncLabel);
        stereoLabel     = (TextView)findViewById (R. id. stereoLabel);
        dynamicLabel    = (TextView)findViewById (R. id. dynamicLabel);
        tStatus         = (TextView)findViewById (R. id. statusLabel);
        tStatus.    setBackgroundColor (Color. YELLOW);
        tStatus.    setTextColor (Color. BLACK);
        snrLabel        = (TextView)findViewById (R. id. snrLabel);
        startButton     = (Button)  findViewById (R. id. startButton);
        startButton. setBackgroundColor (Color. GREEN);
        quitButton      = (Button)  findViewById (R. id. quitButton);
        quitButton.  setBackgroundColor (Color. RED);
        autogainButton  = (Button)  findViewById (R. id. autogainButton);
        autogainButton. setEnabled (false);
        resetButton     = (Button)  findViewById (R. id. resetButton);
        resetButton. setBackgroundColor (Color.  YELLOW);
        autogainButton. setBackgroundColor (Color. GREEN);
        resetButton.    setEnabled (false);
        gainSlider      = (SeekBar) findViewById (R. id. gainSlider);
        gainSlider.  setEnabled (false);
        lnaState        = (Spinner) findViewById (R. id. lnaState);
        lnaState.       setEnabled (false);
        audioGain       = (SeekBar) findViewById (R. id. audioGain);
        audioGain.      setEnabled (false);
        audioLabel      = (TextView)findViewById (R. id. audioLabel);
        services        = (ListView)findViewById (R. id. services);
        lResult         = (ListView) findViewById(R. id. lResult);
        ensembleLabel   = (TextView)findViewById (R. id. ensembleLabel);
        deviceLabel     = (TextView)findViewById (R. id. deviceLabel);
        serviceLabel    = (TextView)findViewById (R. id. serviceLabel);

        myBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        my_radioInterface = new radioInterface (the_gui);
        my_radioInterface. addServiceListener (the_gui);

        lnaAdapter = new ArrayAdapter<Integer> (this,
                                        android. R. layout. simple_spinner_item, lnastateValues);
        serviceItems    = new ArrayList<String> ();
        serviceAdapter  = new ArrayAdapter<String> (this,
                                         android.R.layout. simple_list_item_1,
                                                                serviceItems);
        theServices     = new ArrayList<String> ();

        gainSlider. setMax (59);
        gainSlider. setProgress (33);
        gainLabel.  setText (Integer. toString (33));
        lnaState.   setAdapter (lnaAdapter);
        audioGain.  setMax (100);
        audioGain.  setProgress (50);
        audioLabel. setText (Integer. toString (50));

        services.   setAdapter (serviceAdapter);

//	touching the spinner
        lnaState. setOnItemSelectedListener(new AdapterView.
                                            OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parentView,
                                       View selectedItemView,
                                       int position, long id) {
                try {
                    serviceAdapter.notifyDataSetChanged();
                    String text = lnaState. getSelectedItem().toString();
                    my_radioInterface. setLnaState (Integer. parseInt(text));
                } catch (Exception e) {
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parentView) {
                try {
                    ;
                } catch (Exception e) {}
            }
        });
//
        lResult.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick (AdapterView<?> parent, View v,
                                      int pos, long id) {
                BluetoothDevice device = myBluetoothAdapter.
                      getRemoteDevice((String) BTResultMac.get (pos));
                tStatus. setText ((String) BTResultMac. get (pos));
                if (myBluetoothAdapter.isDiscovering()){
                    myBluetoothAdapter.cancelDiscovery();
                }

                try {
//                  UUID my_uuid = UUID.fromString ("0x00001101-0000-1000-8000-00805f9b34fb");
//                  UUID my_uuid = UUID. fromString ("04c6093b-0000-1000-8000-00805f9b34fb");
                    btSocket = device.createInsecureRfcommSocketToServiceRecord (my_uuid);
                } catch (IOException e) {
                    toaster ("socket create failed: " + e.getMessage() + ".");
                    return;
                }
                toaster ("Now going to connect");

                try {
                    btSocket.connect();
                } catch (IOException e) {
                    e.printStackTrace (System.err);
                    tStatus.setText ("Connecting Failed");
                    return;
                }

                try {
                    my_radioInterface. startRadio (btSocket);
                } catch (IOException e) {
                    e.printStackTrace (System.err);
                    tStatus.setText ("Connecting Failed");
                    toaster ("Instantiation radioInterface errored");
                    return;
                }

                tStatus.setText ("Connected");
                toaster ("Connected, we're on");
                resetButton.       setEnabled (true);
                gainSlider.        setEnabled (true);
                autogainButton.    setEnabled (true);
                lnaState.          setEnabled (true);
                audioGain.         setEnabled (true);
            }
        });

//	touching an element of the services list
        services. setOnItemClickListener (new AdapterView.
                                                   OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parentView, View view,
                                    int position, long id) {
                try {
	            my_radioInterface.
	                      setService (theServices. get (position));
                    serviceLabel. setText (theServices. get (position));
                } catch (Exception e) {
                }
            }
        });
//
//      Open Button
        startButton. setOnClickListener (new View.OnClickListener() {
            public void onClick (View v) {
                int result = connectRadio ();
            }
        });

//
//      Reset Button
        resetButton. setOnClickListener (new View. OnClickListener () {
            public void onClick (View v) {
                doReset ();
            }
        });
//
//	Quit Button
        quitButton. setOnClickListener (new View. OnClickListener () {
            public void

            onClick (View v) {
                if (btSocket. isConnected ())
                    try {
                        btSocket. close ();
                    } catch (IOException e) {}
                finish ();
                System.exit(0);
            }
        });
//
//	Touching the gainSlider
	gainSlider. setOnSeekBarChangeListener (
                                     new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged (SeekBar seekBar,
                                           int progressValue,
                                           boolean fromUser) {
                my_radioInterface. set_gainLevel (progressValue);
	        gainLabel. setText (String. valueOf (progressValue));
            }

            @Override
            public void onStartTrackingTouch (SeekBar seekBar) {
            }
            @Override
            public void onStopTrackingTouch (SeekBar seekBar) {
            }
        });
//
//	Touching the audioGain
	audioGain. setOnSeekBarChangeListener (
                                     new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged (SeekBar seekBar,
                                           int progressValue,
                                           boolean fromUser) {
                my_radioInterface. set_gainLevel (progressValue);
	        audioLabel. setText (String. valueOf (progressValue));
            }

            @Override
            public void onStartTrackingTouch (SeekBar seekBar) {
            }
            @Override
            public void onStopTrackingTouch (SeekBar seekBar) {
            }
        });
    }


    int connectRadio () {
        myBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (myBluetoothAdapter == null) {
            tStatus. setText ("No bluetooth adapter available");
            return -1;
        } else {
            if (!myBluetoothAdapter.isEnabled())
                startActivityForResult (
                        new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE),
                        REQUEST_ENABLE_BT);
            tStatus. setText("Status: BlueTooth Enabled");
            BTArrayAdapter = new ArrayAdapter<String>(this,
                                       android.R.layout.simple_list_item_1);
            BTResultMac = new ArrayList<Object>();
            lResult. setAdapter(BTArrayAdapter);
        }

        BTArrayAdapter. clear ();
        BTResultMac.    clear ();
        serviceAdapter. clear ();
        serviceItems.   clear ();

        myBluetoothAdapter.startDiscovery();
        registerReceiver (bReceiver,
                      new IntentFilter(BluetoothDevice.ACTION_FOUND));
        tStatus.setText ("Status: Discovering...");

        scanTimer = new CountDownTimer(15000,1000){
            @Override
            public void onTick (long millisUntilFinished) {}
            @Override
            public void onFinish () {
                myBluetoothAdapter. cancelDiscovery ();
                tStatus.setText ("Status: Search Finished");
            }
        };
        scanTimer.start();
        return 0;
    }

    final BroadcastReceiver bReceiver = new BroadcastReceiver() {
        public void onReceive (Context context, Intent intent) {
            String action = intent. getAction();
            if (BluetoothDevice. ACTION_FOUND. equals (action)) {
                BluetoothDevice device =
                    intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                BTArrayAdapter. add(device.getName() + "\n" + device.getAddress());
                BTArrayAdapter. notifyDataSetChanged ();
                BTResultMac .add (device. getAddress ());
                toaster ("adding " + device. getAddress ());
            }
        }
    };
//
//	for short messages we need a centralized toaster
    public void toaster (String s) {
        Toast. makeText (this. getApplicationContext (), s, Toast.LENGTH_LONG).show();
    }
//
//	The signals (should) lead to changing the GUI

    public void  doReset () {
        theServices. clear ();
        serviceAdapter. clear ();
        my_radioInterface. doReset ();
    }

    public  void    show_deviceName         (String s1) {
        deviceLabel. setText (s1);
    }

    public  void    set_initialValues       (byte [] v) {
        int device = (int)(v [0]);
        switch (device) {
            case S_DABSTICK:
                break;

            case S_AIRSPY:
                lnaState. setVisibility (View. GONE);
                autogainButton. setVisibility (View. GONE);
                gainSlider. setMax (21);
                gainSlider. setProgress ((int)(v [5]));
                gainLabel. setText (Integer. toString ((int)(v [5])));
                break;

            case S_SDRPLAY:
                lnaState. setVisibility (View. VISIBLE);
                autogainButton. setVisibility (View. VISIBLE);
                gainSlider. setVisibility (View. VISIBLE);
                gainSlider. setMax (59);
                gainSlider. setProgress ((int)(v [7]));
                gainLabel. setText (Integer. toString ((int)v [7]));
                lnaState. setSelection ((int)(v [6]));
                break;
          
            default:
                break;
        }
    }

    public  void    show_ensembleName       (String s1, int s2) {
        ensembleLabel. setText (s1);
    }

    public  void    show_serviceName        (String s1) {
        serviceItems. add (s1);
        serviceAdapter. notifyDataSetChanged ();
        theServices. add (s1);
    }

    public  void    show_dynamicLabel       (String s) {
        dynamicLabel. setText (s);
    }

    public  void    show_state      (byte [] v) {
        if (v [0] != 0)
            syncLabel.setBackgroundColor (Color.GREEN);
        else
            syncLabel.setBackgroundColor (Color.RED);
        snrLabel. setText (Integer. toString (v [1]));
    }

    public  void    clearScreen             () {}

    public  void    set_stereoIndicator     (boolean b) {
        if (b) 
            stereoLabel. setBackgroundColor (Color. GREEN);
        else
            stereoLabel. setBackgroundColor (Color. RED);
    }

    public void     set_quit                () {
        if (btSocket. isConnected ()) {
            try {
                btSocket. close ();
            } catch (IOException e) {}
        }

        finish ();
        System.exit(0);
    }

};

