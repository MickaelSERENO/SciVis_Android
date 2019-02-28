package com.sereno.vfv.Network;

import android.util.Log;

import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.VTKDataset;
import com.sereno.vfv.MainActivity;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.util.ArrayDeque;

public class SocketManager implements Runnable
{
    /** The connection timeout in milliseconds*/
    public static final int    CONNECT_TIMEOUT    = 100;
    /** How many milliseconds the thread has to sleep before reattempting to connect ?*/
    public static final int    FAIL_CONNECT_SLEEP = 200;
    /** How many milliseconds the thread has to sleep before resending data ?*/
    public static final int    THREAD_SLEEP       = 1000/90;

    /* ************************************************************ */
    /* ******************Recognizable server type****************** */
    /* ************************************************************ */

    public static final short IDENT_TABLET    = 1;
    public static final short ADD_VTK_DATASET = 3;
    public static final short ROTATE_DATASET  = 4;

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
    private String m_serverIP;
    /** The port of the server*/
    private int    m_serverPort;

    /** The socket's thread*/
    private Thread  m_thread;
    /** Is the socket closed ?*/
    private boolean m_isClosed = false;

    /** The Hololens IP to bind*/
    private String  m_hololensIP        = null;

    /** Is the hololens bound to this tablet?*/
    private boolean m_isBoundToHololens = false;

    /** The queue buffer storing data to SEND*/
    private ArrayDeque<byte[]> m_queueSendBuf = new ArrayDeque<>();

    public SocketManager(String ip, int port)
    {
        m_socket     = new Socket();
        m_serverIP   = ip;
        m_serverPort = port;
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
                    Log.i(MainActivity.TAG, "Trying to reconnect at " + m_serverIP + ":" + m_serverPort + "\n");
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
                //Send TABLET_IDENT
                if(m_isBoundToHololens == false && m_hololensIP != null)
                {
                    try
                    {
                        m_output.write(getIdentData());
                        m_output.flush();
                        m_isBoundToHololens = true;
                    }
                    catch(final IOException e)
                    {
                        close();
                        continue;
                    }
                }

                //Send other buffers. These buffer are generate from the static method this class provides
                while(!m_queueSendBuf.isEmpty())
                {
                    byte[] d = m_queueSendBuf.poll();
                    try
                    {
                        m_output.write(d);
                        m_output.flush();
                    }
                    catch(final Exception e)
                    {
                        close();
                        break;
                    }
                }
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
            m_serverIP   = ip;
            m_serverPort = port;
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

        try{m_socket.setReuseAddress(true);}catch(Exception e){}
        //Needed to resend a new bound
        m_isBoundToHololens = false;

        Log.i(MainActivity.TAG, "Communication lost with the server\n");
    }

    /* ************************************************************ */
    /* *******************Setters / Getters************************ */
    /* ************************************************************ */

    public synchronized void setHololensIP(String hololensIP)
    {
        m_hololensIP        = hololensIP;
        m_isBoundToHololens = false;
    }

    public synchronized void push(byte[] data)
    {
        m_queueSendBuf.add(data);
    }

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
                m_socket.connect(new InetSocketAddress(m_serverIP, m_serverPort), CONNECT_TIMEOUT);
            }
            m_output = new DataOutputStream(m_socket.getOutputStream());
            m_input  = new BufferedReader(new InputStreamReader(m_socket.getInputStream()));

            //Send an ident without hololens information
            ByteBuffer buf = ByteBuffer.allocate(2+4);
            buf.order(ByteOrder.BIG_ENDIAN);
            buf.putShort(IDENT_TABLET);
            buf.putInt(0);
            m_output.write(buf.array());
            m_output.flush();
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

    /** Get the IDENT_TABLET byte array to send*/
    private byte[] getIdentData()
    {
        ByteBuffer buf = ByteBuffer.allocate(2+4+m_hololensIP.length());
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(IDENT_TABLET);
        buf.putInt(m_hololensIP.length());
        buf.put(m_hololensIP.getBytes(StandardCharsets.US_ASCII));

        return buf.array();
    }

    /* ************************************************************ */
    /* *****************Create buffers - static******************** */
    /* ************************************************************ */

    public static byte[] createRotationEvent(Dataset dataset, int subID, float[] qArr)
    {
        ByteBuffer buf = ByteBuffer.allocate(2+2*4+4*4);
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(ROTATE_DATASET);
        buf.putInt(dataset.getID());
        buf.putInt(subID);

        for(int i = 0; i < 4; i++)
            buf.putFloat(qArr[i]);

        return buf.array();
    }

    public static byte[] createAddVTKDatasetEvent(VTKDataset d)
    {
        ByteBuffer buf = ByteBuffer.allocate(2+3*4+
                                             d.getName().length()+
                                             4*(d.getSelectedCellFieldValues().length + d.getSelectedPtFieldValues().length));
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(ADD_VTK_DATASET);
        buf.putInt(d.getName().length());
        buf.put(d.getName().getBytes(StandardCharsets.US_ASCII));
        buf.putInt(d.getSelectedPtFieldValues().length);

        for(int i = 0; i < d.getSelectedPtFieldValues().length; i++)
            buf.putInt(d.getPtFieldValueIndice(d.getSelectedPtFieldValues()[i]));

        for(int i = 0; i < d.getSelectedCellFieldValues().length; i++)
            buf.putInt(d.getCellFieldValueIndice(d.getSelectedPtFieldValues()[i]));

        return buf.array();
    }
}
