package com.sereno.vfv.Network;

import android.util.Log;

import com.sereno.vfv.MainActivity;

import java.util.ArrayList;

/** Message Buffer class. Permits to parse the incoming server data correctly
 * Format : 2byte -> typeOfMessage then follows (depending on the message) 4 bytes for int/float,
 *          2 bytes for short and/or 4 bytes + n bytes for String (n depending on the first 4 bytes received)*/
public class MessageBuffer
{
    /** A valid read*/
    private class ReadValue
    {
        /** Is the value valid?*/
        boolean valid = false;
        /** The new buffer offset to apply*/
        int     bufOff = 0;
    }

    /** The Int16 value read*/
    private class ReadInt16 extends ReadValue
    {
        /** The value*/
        short value;
    }

    /** The Int32 value read*/
    private class ReadInt32 extends ReadValue
    {
        /** The value*/
        int value;
    }

    /** The float value read*/
    private class ReadFloat extends ReadValue
    {
        /** The value*/
        float value;
    }

    /** The String value read*/
    private class ReadString extends ReadValue
    {
        /** The value*/
        String  value;
    }

    /** Callback interface used when new message has been successfully parsed*/
    public interface IMessageBufferCallback
    {
        /** Called when an empty message has been received (i.e., received only a type with no expected data)
         * @param msg the message parsed*/
        void onEmptyMessage(EmptyMessage msg);

        /** Called when the message acknowledge add dataset has been successfully parsed
         * @param msg the message parsed*/
        void onAcknowledgeAddDatasetMessage(AcknowledgeAddDatasetMessage msg);

        /** Called when the message "ADD VTK DATASET" has been successfully parsed
         * @param msg the message parsed*/
        void onAddVTKDatasetMessage(AddVTKDatasetMessage msg);

        /** Called when the message "ROTATE DATASET" has been successfully parsed
         * @param msg the message parsed*/
        void onRotateDatasetMessage(RotateDatasetMessage msg);

        /** Called when the message "MOVE DATASET" has been successfully parsed
         * @param msg the message parsed*/
        void onMoveDatasetMessage(MoveDatasetMessage msg);

        /** Called when the message "HEADSET_BINDING_INFO" has been successfully parsed
         * @param msg the message parsed*/
        void onHeadsetBindingInfoMessage(HeadsetBindingInfoMessage msg);

        /** Called when the message "GET_SUBDATASET_OWNER" has been successfully parsed
         * @param msg the message parsed*/
        void onSubDatasetOwnerMessage(SubDatasetOwnerMessage msg);
    }

    /** No current type received*/
    public static final int GET_NO_TYPE                 = -1;

    /** Add a new VTK Dataset*/
    public static final int GET_ADD_VTK_DATASET         = 0;

    /** Add dataset acknowledge received*/
    public static final int GET_ADD_DATASET_ACKNOWLEDGE = 1;

    /** Rotate dataset received*/
    public static final int GET_ROTATE_DATASET          = 2;

    /** Received the headset information*/
    public static final int GET_HEADSET_BINDING_INFO    = 4;

    /** Move dataset received*/
    public static final int GET_MOVE_DATASET            = 3;

    /** Received the subdataset owning information*/
    public static final int GET_SUBDATASET_OWNER        = 8;

    /** The current message being parsed*/
    private ServerMessage m_curMsg = null;

    /** The buffer internal position*/
    private int m_dataPos = 0;

    /** The buffer*/
    private byte[] m_data = new byte[4];

    /** Special string buffer for strings*/
    private StringBuilder m_stringBuf  = null;
    /** The total amount of size for the current string buffer being used*/
    private int m_stringSize = -1;

    private ArrayList<IMessageBufferCallback> m_listeners = new ArrayList<>();

    /**Constructor, initialize every variable to default*/
    public MessageBuffer()
    {}

    /**Push values present in the buffer
     * @param buffer  the buffer to read
     * @param readSize the size of the buffer*/
    public void push(byte[] buffer, int readSize)
    {
        int bufPos = 0;
        while(true)
        {
            //If no current message -> fetch the next message type
            if(m_curMsg == null)
            {
                ReadInt16 val = readInt16(buffer, bufPos, readSize);
                bufPos = val.bufOff;
                if(!val.valid)
                    return;
                allocateNewMessage(val.value);
            }

            if(m_curMsg == null)
                continue;

            //Read the data
            while(m_curMsg != null && m_curMsg.cursor <= m_curMsg.getMaxCursor())
            {
                switch(m_curMsg.getCurrentType())
                {
                    case 's':
                     {
                        ReadString val = readString(buffer, bufPos, readSize);
                        bufPos = val.bufOff;
                        if (!val.valid)
                            return;
                        m_curMsg.pushValue(val.value);
                        break;
                    }
                    case 'i':
                    {
                        ReadInt16 val = readInt16(buffer, bufPos, readSize);
                        bufPos = val.bufOff;
                        if(!val.valid)
                            return;
                        m_curMsg.pushValue(val.value);
                        break;
                    }
                    case 'I':
                    {
                        ReadInt32 val = readInt32(buffer, bufPos, readSize);
                        bufPos = val.bufOff;
                        if(!val.valid)
                            return;
                        m_curMsg.pushValue(val.value);
                        break;
                    }
                    case 'f':
                    {
                        ReadFloat val = readFloat(buffer, bufPos, readSize);
                        bufPos = val.bufOff;
                        if(!val.valid)
                            return;
                        m_curMsg.pushValue(val.value);
                        break;
                    }
                    case 'b':
                    {
                        m_curMsg.pushValue(buffer[bufPos]);
                        bufPos++;
                        break;
                    }
                    default:
                        Log.e(MainActivity.TAG, "Unhandled type " + m_curMsg.getCurrentType() + ". Skipping");
                        break;
                }
            }

            //When the message is finished, send it
            switch(m_curMsg.getType())
            {
                case GET_ADD_DATASET_ACKNOWLEDGE:
                    for(IMessageBufferCallback clbk : m_listeners)
                        clbk.onAcknowledgeAddDatasetMessage((AcknowledgeAddDatasetMessage) m_curMsg);
                    break;
                case GET_ADD_VTK_DATASET:
                    for(IMessageBufferCallback clbk : m_listeners)
                        clbk.onAddVTKDatasetMessage((AddVTKDatasetMessage) m_curMsg);
                    break;
                case GET_ROTATE_DATASET:
                    for(IMessageBufferCallback clbk : m_listeners)
                        clbk.onRotateDatasetMessage((RotateDatasetMessage) m_curMsg);
                    break;
                case GET_MOVE_DATASET:
                    for(IMessageBufferCallback clbk : m_listeners)
                        clbk.onMoveDatasetMessage((MoveDatasetMessage) m_curMsg);
                    break;
                case GET_HEADSET_BINDING_INFO:
                    for(IMessageBufferCallback clbk : m_listeners)
                        clbk.onHeadsetBindingInfoMessage((HeadsetBindingInfoMessage) m_curMsg);
                    break;
                case GET_SUBDATASET_OWNER:
                    for(IMessageBufferCallback clbk : m_listeners)
                        clbk.onSubDatasetOwnerMessage((SubDatasetOwnerMessage) m_curMsg);
                    break;
                default:
                    Log.e(MainActivity.TAG, "Unknown type " + m_curMsg.getCurrentType() + ". No more data can be read without errors...");
            }
            m_curMsg = null;
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

    /** Allocate a new message type depending on the type
     * @param type  the type of the message*/
    private void allocateNewMessage(short type)
    {
        switch(type)
        {
            case GET_ADD_DATASET_ACKNOWLEDGE:
                m_curMsg = new AcknowledgeAddDatasetMessage();
                break;
            case GET_ADD_VTK_DATASET:
                m_curMsg = new AddVTKDatasetMessage();
                break;
            case GET_ROTATE_DATASET:
                m_curMsg = new RotateDatasetMessage();
                break;
            case GET_MOVE_DATASET:
                m_curMsg = new MoveDatasetMessage();
                break;
            case GET_HEADSET_BINDING_INFO:
                m_curMsg = new HeadsetBindingInfoMessage();
                break;
            case GET_SUBDATASET_OWNER:
                m_curMsg = new SubDatasetOwnerMessage();
                break;
            default:
                Log.e(MainActivity.TAG, "Unknown type " + type + ". No more data can be read without errors...");
                return;
        }

        m_curMsg.type = type;
    }

    /** Read 16 bits integer in the incoming byte
     * @param data the incoming data
     * @param offset the offset in the data array
     * @param readSize the size of the data (initial size)
     * @return the value read*/
    private ReadInt16 readInt16(byte[] data, int offset, int readSize)
    {
        ReadInt16 val = new ReadInt16();
        val.bufOff = offset;
        val.valid  = false;

        if(readSize+m_dataPos - offset < 2)
        {
            for(int i = offset; i < readSize - offset; i++, val.bufOff++)
                m_data[m_dataPos++] = data[i];
            return val;
        }

        for(int i = val.bufOff; m_dataPos < 2; i++, val.bufOff++)
            m_data[m_dataPos++] = data[i];

        val.value = (short)((m_data[0] << 8) +
                            (m_data[1]));
        val.valid = true;
        m_dataPos = 0;
        return val;
    }

    /** Read 32 bits integer in the incoming byte
     * @param data the incoming data
     * @param offset the offset in the data array
     * @param readSize the size of the data (initial size)
     * @return the value read*/
    private ReadInt32 readInt32(byte[] data, int offset, int readSize)
    {
        ReadInt32 val = new ReadInt32();
        val.bufOff = offset;
        val.valid  = false;

        if(readSize + m_dataPos - offset < 4)
        {
            for(int i = offset; i < readSize - offset; i++, val.bufOff++)
                m_data[m_dataPos++] = data[i];
            return val;
        }

        for(int i = offset; m_dataPos < 4; m_dataPos++, i++, val.bufOff++)
            m_data[m_dataPos] = data[i];

        val.value = (int)(((m_data[0] & 0xff) << 24) +
                          ((m_data[1] & 0xff) << 16) +
                          ((m_data[2] & 0xff) << 8) +
                          (m_data[3] & 0xff));
        val.valid = true;
        m_dataPos = 0;
        return val;
    }

    /** Read 32 bits float in the incoming byte
     * @param data the incoming data
     * @param offset the offset in the data array
     * @return the value read*/
    private ReadFloat readFloat(byte[] data, int offset, int readSize)
    {
        ReadFloat val   = new ReadFloat();
        ReadInt32 val32 = readInt32(data, offset, readSize);

        val.valid  = val32.valid;
        val.bufOff = val32.bufOff;
        if(val32.valid)
            val.value = Float.intBitsToFloat(val32.value);
        return val;
    }

    /** Read String (32 bits + n bits) in the incoming byte
     * @param data the incoming data
     * @param offset the offset in the data array
     * @param readSize the size of the data (initial size)
     * @return the value read*/
    private ReadString readString(byte[] data, int offset, int readSize)
    {
        ReadString val = new ReadString();
        val.valid  = false;
        val.bufOff = offset;

        if(m_stringSize < 0)
        {
            ReadInt32 stringSize = readInt32(data, offset, readSize);
            val.bufOff = stringSize.bufOff;
            if(!stringSize.valid)
                return val;

            m_stringSize = stringSize.value;
            if(m_stringSize < 0)
            {
                Log.e(MainActivity.TAG, "Received a string buffer size inferior than 0... Treat it as 0");
                m_stringSize = 0;
            }
            m_stringBuf  = new StringBuilder(m_stringSize);
        }
        while(m_stringBuf.length() != m_stringSize && val.bufOff < readSize)
        {
            m_stringBuf.append((char)data[val.bufOff++]);
        }

        if(m_stringBuf.length() == m_stringSize)
        {
            val.valid = true;
            val.value = m_stringBuf.toString();
        }
        return val;
    }
}
