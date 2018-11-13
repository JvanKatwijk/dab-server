package model;

public interface modelSignals {
	public	void	show_ensembleName	(String s1, int s2);
	public	void	show_serviceName	(String s1);
	public	void	show_dynamicLabel	(String s);
	public	void	show_programData	(String s);
	public	void	show_signalQuality	(int q);
	public	void	show_synced		(boolean q);
	public	void	set_lnaState		(int s);
	public	void	set_GRdB		(int s);
	public	void	clearScreen		();
	public	void	set_stereoIndicator	(boolean b);
}

