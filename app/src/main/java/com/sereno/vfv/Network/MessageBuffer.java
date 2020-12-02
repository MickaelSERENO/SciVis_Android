package com.sereno.vfv.Network;

import android.util.Log;

import com.sereno.vfv.Data.CloudPointDataset;
import com.sereno.vfv.MainActivity;

import java.util.ArrayList;

/** Message Buffer class. Permits to parse the incoming server data correctly
 * Format : 2 bytes -> typeOfMessage then follows (depending on the message) 4 bytes for int/float,
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

    /** The byte array value read*/
    private class ReadByteArray extends ReadValue
    {
        /** The value*/
        byte[] value;
    }

    /** Callback interface used when new message has been successfully parsed*/
    public interface IMessageBufferCallback
    {
        /** Called when an empty message has been received (i.e., received only a type with no expected data)
         * @param msg the message parsed*/
        void onEmptyMessage(EmptyMessage msg);

        /** Called when the message "ADD VTK DATASET" has been successfully parsed
         * @param msg the message parsed*/
        void onAddVTKDatasetMessage(AddVTKDatasetMessage msg);

        /** Called when the message "ROTATE DATASET" has been successfully parsed
         * @param msg the message parsed*/
        void onRotateDatasetMessage(RotateDatasetMessage msg);

        /** Called when the message "MOVE DATASET" has been successfully parsed
         * @param msg the message parsed*/
        void onMoveDatasetMessage(MoveDatasetMessage msg);

        /** Called when the message "SCALE DATASET" has been successfully parsed
         * @param msg the message parsed*/
        void onScaleDatasetMessage(ScaleDatasetMessage msg);

        /** Called when the message "TF DATASET" has been successfully parsed
         * @param msg the message parsed*/
        void onTFDatasetMessage(TFDatasetMessage msg);

        /** Called when the message "HEADSET_BINDING_INFO" has been successfully parsed
         * @param msg the message parsed*/
        void onHeadsetBindingInfoMessage(HeadsetBindingInfoMessage msg);

        /** Called when the message "GET_SUBDATASET_LOCK_OWNER" has been successfully parsed
         * @param msg the message parsed*/
        void onSubDatasetLockOwnerMessage(SubDatasetLockOwnerMessage msg);

        /** Called when the message "GET_HEADSETS_STATUS" has been successfully parsed
         * @param msg the message parsed*/
        void onHeadsetsStatusMessage(HeadsetsStatusMessage msg);

        /** Called when the message "GET_ANCHOR_ANNOTATION" has been successfully parsed
         * @param msg the message parsed*/
        void onAnchorAnnotation(AnchorAnnotationMessage msg);

        /** Called when the message "GET_CLEAR_ANNOTATIONS" has been successfully parsed
         * @param msg the message parsed*/
        void onClearAnnotations(ClearAnnotationsMessage msg);

        /** Called when the message "GET_ADD_SUBDATASET" has been successfully parsed
         * @param msg the message parsed*/
        void onAddSubDataset(AddSubDatasetMessage msg);

        /** Called when the message "GET_REMOVE_SUBDATASET" has been successfully parsed
         * @param msg the message parsed*/
        void onRemoveSubDataset(RemoveSubDatasetMessage msg);

        /** Called when the message "GET_SET_SUBDATASET_OWNER" has been successfully parsed
         * @param msg the message parsed*/
        void onSubDatasetOwnerMessage(SubDatasetOwnerMessage msg);

        /** Called when the message "GET_SET_SUBDATASET_OWNER" has been successfully parsed
         * @param msg the message parsed*/
        void onLocationTabletMessage(LocationTabletMessage msg);

        /** Called when the message "GET_ADD_CLOUDPOINT_DATASET" has been successfully parsed
         * @param msg the message parsed*/
        void onAddCloudPointDatasetMessage(AddCloudPointDatasetMessage msg);

        /** Called when the message "GET_TOGGLE_MAP_VISIBILITY" has been successfully parsed
         * @param msg the message parsed*/
        void onToggleMapVisibilityMessage(ToggleMapVisibilityMessage msg);

        /** Called when the message "GET_NEXT_TB_TRIAL" has been successfully parsed
         * @param msg the message parsed*/
        void onNextTBTrialMessage(NextTBTrialMessage msg);

        /** Called when the message "GET_RESET_VOLUMETRIC_SELECTION" has been successfully parsed
         * @param msg the message parsed*/
        void onResetVolumetricSelectionMessage(ResetVolumetricSelectionMessage msg);

        /** Called when the message "GET_VOLUMETRIC_MASK" has been successfully parsed
         * @param msg the message parsed*/
        void onSubDatasetVolumetricMaskMessage(SubDatasetVolumetricMaskMessage msg);
    }

    /** No current type received*/
    public static final int GET_NO_TYPE                 = -1;

    /** Add a new VTK Dataset*/
    public static final int GET_ADD_VTK_DATASET         = 0;

    /** Rotate dataset received*/
    public static final int GET_ROTATE_DATASET          = 2;

    /** Received the headset information*/
    public static final int GET_HEADSET_BINDING_INFO    = 4;

    /** Move dataset received*/
    public static final int GET_MOVE_DATASET            = 3;

    /** Headsets Status received*/
    public static final int GET_HEADSETS_STATUS         = 5;

    /** Received the subdataset locking this SubDataset*/
    public static final int GET_SUBDATASET_LOCK_OWNER   = 8;

    /** Scale dataset received*/
    public static final int GET_SCALE_DATASET           = 9;

    /** Transfer Function for a dataset received*/
    public static final int GET_TF_DATASET              = 10;

    /** Anchor a new annotation*/
    public static final int GET_ANCHOR_ANNOTATION       = 12;

    /** Clear all the annotations for a particular dataset*/
    public static final int GET_CLEAR_ANNOTATIONS       = 13;

    /** Add a new SubDataset*/
    public static final int GET_ADD_SUBDATASET          = 14;

    /** Remove a known SubDataset*/
    public static final int GET_DEL_SUBDATASET          = 15;

    public static final int GET_SET_SUBDATASET_OWNER    = 16;

    /** Get location data from tracker*/
    public static final int GET_LOCATION_TABLET         = 18;

    /** Add a cloud point dataset*/
    public static final int GET_ADD_CLOUD_POINT_DATASET = 23;

    /** Toggle the map visibility*/
    public static final int GET_TOGGLE_MAP_VISIBILITY   = 25;

    /** Get a volumetric mask to apply to a subdataset*/
    public static final int GET_VOLUMETRIC_MASK         = 26;

    /** Reset the volumetric selection of a particular subdataset*/
    public static final int GET_RESET_VOLUMETRIC_SELECTION = 27;

    /** Launch the next trial of the TangibleBrush project*/
    public static final int GET_NEXT_TB_TRIAL           = 28;

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

    /** The current byte array being parsed*/
    private byte[] m_byteArray = null;

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
        while(bufPos < readSize)
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
            while(m_curMsg != null && m_curMsg.cursor <= m_curMsg.getMaxCursor() && bufPos < readSize)
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

                    case 'a':
                    {
                        ReadByteArray val = readByteArray(buffer, bufPos, readSize);
                        bufPos = val.bufOff;
                        if(!val.valid)
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

            if(m_curMsg != null && m_curMsg.cursor > m_curMsg.getMaxCursor())
            {
                //When the message is finished, send it
                switch (m_curMsg.getType()) {
                    case GET_ADD_VTK_DATASET:
                        for (IMessageBufferCallback clbk : m_listeners)
                            clbk.onAddVTKDatasetMessage((AddVTKDatasetMessage) m_curMsg);
                        break;
                    case GET_ROTATE_DATASET:
                        for (IMessageBufferCallback clbk : m_listeners)
                            clbk.onRotateDatasetMessage((RotateDatasetMessage) m_curMsg);
                        break;
                    case GET_MOVE_DATASET:
                        for (IMessageBufferCallback clbk : m_listeners)
                            clbk.onMoveDatasetMessage((MoveDatasetMessage) m_curMsg);
                        break;
                    case GET_HEADSET_BINDING_INFO:
                        for (IMessageBufferCallback clbk : m_listeners)
                            clbk.onHeadsetBindingInfoMessage((HeadsetBindingInfoMessage) m_curMsg);
                        break;
                    case GET_HEADSETS_STATUS:
                        for (IMessageBufferCallback clbk : m_listeners)
                            clbk.onHeadsetsStatusMessage((HeadsetsStatusMessage) m_curMsg);
                        break;
                    case GET_SUBDATASET_LOCK_OWNER:
                        for (IMessageBufferCallback clbk : m_listeners)
                            clbk.onSubDatasetLockOwnerMessage((SubDatasetLockOwnerMessage) m_curMsg);
                        break;
                    case GET_SCALE_DATASET:
                        for (IMessageBufferCallback clbk : m_listeners)
                            clbk.onScaleDatasetMessage((ScaleDatasetMessage) m_curMsg);
                        break;
                    case GET_TF_DATASET:
                        for(IMessageBufferCallback clbk : m_listeners)
                            clbk.onTFDatasetMessage((TFDatasetMessage)m_curMsg);
                        break;
                    case GET_ANCHOR_ANNOTATION:
                        for (IMessageBufferCallback clbk : m_listeners)
                            clbk.onAnchorAnnotation((AnchorAnnotationMessage) m_curMsg);
                        break;
                    case GET_CLEAR_ANNOTATIONS:
                        for (IMessageBufferCallback clbk : m_listeners)
                            clbk.onClearAnnotations((ClearAnnotationsMessage) m_curMsg);
                        break;
                    case GET_ADD_SUBDATASET:
                        for (IMessageBufferCallback clbk : m_listeners)
                            clbk.onAddSubDataset((AddSubDatasetMessage)m_curMsg);
                        break;
                    case GET_DEL_SUBDATASET:
                        for (IMessageBufferCallback clbk : m_listeners)
                            clbk.onRemoveSubDataset((RemoveSubDatasetMessage)m_curMsg);
                        break;
                    case GET_SET_SUBDATASET_OWNER:
                        for (IMessageBufferCallback clbk : m_listeners)
                            clbk.onSubDatasetOwnerMessage((SubDatasetOwnerMessage)m_curMsg);
                        break;
                    case GET_LOCATION_TABLET:
                        for (IMessageBufferCallback clbk : m_listeners)
                            clbk.onLocationTabletMessage((LocationTabletMessage)m_curMsg);
                        break;
                    case GET_ADD_CLOUD_POINT_DATASET:
                        for(IMessageBufferCallback clbk : m_listeners)
                            clbk.onAddCloudPointDatasetMessage((AddCloudPointDatasetMessage)m_curMsg);
                        break;
                    case GET_TOGGLE_MAP_VISIBILITY:
                        for(IMessageBufferCallback clbk : m_listeners)
                            clbk.onToggleMapVisibilityMessage((ToggleMapVisibilityMessage) m_curMsg);
                        break;
                    case GET_NEXT_TB_TRIAL:
                        for(IMessageBufferCallback clbk : m_listeners)
                            clbk.onNextTBTrialMessage((NextTBTrialMessage)m_curMsg);
                        break;
                    case GET_RESET_VOLUMETRIC_SELECTION:
                        for(IMessageBufferCallback clbk : m_listeners)
                            clbk.onResetVolumetricSelectionMessage((ResetVolumetricSelectionMessage) m_curMsg);
                        break;
                    case GET_VOLUMETRIC_MASK:
                        for(IMessageBufferCallback clbk : m_listeners)
                            clbk.onSubDatasetVolumetricMaskMessage((SubDatasetVolumetricMaskMessage) m_curMsg);
                        break;
                    default:
                        Log.e(MainActivity.TAG, "Unknown type " + m_curMsg.getCurrentType() + ". No more data can be read without errors...");
                        break;
                }

                m_curMsg = null;
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

    /** Allocate a new message type depending on the type
     * @param type  the type of the message*/
    private void allocateNewMessage(short type)
    {
        switch(type)
        {
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
            case GET_SUBDATASET_LOCK_OWNER:
                m_curMsg = new SubDatasetLockOwnerMessage();
                break;
            case GET_SCALE_DATASET:
                m_curMsg = new ScaleDatasetMessage();
                break;
            case GET_TF_DATASET:
                m_curMsg = new TFDatasetMessage();
                break;
            case GET_HEADSETS_STATUS:
                m_curMsg = new HeadsetsStatusMessage();
                break;
            case GET_ANCHOR_ANNOTATION:
                m_curMsg = new AnchorAnnotationMessage();
                break;
            case GET_CLEAR_ANNOTATIONS:
                m_curMsg = new ClearAnnotationsMessage();
                break;
            case GET_ADD_SUBDATASET:
                m_curMsg = new AddSubDatasetMessage();
                break;
            case GET_DEL_SUBDATASET:
                m_curMsg = new RemoveSubDatasetMessage();
                break;
            case GET_SET_SUBDATASET_OWNER:
                m_curMsg = new SubDatasetOwnerMessage();
                break;
            case GET_LOCATION_TABLET:
                m_curMsg = new LocationTabletMessage();
                break;
            case GET_ADD_CLOUD_POINT_DATASET:
                m_curMsg = new AddCloudPointDatasetMessage();
                break;
            case GET_TOGGLE_MAP_VISIBILITY:
                m_curMsg = new ToggleMapVisibilityMessage();
                break;
            case GET_NEXT_TB_TRIAL:
                m_curMsg = new NextTBTrialMessage();
                break;
            case GET_RESET_VOLUMETRIC_SELECTION:
                m_curMsg = new ResetVolumetricSelectionMessage();
                break;
            case GET_VOLUMETRIC_MASK:
                m_curMsg = new SubDatasetVolumetricMaskMessage();
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

        if(readSize - offset < 2 - m_dataPos)
        {
            for(int i = offset; i < readSize; i++, val.bufOff++)
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
            for(int i = offset; i < readSize; i++, val.bufOff++)
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
            m_stringSize = -1;
        }
        return val;
    }

    /** Read a Byte Array (32 bits + n bits) in the incoming byte
     * @param data the incoming data
     * @param offset the offset in the data array
     * @param readSize the size of the data (initial size)
     * @return the value read*/
    private ReadByteArray readByteArray(byte[] data, int offset, int readSize)
    {
        ReadByteArray val = new ReadByteArray();
        val.valid  = false;
        val.bufOff = offset;

        if(m_byteArray == null)
        {
            ReadInt32 arraySize = readInt32(data, offset, readSize);
            val.bufOff = arraySize.bufOff;
            if(!arraySize.valid)
                return val;

            if(arraySize.value < 0)
            {
                Log.e(MainActivity.TAG, "Received a byte array size inferior than 0... Treat it as 0");
                m_stringSize = 0;
            }

            m_byteArray = new byte[arraySize.value];
        }

        for(; m_dataPos != m_byteArray.length && val.bufOff < readSize; val.bufOff++, m_dataPos++)
            m_byteArray[m_dataPos] = (byte)data[val.bufOff];

        if(m_dataPos == m_byteArray.length)
        {
            val.valid = true;
            val.value = m_byteArray;
            m_byteArray = null;
            m_dataPos = 0;
        }
        return val;
    }

}
