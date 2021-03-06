package controller;

import	java. util. *;
import	viewer.*;
import	model.*;

public class ClientController implements modelSignals, viewSignals {

//	The Controller needs to interact with both the Model and View.
//	It "listens" to signals from both and it issues commands to both
	private final	ClientModel	m_model;
	private final	ClientView	m_view;
//========================================================== constructor
    /** Constructor
     * @param model
     * @param view */
	public ClientController (ClientModel model, ClientView view) {
	   m_model		= model;
           m_view		= view;
	}
//
	public	void	startRadio () {
	   m_model.	addServiceListener (this);
	   m_view. 	addServiceListener (this);
	   try {
	      m_model.	startRadio ();
	   } catch (Exception e) {}
	}
//
	@Override
	public	void	reset		() {
	   m_model. reset	();
	   m_view. clearScreen	();
	}

	@Override
	public	void	tableSelect_withLeft (String serviceName) {
	   startService (serviceName);
	}

	@Override
	public	void	gainValue	(int val) {
	   m_model.	gainReduction (val);
	}

	@Override
	public	void	lnaStateValue	(int lnaState) {
	   m_model.	setLnaState (lnaState);
	}

	@Override
	public	void	soundLevel	(int soundLevel) {
	   m_model.	set_soundLevel (soundLevel);
	}

	@Override
	public	void	autogainButton	() {
	   m_model.	set_autoGain	();
	}

	@Override
	public	void	set_initialValues (char [] v) {
	   m_view. set_initialValues (v);
	}

	@Override
	public	void	show_deviceName	(String s) {
	   m_view. show_deviceName (s);
	}

	@Override
	public void	show_ensembleName (String s, int SId) {
	   m_view. show_ensembleName (s);
	}

	@Override
	public void	show_serviceName (String s) {
	   m_view. show_serviceName (s);
	}

	@Override
	public	void	show_dynamicLabel	(String s) {
	   m_view. show_dynamicLabel (s);
	}

	@Override
	public	void	show_state	(char [] q) {
	   m_view. show_state (q);
	}

	@Override
	public	void	clearScreen		() {
	   m_view. clearScreen ();
	}

	@Override
	public	void	set_stereoIndicator	(boolean b) {
	   m_view. set_stereoIndicator (b);
	}

	public	void	startService	(String s) {
	   String serviceName	= s;
	   m_view.  showService   (serviceName);
	   m_model. setService	  (serviceName);
	}

}

