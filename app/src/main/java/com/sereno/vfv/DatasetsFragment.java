package com.sereno.vfv;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Bundle;
import android.view.GestureDetector;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.PopupMenu;
import android.widget.TextView;
import android.widget.Toast;

import com.sereno.Tree;
import com.sereno.gl.VFVSurfaceView;
import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.BinaryDataset;
import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Data.VTKDataset;
import com.sereno.vfv.Network.HeadsetBindingInfoMessage;
import com.sereno.vfv.Network.HeadsetsStatusMessage;
import com.sereno.view.AnnotationData;
import com.sereno.view.TreeView;

import java.util.ArrayList;
import java.util.HashMap;

public class DatasetsFragment extends VFVFragment implements ApplicationModel.IDataCallback
{
    /** Interface proposing callback methods regarding the DatasetsFragment*/
    public interface IDatasetsFragmentListener
    {
        /** Called when a SubDataset needs to be duplicated
         * @param frag the Fragment calling this method
         * @param sd the SubDataset to duplicate*/
        void onDuplicateSubDataset(DatasetsFragment frag, SubDataset sd);

        /** Called when a SubDataset needs to be removed
         * @param frag the Fragment calling this method
         * @param sd the SubDataset to duplicate*/
        void onRemoveSubDataset(DatasetsFragment frag, SubDataset sd);
    }

    private VFVSurfaceView   m_surfaceView       = null;  /*!< The surface view displaying the vector field*/
    private TreeView         m_previewLayout     = null;  /*!< The preview layout*/
    private Bitmap           m_noSnapshotBmp     = null;  /*!< The bitmap used when no preview is available*/
    private ImageView        m_headsetColor      = null;  /*!< Image view representing the headset color*/
    private ApplicationModel m_model             = null;  /*!< The application model to use*/
    private Context          m_ctx               = null;  /*!< The application context*/
    private boolean          m_modelBound        = false; /*!< Is the model bound?*/

    private HashMap<SubDataset, Tree<View>> m_sdTrees      = new HashMap<>(); /*!< HashMap binding subdataset to their represented Tree*/
    private HashMap<Dataset, Tree<View>>    m_datasetTrees = new HashMap<>(); /*!< HashMap binding dataset to their represented Tree*/

    private HashMap<SubDataset, ImageView> m_sdImages  = new HashMap<>(); /*!< HashMap binding subdataset to their represented ImageView*/

    private ArrayList<IDatasetsFragmentListener> m_dfListeners = new ArrayList<>();

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

        m_ctx = getContext();
        if(m_model != null && !m_modelBound)
            setUpModel(m_model);

        return v;
    }

    /** Set up the model callback through this fragment. Call this method only once!!!
     * @param model the model to link with the internal views*/
    public void setUpModel(ApplicationModel model)
    {
        if(m_model != null)
            m_model.removeListener(this);
        m_model = model;

        if(m_ctx != null)
        {
            onUpdateBindingInformation(m_model, m_model.getBindingInfo());
            for (BinaryDataset d : m_model.getBinaryDatasets())
                onAddBinaryDataset(m_model, d);
            for (VTKDataset d : m_model.getVTKDatasets())
                onAddVTKDataset(m_model, d);
            onChangeCurrentSubDataset(m_model, m_model.getCurrentSubDataset());

            if (m_surfaceView != null)
            {
                m_surfaceView.onUpdateBindingInformation(m_model, m_model.getBindingInfo());
                m_surfaceView.onUpdateHeadsetsStatus(m_model, m_model.getHeadsetsStatus());
                for (BinaryDataset d : m_model.getBinaryDatasets())
                    m_surfaceView.onAddBinaryDataset(m_model, d);
                for (VTKDataset d : m_model.getVTKDatasets())
                    m_surfaceView.onAddVTKDataset(m_model, d);
                m_surfaceView.onChangeCurrentSubDataset(m_model, m_model.getCurrentSubDataset());
                model.addListener(m_surfaceView);

            }

            m_modelBound = true;
            m_model.addListener(this);
        }
    }

    /** Remove an already registered listener for the DatasetsFragment specification
     * @param clbk the listener to not call anymore*/
    public void removeDFListener(IDatasetsFragmentListener clbk)
    {
        m_dfListeners.remove(clbk);
    }

    /** @brief Add a callback object to call at actions performed by the datasets fragment
     * @param clbk the new callback to take account of*/
    public void addDFListener(IDatasetsFragmentListener clbk)
    {
        if(!m_dfListeners.contains(clbk))
            m_dfListeners.add(clbk);
    }

    @Override
    public void onAttach(Context context)
    {
        super.onAttach(context);
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
    public void onAddSubDataset(ApplicationModel model, final SubDataset sd)
    {
        Tree<View> dataView = m_datasetTrees.get(sd.getParent());

        //Set the color range listener
        View layout = getLayoutInflater().inflate(R.layout.dataset_icon_layout, null);
        layout.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));

        final ImageView snapImg = (ImageView)layout.findViewById(R.id.snapshotImageView);
        final GestureDetector gestureDetector = new GestureDetector(getContext(), new GestureDetector.OnGestureListener() {
            @Override
            public boolean onDown(MotionEvent motionEvent) {return false;}

            @Override
            public void onShowPress(MotionEvent motionEvent) {}

            @Override
            public boolean onSingleTapUp(MotionEvent motionEvent) {return false;}

            @Override
            public boolean onScroll(MotionEvent motionEvent, MotionEvent motionEvent1, float v, float v1) {return false;}

            @Override
            public void onLongPress(MotionEvent motionEvent)
            {
                //Creating the instance of PopupMenu
                PopupMenu popup = new PopupMenu(getContext(), snapImg);
                //Inflating the Popup using xml file
                popup.getMenuInflater().inflate(R.menu.subdataset_menu, popup.getMenu());

                //registering popup with OnMenuItemClickListener
                popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
                    @Override
                    public boolean onMenuItemClick(MenuItem item) {
                        Toast.makeText(getContext(),"You Clicked : " + item.getTitle(), Toast.LENGTH_SHORT).show();
                        switch(item.getItemId())
                        {
                            case R.id.duplicateSD_item:
                                break;

                            case R.id.removeSD_item:
                                break;
                        }
                        return true;
                    }
                });
                popup.show();//showing popup menu
            }

            @Override
            public boolean onFling(MotionEvent motionEvent, MotionEvent motionEvent1, float v, float v1) {return false;}
        });

        snapImg.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                return gestureDetector.onTouchEvent(motionEvent);
            }
        });

        //Add the snap image
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
        snapImg.setPadding(10, 10, 10, 10);

        final ImageView publicIcon  = (ImageView)layout.findViewById(R.id.datasetPublicIcon);
        final ImageView privateIcon = (ImageView)layout.findViewById(R.id.datasetPrivateIcon);

        SubDataset.ISubDatasetListener snapEvent = new SubDataset.ISubDatasetListener()
        {
            @Override
            public void onClampingChange(SubDataset sd, float min, float max) {}


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

            @Override
            public void onRemove(SubDataset dataset)
            {
                if(!m_sdTrees.containsKey(dataset))
                    return;
                m_sdTrees.get(dataset).setParent(null, 0);
                m_sdTrees.remove(dataset);
                if(m_sdImages.containsKey(dataset))
                    m_sdImages.remove(dataset);
            }

            @Override
            public void onRemoveAnnotation(SubDataset dataset, AnnotationData annotation) {}
        };

        //Snapshot event
        sd.addListener(snapEvent);
        m_sdImages.put(sd, snapImg);

        if(m_model.getCurrentSubDataset() == null)
            m_model.setCurrentSubDataset(sd);

        Tree<View> layoutTree = new Tree<View>(layout);
        dataView.addChild(layoutTree, -1);
        m_sdTrees.put(sd, layoutTree);
    }

    @Override
    public void onAddAnnotation(ApplicationModel model, AnnotationData annot, ApplicationModel.AnnotationMetaData metaData)
    {}

    @Override
    public void onPendingAnnotation(ApplicationModel model, SubDataset sd) {

    }

    @Override
    public void onEndPendingAnnotation(ApplicationModel model, SubDataset sd, boolean cancel) {

    }

    @Override
    public void onChangeCurrentAction(ApplicationModel model, int action) {
    }


    @Override
    public void onChangeCurrentSubDataset(ApplicationModel model, SubDataset sd)
    {
        for(ImageView v : m_sdImages.values())
            v.setBackgroundResource(0);
        if(sd != null)
            m_sdImages.get(sd).setBackgroundResource(R.drawable.round_rectangle_background);
    }

    @Override
    public void onUpdateHeadsetsStatus(ApplicationModel model, HeadsetsStatusMessage.HeadsetStatus[] headsetsStatus) {}

    @Override
    public void onUpdateBindingInformation(ApplicationModel model, HeadsetBindingInfoMessage msg)
    {
        if(msg != null && msg.getHeadsetID() != -1)
        {
            final int color = msg.getHeadsetColor();
            m_headsetColor.setImageBitmap(null);
            m_headsetColor.setBackgroundColor((color & 0xffffff) + ((byte)0xff << 24));
        }
        else
        {
            m_headsetColor.setImageDrawable(getResources().getDrawable(R.drawable.no_snapshot));
            m_headsetColor.setBackgroundColor(Color.TRANSPARENT);
        }
    }

    @Override
    public void onRemoveDataset(ApplicationModel model, Dataset dataset)
    {
        if(!m_datasetTrees.containsKey(dataset))
            return;

        m_datasetTrees.get(dataset).setParent(null, -1);
        m_datasetTrees.remove(dataset);
    }

    @Override
    public void onUpdatePointingTechnique(ApplicationModel model, int pt) {}

    /** Set up the main layout
     * @param v the main view containing all the Widgets*/
    private void setUpMainLayout(View v)
    {
        m_surfaceView   = (VFVSurfaceView)v.findViewById(R.id.mainView);
        m_previewLayout = (TreeView)v.findViewById(R.id.previewLayout);
        m_headsetColor  = (ImageView)v.findViewById(R.id.headsetColor);

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
        dataText.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));
        dataText.setText(d.getName());

        Tree<View> dataView = new Tree<View>(dataText);
        m_previewLayout.getModel().addChild(dataView, -1);
        m_datasetTrees.put(d, dataView);
    }
}
