package com.example.jan.dabradio;

public interface Signals {
        void    show_deviceName(String s);
        void    set_initialValues(byte[] v);
        void    show_ensembleName(String s1, int s2);
        void    show_serviceName(String s1);
        void    show_dynamicLabel(String s);
        void    show_state(byte[] v);
        void    clearScreen();
        void    set_stereoIndicator(boolean b);
        void	toaster(String s);
        void    set_quit();
}


