package com.sereno.vfv.Network;

import android.util.Log;

import com.sereno.vfv.MainActivity;

import java.nio.ByteBuffer;
import java.util.ArrayList;

/** Message Buffer class. Permits to parse the incoming server data correctly
 * Format : 2byte -> typeOfMessage then follows (depending on the message) 4 bytes for int/float,
 *          2 bytes for short and/or 4 bytes + n bytes for String (n depending on the first 4 bytes received)*/
public class MessageBuffer
{
    /** Callback interface used when new message has been successfully parsed*/
    public interface IMessageBufferCallback
    {
        /** Called when an empty message has been received (i.e., received only a type with no expected data)
         * @param msg the message parsed*/
        void onEmptyMessage(EmptyMessage msg);

        /** Called when the message acknowledge add dataset has been successfully parsed
         * @param msg the message parsed*/
        void onAcknowledgeAddDatasetMessage(AcknowledgeAddDatasetMessage msg);
    }

    /** No current type received*/
    public static final int GET_NO_TYPE                 = -1;

    /** Add dataset acknowledge received*/
    public static final int GET_ADD_DATASET_ACKNOWLEDGE = 0;

    /** The current message being parsed*/
    private ServerMessage m_curMsg = null;
    /**The current type being parsed*/
    private short         m_type   = -1;

    /** Special string buffer for strings*/
    private StringBuilder m_stringBuf  = null;
    /** The total amount of size for the current string buffer being used*/
    private int m_stringSize = -1;

    private ArrayList<IMessageBufferCallback> m_listeners = new ArrayList<>();

    /**Constructor, initialize every variable to default*/
    public MessageBuffer()
    {}

    /**Push values present in the ByteBuffer
     * The ByteBuffer will, at the maximum, be left with 3 bytes (4bytes -1)*/
    public void push(ByteBuffer buffer)
    {
        while(true)
        {
            try
            {
                if(m_type == GET_NO_TYPE)
                {
                    m_type = buffer.getShort();
                    allocateNewMessage();
                }

                //Read the data
                while(m_curMsg != null && m_curMsg.cursor <= m_curMsg.getMaxCursor())
                {
                    switch(m_curMsg.getCurrentType())
                    {
                        case 's':
                            if(m_stringSize < 0)
                            {
                                m_stringSize = buffer.getInt();
                                if(m_stringSize < 0)
                                {
                                    Log.e(MainActivity.TAG, "Received a string buffer size inferior than 0... Treat it as 0");
                                    m_stringSize = 0;
                                }
                                m_stringBuf  = new StringBuilder(m_stringSize);
                            }
                            while(m_stringBuf.length() != m_stringSize)
                                m_stringBuf.append(buffer.getChar());
                            m_curMsg.pushValue(m_stringBuf.toString());
                            break;
                        case 'i':
                            m_curMsg.pushValue(buffer.getShort());
                            break;
                        case 'I':
                            m_curMsg.pushValue(buffer.getInt());
                            break;
                        case 'f':
                            m_curMsg.pushValue(buffer.getFloat());
                            break;
                        default:
                            Log.e(MainActivity.TAG, "Unhandled type " + m_curMsg.getCurrentType() + ". Skipping");
                            break;
                    }
                    m_curMsg.cursor++;
                }

                //Yields the events
                if(m_curMsg != null && m_curMsg.cursor > m_curMsg.getMaxCursor())
                {
                    switch(m_type)
                    {
                        case GET_ADD_DATASET_ACKNOWLEDGE:
                            for(IMessageBufferCallback clbk : m_listeners)
                                clbk.onAcknowledgeAddDatasetMessage((AcknowledgeAddDatasetMessage) m_curMsg);
                            m_type = GET_NO_TYPE;
                            continue;

                        default:
                            Log.e(MainActivity.TAG, "Unknown type " + m_type);
                            m_type = GET_NO_TYPE;
                            continue;
                    }
                }
            }

            catch(Exception e)
            {
                return;
            }
        }
    }

    /**Add a new listener callback
     * @param lst the new callback to call when new message as been parsed*/
    public void addListener(IMessageBufferCallback lst)
    {
        m_listeners.add(lst);
    }

    /**Add remove an already known listener callback
     * @param lst the listener callback to remove*/
    public void removeListener(IMessageBufferCallback lst)
    {
        m_listeners.remove(lst);
    }

    /** Allocate a new message type depending on m_type*/
    private void allocateNewMessage()
    {
        switch(m_type)
        {
            case GET_ADD_DATASET_ACKNOWLEDGE:
                m_curMsg      = new AcknowledgeAddDatasetMessage();
                m_curMsg.type = GET_ADD_DATASET_ACKNOWLEDGE;
                break;
            default:
                Log.e(MainActivity.TAG, "Unknown type " + m_type);
                m_type = GET_NO_TYPE;
                break;
        }
    }
}
