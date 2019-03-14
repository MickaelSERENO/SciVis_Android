package com.sereno.vfv;

import android.support.v4.app.Fragment;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.sereno.Tree;
import com.sereno.gl.VFVSurfaceView;
import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.BinaryDataset;
import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Data.VTKDataset;
import com.sereno.vfv.Network.AcknowledgeAddDatasetMessage;
import com.sereno.vfv.Network.AddVTKDatasetMessage;
import com.sereno.vfv.Network.EmptyMessage;
import com.sereno.vfv.Network.HeadsetBindingInfoMessage;
import com.sereno.vfv.Network.MessageBuffer;
import com.sereno.vfv.Network.MoveDatasetMessage;
import com.sereno.vfv.Network.RotateDatasetMessage;
import com.sereno.view.RangeColorData;
import com.sereno.view.TreeView;

import java.util.ArrayList;

public class DatasetsFragment extends Fragment implements ApplicationModel.IDataCallback, SubDataset.ISubDatasetCallback, MessageBuffer.IMessageBufferCallback, RangeColorData.IOnRangeChangeListener
{
    /** @brief The Listener used when DatasetFragment is modifying its internal states*/
    public interface IDatasetFragmentListener
    {
        /** Called wheter the current subdataset being displayed changed
         * @param d the current subdataset being displayed*/
        void onChangeCurrentSubDataset(SubDataset d);
    }

    private VFVSurfaceView   m_surfaceView       = null; /*!< The surface view displaying the vector field*/
    private TreeView         m_previewLayout     = null; /*!< The preview layout*/
    private Bitmap           m_noSnapshotBmp     = null; /*!< The bitmap used when no preview is available*/
    private SubDataset       m_currentSubDataset = null; /*!< The current application sub dataset*/
    private ImageView        m_headsetColor      = null; /*!< Image view representing the headset color*/
    private ApplicationModel m_model             = null; /*!< The application model to use*/

    private ArrayList<IDatasetFragmentListener> m_listeners = new ArrayList<>();

    public DatasetsFragment()
    {
        super();
    }

    /** @brief OnCreate function. Called when the activity is on creation*/
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        m_noSnapshotBmp = BitmapFactory.decodeResource(getResources(), R.drawable.no_snapshot);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceStates)
    {
        View v = inflater.inflate(R.layout.datasets_fragment, container, false);
        setUpMainLayout(v);
        return v;
    }

    /** Set up the model callback through this fragment
     * @param model the model to link with the internal views*/
    public void setUpModel(ApplicationModel model)
    {
        m_model = model;
        m_model.addListener(this);
        if(m_surfaceView != null)
            model.addListener(m_surfaceView);
    }

    /** @brief Add a listener object to call when the internal states of this fragment changes
     * @param l the new listener to take account of*/
    public void addListener(IDatasetFragmentListener l)
    {
        if(!m_listeners.contains(l))
            m_listeners.add(l);
    }

    /** @brief Remove a listener object to call when the internal states of this fragment changes
     * @param l the new listener to take account of*/
    public void removeListener(IDatasetFragmentListener l)
    {
        m_listeners.remove(l);
    }

    @Override
    public void onRangeChange(RangeColorData data, float minVal, float maxVal, int mode)
    {
        if(m_currentSubDataset != null)
            m_currentSubDataset.setRangeColor(minVal, maxVal, mode);
    }

    @Override
    public void onAddBinaryDataset(ApplicationModel model, BinaryDataset d)
    {
        if(d.getID() < 0)
            addDataset(d);
    }

    @Override
    public void onAddVTKDataset(ApplicationModel model, VTKDataset d)
    {
        addDataset(d);
    }

    @Override
    public void onRangeColorChange(SubDataset sd, float min, float max, int mode)
    {}

    @Override
    public void onRotationEvent(SubDataset dataset, float dRoll, float dPitch, float dYaw)
    {}

    @Override
    public void onRotationEvent(SubDataset dataset, float[] quaternion)
    {}

    @Override
    public void onSnapshotEvent(SubDataset dataset, Bitmap snapshot) {}

    @Override
    public void onEmptyMessage(EmptyMessage msg)
    {
        if(msg.getType() == MessageBuffer.GET_HEADSET_DISCONNECTED)
        {
            getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    m_headsetColor.setImageBitmap(m_noSnapshotBmp);
                }
            });
        }
    }

    @Override
    public void onAcknowledgeAddDatasetMessage(AcknowledgeAddDatasetMessage msg)
    {}

    @Override
    public void onAddVTKDatasetMessage(AddVTKDatasetMessage msg)
    {}

    @Override
    public void onRotateDatasetMessage(RotateDatasetMessage msg)
    {}

    private void changeCurrentSubDataset(SubDataset sd)
    {
        m_currentSubDataset = sd;
        m_surfaceView.changeCurrentSubDataset(sd);

        for(IDatasetFragmentListener l : m_listeners)
            l.onChangeCurrentSubDataset(sd);
    }

    @Override
    public void onMoveDatasetMessage(MoveDatasetMessage msg)
    {
        //TODO
    }

    @Override
    public void onHeadsetBindingInfoMessage(HeadsetBindingInfoMessage msg)
    {
        final int color = msg.getHeadsetColor();
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                m_headsetColor.setImageBitmap(null);
                m_headsetColor.setBackgroundColor((color & 0xffffff) + ((byte)0xff << 24));
            }
        });
    }

    /** Set up the main layout*/
    private void setUpMainLayout(View v)
    {
        m_surfaceView   = (VFVSurfaceView)v.findViewById(R.id.mainView);
        m_previewLayout = (TreeView)v.findViewById(R.id.previewLayout);
        m_headsetColor = (ImageView)v.findViewById(R.id.headsetColor);

        if(m_model != null)
            m_model.addListener(m_surfaceView);
    }

    private void addDataset(final Dataset d)
    {
        //Add the preview
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                TextView dataText = new TextView(getContext());
                dataText.setText(d.getName());
                Tree<View> dataView = new Tree<View>(dataText);
                m_previewLayout.getData().addChild(dataView, -1);

                for(int i = 0; i < d.getNbSubDataset(); i++)
                {
                    //Set the color range listener
                    final SubDataset sd = d.getSubDataset(i);
                    if(m_currentSubDataset == null)
                        m_currentSubDataset = sd;
                    sd.addListener(DatasetsFragment.this);

                    //Add the snap image
                    final ImageView snapImg = new ImageView(getContext());
                    snapImg.setAdjustViewBounds(true);
                    snapImg.setScaleType(ImageView.ScaleType.FIT_CENTER);
                    snapImg.setMaxWidth(256);
                    snapImg.setMaxHeight(256);
                    snapImg.setImageResource(R.drawable.no_snapshot);
                    snapImg.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            changeCurrentSubDataset(sd);
                        }
                    });

                    //Snapshot event
                    sd.addListener(new SubDataset.ISubDatasetCallback() {
                        @Override
                        public void onRangeColorChange(SubDataset sd, float min, float max, int mode) {}

                        @Override
                        public void onRotationEvent(SubDataset dataset, float dRoll, float dPitch, float dYaw) {}

                        @Override
                        public void onRotationEvent(SubDataset dataset, float[] quaternion) {}

                        @Override
                        public void onSnapshotEvent(SubDataset dataset, Bitmap snapshot)
                        {
                            final Bitmap s = snapshot;
                            getActivity().runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    snapImg.setImageBitmap(s);
                                }
                            });
                        }
                    });
                    dataView.addChild(new Tree<View>(snapImg), -1);
                }
            }
        });

    }
}
