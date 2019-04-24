package com.sereno.vfv;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MotionEvent;
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
import com.sereno.vfv.Network.HeadsetsStatusMessage;
import com.sereno.vfv.Network.MessageBuffer;
import com.sereno.vfv.Network.MoveDatasetMessage;
import com.sereno.vfv.Network.RotateDatasetMessage;
import com.sereno.vfv.Network.ScaleDatasetMessage;
import com.sereno.vfv.Network.SubDatasetOwnerMessage;
import com.sereno.view.AnnotationData;
import com.sereno.view.RangeColorData;
import com.sereno.view.TreeView;

import java.util.ArrayList;

public class DatasetsFragment extends VFVFragment implements ApplicationModel.IDataCallback, SubDataset.ISubDatasetListener
{
    private VFVSurfaceView   m_surfaceView       = null; /*!< The surface view displaying the vector field*/
    private TreeView         m_previewLayout     = null; /*!< The preview layout*/
    private Bitmap           m_noSnapshotBmp     = null; /*!< The bitmap used when no preview is available*/
    private ImageView        m_headsetColor      = null; /*!< Image view representing the headset color*/
    private ApplicationModel m_model             = null; /*!< The application model to use*/

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

        onUpdateBindingInformation(m_model, m_model.getBindingInfo());
        for(BinaryDataset d : m_model.getBinaryDatasets())
            onAddBinaryDataset(m_model, d);
        for(VTKDataset d : m_model.getVTKDatasets())
            onAddVTKDataset(m_model, d);

        if(m_surfaceView != null)
            model.addListener(m_surfaceView);
    }

    @Override
    public void onAttach(Context context)
    {
        super.onAttach(context);
        setUpModel(m_model);
    }

    /** @brief Set the visibility of this fragment. Useful for optimization (do not draw what is not on screen)
     * @param visibility the new visibility state. false == not visible, true = visible*/
    public void setVisibility(boolean visibility)
    {
        m_surfaceView.setRenderVisibility(visibility ? View.VISIBLE : View.INVISIBLE);
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
    public void onAddAnnotation(ApplicationModel model, AnnotationData annot, ApplicationModel.AnnotationMetaData metaData)
    {}

    @Override
    public void onChangeCurrentAction(ApplicationModel model, int action) {
    }

    @Override
    public void onRangeColorChange(SubDataset sd, float min, float max, int mode)
    {}

    @Override
    public void onRotationEvent(SubDataset dataset, float[] quaternion)
    {}

    @Override
    public void onPositionEvent(SubDataset dataset, float[] position) {}

    @Override
    public void onScaleEvent(SubDataset dataset, float[] scale) {}

    @Override
    public void onSnapshotEvent(SubDataset dataset, Bitmap snapshot) {}

    @Override
    public void onAddAnnotation(SubDataset dataset, AnnotationData annotation) {}

    @Override
    public void onChangeCurrentSubDataset(ApplicationModel model, SubDataset sd)
    { }

    @Override
    public void onUpdateHeadsetsStatus(ApplicationModel model, HeadsetsStatusMessage.HeadsetStatus[] headsetsStatus) {}

    @Override
    public void onUpdateBindingInformation(ApplicationModel model, HeadsetBindingInfoMessage msg)
    {
        if(msg == null)
            return;
        if(msg.getHeadsetID() != -1)
        {
            final int color = msg.getHeadsetColor();
            m_headsetColor.setImageBitmap(null);
            m_headsetColor.setBackgroundColor((color & 0xffffff) + ((byte)0xff << 24));
        }
    }

    /** Set up the main layout
     * @param v the main view containing all the Widgets*/
    private void setUpMainLayout(View v)
    {
        m_surfaceView   = (VFVSurfaceView)v.findViewById(R.id.mainView);
        m_previewLayout = (TreeView)v.findViewById(R.id.previewLayout);
        m_headsetColor  = (ImageView)v.findViewById(R.id.headsetColor);

        if(m_model != null)
            m_model.addListener(m_surfaceView);

        //Surface view listeners
        m_surfaceView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                if(motionEvent.getAction() == MotionEvent.ACTION_DOWN)
                {
                    for(IFragmentListener l : ((VFVFragment)(DatasetsFragment.this)).m_listeners)
                        l.onDisableSwipping(DatasetsFragment.this);
                }
                else if(motionEvent.getAction() == MotionEvent.ACTION_UP)
                {
                    for(IFragmentListener l : ((VFVFragment)(DatasetsFragment.this)).m_listeners)
                        l.onEnableSwipping(DatasetsFragment.this);
                }
                return false;
            }
        });

        m_surfaceView.addListener(new VFVSurfaceView.IVFVSurfaceViewListener() {
            @Override
            public void onChangeCurrentAction(final int action) {
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if(m_model != null)
                            m_model.setCurrentAction(action);
                    }
                });
            }
        });
    }

    private void addDataset(final Dataset d)
    {
        //Add the preview
        TextView dataText = new TextView(getContext());
        dataText.setText(d.getName());
        Tree<View> dataView = new Tree<View>(dataText);
        m_previewLayout.getModel().addChild(dataView, -1);

        for(int i = 0; i < d.getNbSubDataset(); i++)
        {
            //Set the color range listener
            final SubDataset sd = d.getSubDataset(i);
            if(m_model.getCurrentSubDataset() == null)
                m_model.setCurrentSubDataset(sd);
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
                    m_model.setCurrentSubDataset(sd);
                }
            });

            //Snapshot event
            sd.addListener(new SubDataset.ISubDatasetListener() {
                @Override
                public void onRangeColorChange(SubDataset sd, float min, float max, int mode) {}


                @Override
                public void onRotationEvent(SubDataset dataset, float[] quaternion) {}

                @Override
                public void onPositionEvent(SubDataset dataset, float[] position) {}

                @Override
                public void onScaleEvent(SubDataset dataset, float[] scale) {}

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

                @Override
                public void onAddAnnotation(SubDataset dataset, AnnotationData annotation) {}
            });
            dataView.addChild(new Tree<View>(snapImg), -1);
        }


    }
}
