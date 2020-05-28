package com.sereno.vfv.Network;

import android.graphics.Point;
import android.graphics.PointF;
import android.util.Log;

import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Data.VTKDataset;
import com.sereno.vfv.MainActivity;
import com.sereno.view.AnnotationData;
import com.sereno.view.AnnotationStroke;
import com.sereno.view.AnnotationText;
import com.sereno.view.GTFData;

import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Map;

import static com.sereno.vfv.Data.SubDataset.VISIBILITY_PRIVATE;

public class SocketManager
{
    public interface ISocketManagerListener
    {
        /** Function called when the socket has been disconnected
         * @param socket The SocketManager calling this Method*/
        void onDisconnection(SocketManager socket);
    }

    /** The connection timeout in milliseconds*/
    public static final int CONNECT_TIMEOUT    = 100;
    /** How many milliseconds the thread has to sleep before reattempting to connect ?*/
    public static final int FAIL_CONNECT_SLEEP = 200;
    /** How many milliseconds the thread has to sleep before resending data ?*/
    public static final int THREAD_SLEEP       = 1000/90;
    public static final int READ_TIMEOUT       = 0;

    /* ************************************************************ */
    /* ******************Recognizable server type****************** */
    /* ************************************************************ */

    public static final short IDENT_TABLET            = 1;
    public static final short ADD_VTK_DATASET         = 3;
    public static final short ROTATE_DATASET          = 4;
    public static final short SEND_ANNOTATION         = 6;
    public static final short SEND_CURRENT_ACTION     = 9;
    public static final short SEND_CURRENT_SUBDATASET = 10;
    public static final short TRANSLATE_DATASET       = 11;
    public static final short SCALE_DATASET           = 12;
    public static final short TF_SUBDATASET           = 13;
    public static final short SEND_START_ANNOTATION   = 14;
    public static final short ADD_SUBDATASET          = 17;
    public static final short REMOVE_SUBDATASET       = 18;
    public static final short MAKE_SUBDATASET_PUBLIC  = 19;
    public static final short DUPLICATE_SUBDATASET    = 20;
    public static final short LOCATION                = 21;
    public static final short LASSO                   = 22;
    public static final short TABLETSCALE             = 23;

    /* ************************************************************ */
    /* *********************Private attributes********************* */
    /* ************************************************************ */

    /** The socket object for communicating with the server*/
    private Socket           m_socket;
    /** The output stream of the socket*/
    private DataOutputStream m_output;
    /** The input stream of the socket*/
    private InputStream      m_input;

    /** The ip of the server*/
    private String m_serverIP;
    /** The port of the server*/
    private int    m_serverPort;

    /** The socket's write thread*/
    private Thread  m_writeThread;
    /** The socket's read thread. This thread also handles disconnections.*/
    private Thread  m_readThread;
    /** Is the socket closed ?*/
    private boolean m_isClosed = false;

    /** The Hololens IP to bind*/
    private String  m_hololensIP = null;
    /** The user's handedness (default: right)*/
    private int     m_handedness = ApplicationModel.HANDEDNESS_RIGHT;
    /** The tablet ID (see config.json)*/
    private int     m_tabletID = 0;

    /** Is the hololens bound to this tablet?*/
    private boolean m_isBoundToHololens = false;

    /** The message buffer being used*/
    private MessageBuffer m_msgBuffer = new MessageBuffer();

    /** The queue buffer storing data to SEND*/
    private ArrayDeque<byte[]> m_queueSendBuf = new ArrayDeque<>();

    /** List of listener to call when the socket status changes*/
    private ArrayList<ISocketManagerListener> m_listeners = new ArrayList<>();


    /** Runnable writing to the socket. It also tries to reconnect to the server every time*/
    private Runnable m_writeThreadRunnable = new Runnable()
    {
        @Override
        public void run()
        {
            while(!m_isClosed)
            {
                //Connect the socket
                boolean isConnected = m_socket.isConnected();

                //If not connected, sleep longer
                if(!isConnected)
                {
                    try {Thread.sleep(SocketManager.FAIL_CONNECT_SLEEP);} catch (Exception e) {}
                    continue;
                }

                if(!checkWritting())
                    continue;

                try{Thread.sleep(SocketManager.THREAD_SLEEP);} catch (Exception e) {}
            }
        }
    };

    /** Runnable reading the socket*/
    private Runnable m_readThreadRunnable = new Runnable()
    {
        @Override
        public void run()
        {
            byte[] buf = new byte[65536];

            while(!m_isClosed)
            {
                //Connect the socket

                boolean isConnected = false;

                synchronized (this)
                {
                    isConnected = m_socket.isConnected();
                    //Check the connection
                    if(!isConnected)
                        isConnected = connect();
                }

                try
                {
                    //Read the data
                    if(isConnected)
                    {
                        int readSize = m_input.read(buf);
                        if (readSize > 0)
                            m_msgBuffer.push(buf, readSize);
                        else //EOF
                            close();
                    }
                    else
                    {
                        Thread.sleep(THREAD_SLEEP);
                        continue;
                    }
                }
                catch(Exception e)
                {
                    synchronized (this)
                    {
                        close();
                    }
                }
            }
        }
    };

    public SocketManager(String ip, int port)
    {
        m_socket      = new Socket();
        m_serverIP    = ip;
        m_serverPort  = port;
        m_writeThread = new Thread(m_writeThreadRunnable);
        m_writeThread.start();

        m_readThread = new Thread(m_readThreadRunnable);
        m_readThread.start();
    }

    /** @brief Add a listener object to call when the internal states of this socket changes
     * @param l the new listener to take account of*/
    public void addListener(ISocketManagerListener l)
    {
        if(!m_listeners.contains(l))
            m_listeners.add(l);
    }

    /** @brief Remove a listener object to call when the internal states of this socket changes
     * @param l the new listener to take account of*/
    public void removeListener(ISocketManagerListener l)
    {
        m_listeners.remove(l);
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
        close();
        m_isClosed = true;
        try{m_writeThread.join();} catch (Exception e){}
        try{m_readThread.join();} catch (Exception e){}
    }

    /** Close the socket*/
    public synchronized void close()
    {
        if(m_socket.isConnected())
            try{m_socket.close();} catch(Exception e){}
        m_output = null;
        m_input  = null;
        m_socket = new Socket();

        try{m_socket.setReuseAddress(true);}catch(Exception e){}
        //Needed to resend a new bound
        m_isBoundToHololens = false;

        for(ISocketManagerListener l : m_listeners)
            l.onDisconnection(this);
    }

    /* ************************************************************ */
    /* *******************Setters / Getters************************ */
    /* ************************************************************ */

    /** Get the Message Buffer parsing the incoming data
     * @return The Message Buffer parsing the incoming data. Useful for listening to new events*/
    public MessageBuffer getMessageBuffer()
    {
        return m_msgBuffer;
    }

    /** Set Ident information
     * @param hololensIP the hololens IP bound to the tablet. Will be resent at each disconnection
     * @param handedness the user's handedness (left or right. See ApplicationModel.HANDEDNESS_*)
     * @param tabletID the tablet ID*/
    public synchronized void setIdentInformation(String hololensIP, int handedness, int tabletID)
    {
        m_hololensIP        = hololensIP;
        m_handedness        = handedness;
        m_isBoundToHololens = false;
        m_tabletID          = tabletID;
    }

    /** Push a new value to write to the server
     * @param data array of bytes to write to the server*/
    public synchronized void push(byte[] data)
    {
        synchronized(m_queueSendBuf)
        {
            m_queueSendBuf.add(data);
        }
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
            if(m_input != null || m_output != null || m_socket.isConnected())
                close();

            m_socket.connect(new InetSocketAddress(m_serverIP, m_serverPort), CONNECT_TIMEOUT);
            m_socket.setSoTimeout(READ_TIMEOUT);
            m_output = new DataOutputStream(m_socket.getOutputStream());
            m_input  = m_socket.getInputStream();
        }
        catch(Exception e)
        {
            m_socket = new Socket();
            m_output = null;
            m_input  = null;
            return false;
        }

        return true;
    }

    /** Get the IDENT_TABLET byte array to send*/
    private byte[] getIdentData()
    {
        ByteBuffer buf = ByteBuffer.allocate(2+4+4+m_hololensIP.length()+4);
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(IDENT_TABLET);
        buf.putInt(m_hololensIP.length());
        buf.put(m_hololensIP.getBytes(StandardCharsets.US_ASCII));
        buf.putInt(m_handedness);
        buf.putInt(m_tabletID);

        return buf.array();
    }

    /** Check the writting part of the client
     * @return true if no error occured, false otherwise*/
    private synchronized boolean checkWritting()
    {
        //Send TABLET_IDENT
        if(m_isBoundToHololens == false && m_hololensIP != null)
        {
            synchronized(m_queueSendBuf)
            {
                m_queueSendBuf.push(getIdentData());
                m_isBoundToHololens = true;
            }
        }

        //Send buffers. These buffer are generate from the static method this class provides
        synchronized(m_queueSendBuf)
        {
            while(!m_queueSendBuf.isEmpty())
            {
                byte[] d = m_queueSendBuf.poll();
                try
                {
                    synchronized(this)
                    {
                        m_output.write(d);
                        m_output.flush();
                    }
                }
                catch(final Exception e)
                {
                    synchronized(this)
                    {
                        close();
                    }
                    return false;
                }
            }
            return true;
        }
    }

    /* ************************************************************ */
    /* *****************Create buffers - static******************** */
    /* ************************************************************ */

    /** Create a "Set current Working subdataset" event
     * @param ids the dataset and subdatasets IDs
     * @return array of byte to send to push*/
    public static byte[] createCurrentSubDatasetEvent(MainActivity.DatasetIDBinding ids)
    {
        ByteBuffer buf = ByteBuffer.allocate(2+2*4);
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(SEND_CURRENT_SUBDATASET);
        buf.putInt(ids.dataset.getID());
        buf.putInt(ids.subDatasetID);

        return buf.array();
    }

    /** Create a set current action event to send to the server
     * @param action the new current action
     * @return array of byte to send to push*/
    public static byte[] createCurrentActionEvent(int action)
    {
        ByteBuffer buf = ByteBuffer.allocate(2+4);
        buf.order(ByteOrder.BIG_ENDIAN);
        buf.putShort(SEND_CURRENT_ACTION);
        buf.putInt(action);

        return buf.array();
    }

    /** Create a Rotation event data to send to the server
     * @param ids the dataset and subdatasets IDs
     * @param qArr the array of the new quaternion to send (w, i, j, k)
     * @return array of byte to send to push*/
    public static byte[] createRotationEvent(MainActivity.DatasetIDBinding ids, float[] qArr)
    {
        ByteBuffer buf = ByteBuffer.allocate(2+2*4+4*4);
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(ROTATE_DATASET);
        buf.putInt(ids.dataset.getID());
        buf.putInt(ids.subDatasetID);

        for(int i = 0; i < 4; i++)
            buf.putFloat(qArr[i]);

        return buf.array();
    }

    /** Create a Translation event data to send to the server
     * @param ids the dataset and subdatasets IDs
     * @param pArr the array of the new position to send (x, y, z)
     * @return array of byte to send to push*/
    public static byte[] createPositionEvent(MainActivity.DatasetIDBinding ids, float[] pArr)
    {
        ByteBuffer buf = ByteBuffer.allocate(2+2*4+3*4);
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(TRANSLATE_DATASET);
        buf.putInt(ids.dataset.getID());
        buf.putInt(ids.subDatasetID);

        for(int i = 0; i < 3; i++)
            buf.putFloat(pArr[i]);

        return buf.array();
    }

    /** Create a Scale event data to send to the server
     * @param ids the dataset and subdatasets IDs
     * @param sArr the array of the new scale to send (x, y, z)
     * @return array of byte to send to push*/
    public static byte[] createScaleEvent(MainActivity.DatasetIDBinding ids, float[] sArr)
    {
        ByteBuffer buf = ByteBuffer.allocate(2+2*4+3*4);
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(SCALE_DATASET);
        buf.putInt(ids.dataset.getID());
        buf.putInt(ids.subDatasetID);

        for(int i = 0; i < 3; i++)
            buf.putFloat(sArr[i]);

        return buf.array();
    }

    /** Fill common data stored in a GTFData
     * @param ids the dataset and subdatasets IDs
     * @param data the gtf data to look at*/
    public static byte[] createGTFEvent(MainActivity.DatasetIDBinding ids, GTFData data)
    {
        ByteBuffer buf = ByteBuffer.allocate(2 + 3*4 + 2*1 + (3*4*data.getRanges().size()));
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(TF_SUBDATASET);

        //Header common to TF
        buf.putInt(ids.dataset.getID());
        buf.putInt(ids.subDatasetID);
        buf.put((byte)data.getDataset().getTransferFunctionType());
        buf.put((byte)data.getColorMode());

        //Common values for all GTF properties
        buf.putInt(data.getRanges().size());

        //Send properties
        for(Map.Entry<Integer, GTFData.GTFPoint> entry : data.getRanges().entrySet())
        {
            GTFData.GTFPoint range = entry.getValue();
            buf.putInt(entry.getKey());
            buf.putFloat(range.center);
            buf.putFloat(range.scale);
        }

        return buf.array();
    }

    /** Create a Add VTK Dataset Event to send to the server
     * @param d the VTK Dataset being added
     * @return array of byte to send to push*/
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

    /** Create an annotation event to send
     * @param ids the Dataset and SubDataset ids attached to this annotation
     * @param annotationData the annotation data
     * @param metaData the meta data of this annotation*/
    public static byte[] createAnnotationEvent(MainActivity.DatasetIDBinding ids, AnnotationData annotationData, ApplicationModel.AnnotationMetaData metaData)
    {
        int size = 2+7*4;
        for(AnnotationStroke s : annotationData.getStrokes())
            size += 3*4 + 2*s.getPoints().size()*4;
        for(AnnotationText t : annotationData.getTexts())
            size += 3*4 + 4 + t.getText().length();
        ByteBuffer buf = ByteBuffer.allocate(size);
        buf.order(ByteOrder.BIG_ENDIAN);

        //Send "header" data
        buf.putShort(SEND_ANNOTATION);
        buf.putInt(ids.dataset.getID());
        buf.putInt(ids.subDatasetID);
        buf.putInt(metaData.getAnnotationID());
        buf.putInt(annotationData.getWidth());
        buf.putInt(annotationData.getHeight());
        buf.putInt(annotationData.getStrokes().size());
        buf.putInt(annotationData.getTexts().size());

        //Send strokes data
        for(AnnotationStroke s : annotationData.getStrokes())
        {
            buf.putInt(s.getColor());
            buf.putFloat(s.getWidth());
            buf.putInt(s.getPoints().size());

            for(Point p : s.getPoints())
            {
                buf.putFloat(p.x);
                buf.putFloat(p.y);
            }
        }

        //Send text data
        for(AnnotationText t : annotationData.getTexts())
        {
            buf.putInt(t.getColor());
            buf.putFloat(t.getPosition().x);
            buf.putFloat(t.getPosition().y);
            buf.putInt(t.getText().length());
            buf.put(t.getText().getBytes(StandardCharsets.US_ASCII));
        }

        return buf.array();
    }

    /** Create an event regarding to specify that the user wants to start an annotation
     * @param ids the Dataset and SubDataset ids attached to this annotation
     * @param pointingID the ID of the pointing interaction technique in use*/
    public static byte[] createStartAnnotationEvent(MainActivity.DatasetIDBinding ids, int pointingID)
    {
        int size = 2+3*4;

        ByteBuffer buf = ByteBuffer.allocate(size);
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(SEND_START_ANNOTATION);
        buf.putInt(ids.dataset.getID());
        buf.putInt(ids.subDatasetID);
        buf.putInt(pointingID);

        return buf.array();
    }

    /** Create an add subdataset event
     * @param datasetID the dataset parent to the future SubDataset
     * @param publicSD will the SubDataset be public?*/
    public static byte[] createAddSubDatasetEvent(int datasetID, boolean publicSD)
    {
        ByteBuffer buf = ByteBuffer.allocate(2+4+1);
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(ADD_SUBDATASET);
        buf.putInt(datasetID);
        buf.put((byte)(publicSD ? 1 : 0));

        return buf.array();
    }

    /** Create a duplicate subdataset event
     * @param ids the subdataset IDs to duplicate*/
    public static byte[] createDuplicateSubDatasetEvent(MainActivity.DatasetIDBinding ids)
    {
        ByteBuffer buf = ByteBuffer.allocate(2+2*4);
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(DUPLICATE_SUBDATASET);
        buf.putInt(ids.dataset.getID());
        buf.putInt(ids.subDatasetID);

        return buf.array();
    }

    /** Create an remove subdataset event
     * @param ids the subdataset IDs to remove*/
    public static byte[] createRemoveSubDatasetEvent(MainActivity.DatasetIDBinding ids)
    {
        ByteBuffer buf = ByteBuffer.allocate(2+2*4);
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(REMOVE_SUBDATASET);
        buf.putInt(ids.dataset.getID());
        buf.putInt(ids.subDatasetID);

        return buf.array();
    }

    /** Create an remove subdataset event
     * @param ids the subdataset IDs*/
    public static byte[] createMakeSubDatasetPublicEvent(MainActivity.DatasetIDBinding ids)
    {
        ByteBuffer buf = ByteBuffer.allocate(2+2*4);
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(MAKE_SUBDATASET_PUBLIC);
        buf.putInt(ids.dataset.getID());
        buf.putInt(ids.subDatasetID);

        return buf.array();
    }

    /** Create a location event
     * @param pos the new position
     * @param rot the new rotation
     * @return array of byte to send to push*/
    public static byte[] createLocationEvent(float[] pos, float[] rot)
    {
        ByteBuffer buf = ByteBuffer.allocate(2+3*4+4*4);
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(LOCATION);

        for(int i = 0; i < 3; i++)
            buf.putFloat(pos[i]);

        for(int i = 0; i < 4; i++)
            buf.putFloat(rot[i]);

        return buf.array();
    }

    /** Create a lasso event
     * @param lasso the lasso data
     * @return array of byte to send to push*/
    public static byte[] createLassoEvent(float[] lasso)
    {
        ByteBuffer buf = ByteBuffer.allocate(2 + 4 + lasso.length*4);
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(LASSO);

        buf.putInt(lasso.length);

        for(int i = 0; i < lasso.length; i++)
            buf.putFloat(lasso[i]);

        return buf.array();
    }

    /** Create a tablet scale event
     * @param scale the tablet scale
     * @return array of byte to send to push*/
    public static byte[] createTabletScaleEvent(float scale, float width, float height, float posx, float posy)
    {
        ByteBuffer buf = ByteBuffer.allocate(2 + 5*4);
        buf.order(ByteOrder.BIG_ENDIAN);

        buf.putShort(TABLETSCALE);

        buf.putFloat(scale);
        buf.putFloat(width);
        buf.putFloat(height);
        buf.putFloat(posx);
        buf.putFloat(posy);

        return buf.array();
    }
}
