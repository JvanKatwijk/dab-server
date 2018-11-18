package viewer;
import	java.awt.*;
import	javax.swing.*;
import	javax.swing.JOptionPane;
import	javax.swing.JPanel;
import	java.awt.event.*;
import	java.io.ByteArrayInputStream;
import  java.util.*;
import  javax.imageio.ImageIO;
import  javax.imageio.ImageReadParam;
import  javax.imageio.ImageReader;
import  javax.imageio.stream.ImageInputStream;
import	javax.swing.event.*;
import	javax. swing. JSlider;
import	java. awt. Dimension;

public class ClientView extends JFrame {
	private static final long serialVersionUID = 1234567;
//... Components
	private final	JLabel	m_dynamicLabel	= new JLabel ("           ");
	private	final	JSlider	m_gainSlider	=
	                    new JSlider (JSlider. HORIZONTAL, 20, 59, 30);
	private	final	JButton	m_resetButton	= new JButton ("reset");
	private final	JSpinner m_lnaState	=
	                     new JSpinner (new SpinnerNumberModel (2, 0, 8, 1));
	private	final	JButton	m_autogainButton	= new JButton ("auto");
	private		boolean	autogainState		= false;
	private	final	JLabel	m_gainLabel	= new JLabel ("30");
	private final	JLabel	m_qualityLabel	= new JLabel ("0");
	private	final	JLabel	m_syncedLabel	= new JLabel ("sync");
	private	final	JSpinner m_soundLevel	=
	                     new JSpinner (new SpinnerNumberModel (0, -10, 10, 1));
	private final	JLabel m_ensembleLabel	= new JLabel ("ensemble");
	private final	JLabel m_selectedService= new JLabel ("wait for services");
	private final	JLabel m_stereoIndicator = new JLabel ("st");
	
	private final	JLabel m_channelLabel	= new JLabel ("          ");
	public serviceTable m_serviceTable	= new serviceTable ();
	private java.util.List<viewSignals> listeners = new ArrayList<>();
        public  void    addServiceListener (viewSignals service) {
           listeners. add (service);
        }
    
    //======================================================= constructor
    /** Constructor */
	public ClientView () {
        //... Set up the logic
	   m_selectedService. setToolTipText ("the name of the selected service");
	   m_resetButton. 	setBackground (Color. red);
	   m_resetButton. 	setOpaque (true);
	   m_resetButton. 	setToolTipText ("Pushing will instruct the server to scan over all channels");
	   m_stereoIndicator.	setBackground (Color. red);
	   m_stereoIndicator.	setOpaque (true);
	   m_stereoIndicator.	setToolTipText ("green: stereo signal");

	   m_autogainButton.	setBackground	(Color. green);
	   m_autogainButton.	setOpaque (true);	
	   m_autogainButton.	setText ("autogain off");
	   m_selectedService.	setBackground (Color. red);
	   m_selectedService.	setOpaque (true);
	   m_gainSlider.	setPreferredSize (new Dimension (150, 30));
	   m_syncedLabel.	setBackground (Color. red);
	   m_syncedLabel.	setOpaque (true);
	   m_syncedLabel.	setToolTipText ("green: decoding might succeed");
	   m_qualityLabel.	setToolTipText ("snr");
	   m_lnaState.		setToolTipText ("set the lna state of the SDRplay");
//... Layout the components.      
           JPanel row1 = new JPanel();
	   row1.setLayout(new BoxLayout (row1, BoxLayout. X_AXIS));
	   row1. add (m_resetButton);
	   row1. add (Box. createRigidArea (new Dimension (20, 0)));
	   row1. add (m_channelLabel);
	   row1. add (Box. createRigidArea (new Dimension (20, 0)));
	   row1. add (m_ensembleLabel);
	   row1. add (Box. createRigidArea (new Dimension (20, 0)));
	   row1. add (m_qualityLabel);
	   row1. add (Box. createRigidArea (new Dimension (10, 0)));
	   row1. add (m_stereoIndicator);
	
	   row1. add (Box. createRigidArea (new Dimension (20, 0)));
	   row1. add (m_syncedLabel);
	   row1. add (Box. createRigidArea (new Dimension (20, 0)));
	   row1. add (m_selectedService);

	   JPanel row2	= new JPanel ();
	   row2. setLayout (new BoxLayout (row2, BoxLayout. X_AXIS));
	   row2. add (m_dynamicLabel);

	   JPanel row3	= new JPanel ();
	   row3. add (m_soundLevel);
	   m_soundLevel. setToolTipText ("set audio level here");
	   row3. add (Box. createRigidArea (new Dimension (30, 0)));
	   row3. add (m_autogainButton);
	   row3. add (Box. createRigidArea (new Dimension (30, 0)));
	   row3. add (m_gainSlider);
	   m_gainSlider. setToolTipText ("The Gain Reduction 20 .. 59");
	   row3. add (m_gainLabel);
	   row3. add (Box. createRigidArea (new Dimension (50, 0)));
	   row3. add (m_lnaState);

	   JPanel frame	= new JPanel ();
	   frame. setLayout (new BoxLayout (frame, BoxLayout. Y_AXIS));
	   frame. add (row1);
	   frame. add (row2);
	   frame. add (Box. createRigidArea (new Dimension (40, 20)));
	   frame. add (row3);
	   frame. add (Box. createRigidArea (new Dimension (40, 0)));
	   JPanel gui		= new JPanel ();
	   gui. setLayout (new BoxLayout (gui, BoxLayout. Y_AXIS));
	   gui. add (frame);
	   //... finalize layout
	   this. setContentPane (gui);
	   this. pack ();
        
	   this. setTitle("Jan's dabradio client - MVC");
	   // The window closing event should probably be passed to the 
	   // Controller in a real program, but this is a short example.
//	   this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

	   m_serviceTable. table. addMouseListener (new MouseAdapter() {
              @Override
              public void mouseClicked (MouseEvent evt) {
                 if (evt. getClickCount() > 0) {
                    int row     = m_serviceTable. table. rowAtPoint (evt. getPoint ());
                    Object o    = m_serviceTable. table. getValueAt (row, 0);
                    if (!(o instanceof String))
                       return;
                    if (!javax. swing. SwingUtilities.
	                           isRightMouseButton (evt)) {
	               tableSelect_withLeft ((String)o);
	               m_dynamicLabel. setText (" ");
	            }
//	            else
//	               tableSelect_withRight ((String)o);
	         }
	      }
	   });

	   m_gainSlider.
	      addChangeListener (new ChangeListener() {
	         public void
	               stateChanged(javax. swing.event.ChangeEvent evt) {
	            int gainValue = ((JSlider)evt. getSource ()). getValue ();
	            m_gainLabel. setText (Integer. toString (gainValue));
	            listeners. forEach ((hl) -> {
	               hl.  gainValue (gainValue);
	             });
	         }
           });

	   m_lnaState.
	      addChangeListener(new ChangeListener() {
    	         @Override
	         public void stateChanged (ChangeEvent e) {
	            int lnaState = (int)(m_lnaState. getValue ());
	            listeners. forEach ((hl) -> {
	               hl.  lnaStateValue (lnaState);
	             });
	         }
	   });

	   m_soundLevel.
	      addChangeListener(new ChangeListener() {
    	         @Override
	         public void stateChanged (ChangeEvent e) {
	            int soundLevel = (int)(m_soundLevel. getValue ());
	            listeners. forEach ((hl) -> {
	               hl. soundLevel (soundLevel);
	             });
	         }
	   });

	   m_resetButton.
	      addActionListener (new ActionListener () {
	         @Override
	         public void actionPerformed (ActionEvent evt) {
	            listeners. forEach ((hl) -> {
                       hl. reset ();
                     });
	         }
	      });

	   m_autogainButton.
	      addActionListener (new ActionListener () {
	         @Override
	         public void actionPerformed (ActionEvent evt) {
	            if (autogainState)
	               m_autogainButton. setText ("autogain off");
	            else
	               m_autogainButton. setText ("autogain on");
	            autogainState = !autogainState;
	            listeners. forEach ((hl) -> {
                       hl. autogainButton ();
                     });
	         }
	      });
	}

	private	void	tableSelect_withLeft (String name) {
	   listeners.forEach((hl) -> {
              hl. tableSelect_withLeft (name);
           });
        }

	public	String	get_serviceName (int row, int column) {
	   return m_serviceTable. get_serviceName (row, column);
	}

	public	void	show_ensembleName	(String s) {
	   m_ensembleLabel. setText (s);
	}

	public	void	show_serviceName (String name) {
	   m_serviceTable. newService (name);
	}

	public	void	show_dynamicLabel	(String s) {
	   m_dynamicLabel. setText (s);
	}

	public	void	show_programData	(String s) {
	}

	public	void	show_signalQuality	(int q) {
	   m_qualityLabel. setText (Integer. toString (q));
	}

	public	void	show_synced		(boolean b) {
	   m_syncedLabel. setBackground (b ? Color. green : Color. red);
	}

	public	void	set_lnaState		(int s) {
	   m_lnaState. setValue (s);
	}

	public	void	set_GRdB		(int s) {
	   m_gainSlider. setValue (s);
	}

	public	void	showSelectedChannel (String channel) {
	   m_channelLabel. setText ("Selected " + channel);
	}

	public	void	showService         (String currentService) {
	   m_selectedService. setText (currentService);
	   m_selectedService. setBackground (Color. green);
	   m_selectedService. setOpaque (true);
	}

	public	void	showServiceEnabled (int numofServices) {
	   m_selectedService. setText ("please choose a service");
	   m_selectedService. setBackground (Color. green);
	   m_selectedService. setOpaque (true);
	   m_resetButton. setBackground (Color. green);
	   m_resetButton. setOpaque (true);
	}

	public	void	clear_dynamicLabel	() {
	   String s = " ";
	   m_dynamicLabel. setText (s);
	}

//	Used to set the initial value
	public	void	setDeviceGain (int v) {
	   m_gainSlider. setValue (v);
	   m_gainLabel. setText (Integer. toString (v));
	}

	public	void	set_stereoIndicator	(boolean b) {
	   if (b)
	      m_stereoIndicator. setBackground (Color. green);
	   else
	      m_stereoIndicator. setBackground (Color. red);
	}

	public	void	clearScreen () {
	   m_dynamicLabel. setText (" ");
	   m_ensembleLabel. setText (" ");
	   m_serviceTable. clearTable ();
	   m_selectedService. setText ("wait for services");
	   m_selectedService. setBackground (Color. red);
	   m_selectedService. setOpaque (true);
	   m_resetButton. setBackground (Color.red);
	   m_resetButton. setOpaque (true);
	}
}
