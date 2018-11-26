package model;

public interface modelSignals {
	public	void	show_deviceName		(String s);
	public	void	set_initialValues	(char [] v);
	public	void	show_ensembleName	(String s1, int s2);
	public	void	show_serviceName	(String s1);
	public	void	show_dynamicLabel	(String s);
	public	void	show_state		(char [] v);
	public	void	clearScreen		();
	public	void	set_stereoIndicator	(boolean b);
}

