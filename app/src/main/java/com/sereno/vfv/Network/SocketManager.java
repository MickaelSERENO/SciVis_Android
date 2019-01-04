package com.sereno.vfv.Network;

import android.util.Log;

import com.sereno.vfv.MainActivity;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.net.InetSocketAddress;
import java.net.Socket;

public class SocketManager implements Runnable
{
    /** The connection timeout in milliseconds*/
    public static final int    CONNECT_TIMEOUT    = 100;
    /** How many milliseconds the thread has to sleep before reattempting to connect ?*/
    public static final int    FAIL_CONNECT_SLEEP = 200;
    /** How many milliseconds the thread has to sleep before resending data ?*/
    public static final int    THREAD_SLEEP       = 1000/90;

    /* ************************************************************ */
    /* *********************Private attributes********************* */
    /* ************************************************************ */

    /** The socket object for communicating with the server*/
    private Socket           m_socket;
    /** The output stream of the socket*/
    private DataOutputStream m_output;
    /** The input stream of the socket*/
    private BufferedReader   m_input;

    /** The ip of the server*/
    private String m_ipServer;
    /** The port of the server*/
    private int    m_portServer;

    /** The socket's thread*/
    private Thread           m_thread;
    /** Is the socket closed ?*/
    private boolean          m_isClosed    = false;

    public SocketManager(String ip, int port)
    {
        m_socket     = new Socket();
        m_ipServer   = ip;
        m_portServer = port;
        m_thread     = new Thread(this);
        m_thread.start();
    }

    /** Connect to the server and send the data periodically.*/
    public void run()
    {
        while(!m_isClosed)
        {
            //Connect the socket
            boolean isConnected = m_socket.isConnected();
            synchronized (this)
            {
                if (!isConnected)

                {
                    Log.i(MainActivity.TAG, "Trying to reconnect\n");
                    isConnected = this.connect();
                }
            }

            //If not connected, sleep longer
            if(!isConnected)
            {
                try {m_thread.sleep(FAIL_CONNECT_SLEEP);} catch (Exception e) {}
                continue;
            }

            //Send the data. Synchronize the object first
            synchronized(this)
            {
            }

            try{m_thread.sleep(THREAD_SLEEP);} catch (Exception e) {}
        }
    }

    /** \brief Set the server address
     * @param ip the server ip
     * @param port the server port
     */
    public void setServerAddr(String ip, int port)
    {
        synchronized(this)
        {
            m_ipServer   = ip;
            m_portServer = port;
            close();
        }
    }

    /** Stop the thread*/
    public void stopThread()
    {
        m_isClosed = true;
        try{m_thread.join();} catch (Exception e){}
    }

    /** Close the socket*/
    public void close()
    {
        if(m_socket.isConnected())
            try{m_socket.close();} catch(Exception e){}
        m_output = null;
        m_input  = null;
        m_socket = new Socket();

        Log.i(MainActivity.TAG, "Communication lost with the server\n");
    }

    /* ************************************************************ */
    /* *******************Setters / Getters************************ */
    /* ************************************************************ */

    /** Connect to the server. We put this into a separate function for letting the thread connecting to the server
     * (non-blocking connection)
     *
     * Do not connect more than once if the boolean has returned true
     * The function "run" called this method.
     *
     * @return true on success, false on failure*/
    private boolean connect()
    {
        try
        {
            //We may want to close everything just in case
            //Indeed, the stream output or input may have been the issue into the last attempt
            //In can also be an error from the main application
            close();

            synchronized(this)
            {
                m_socket.connect(new InetSocketAddress(m_ipServer, m_portServer), CONNECT_TIMEOUT);
            }
            m_output = new DataOutputStream(m_socket.getOutputStream());
            m_input  = new BufferedReader(new InputStreamReader(m_socket.getInputStream()));
        }
        catch(Exception e)
        {
            m_socket = new Socket();
            m_output = null;
            m_input  = null;
            return false;
        }
        Log.i(MainActivity.TAG, "Reconnected\n");
        return true;
    }
}
