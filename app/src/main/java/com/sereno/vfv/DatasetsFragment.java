package com.sereno.vfv;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
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
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.PopupMenu;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.sereno.Tree;
import com.sereno.gl.GLSurfaceView;
import com.sereno.gl.VFVSurfaceView;
import com.sereno.vfv.Data.Annotation.AnnotationLogContainer;
import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.CloudPointDataset;
import com.sereno.vfv.Data.TF.TransferFunction;
import com.sereno.vfv.Data.VectorFieldDataset;
import com.sereno.vfv.Data.CPCPTexture;
import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Data.VTKDataset;
import com.sereno.vfv.Dialog.Listener.INoticeCreateSDDialogListener;
import com.sereno.vfv.Dialog.OpenCreateSDDialog;
import com.sereno.vfv.Network.HeadsetBindingInfoMessage;
import com.sereno.vfv.Network.HeadsetsStatusMessage;
import com.sereno.view.AnnotationCanvasData;
import com.sereno.view.TreeView;

import java.util.ArrayList;
import java.util.HashMap;

public class DatasetsFragment extends VFVFragment implements ApplicationModel.IDataCallback, Dataset.IDatasetListener
{
    /** Interface proposing callback methods regarding the DatasetsFragment*/
    public interface IDatasetsFragmentListener
    {
        /** Called when a SubDatasets needs to be renamed
         * @param frag the Fragment calling this method
         * @param sd the SubDataset to rename
         * @param name the new name to apply*/
        void onRenameSubDataset(DatasetsFragment frag, SubDataset sd, String name);

        /** Called when a SubDataset needs to be duplicated
         * @param frag the Fragment calling this method
         * @param sd the SubDataset to duplicate*/
        void onDuplicateSubDataset(DatasetsFragment frag, SubDataset sd);

        /** Called when a SubDataset needs to be removed
         * @param frag the Fragment calling this method
         * @param sd the SubDataset to duplicate*/
        void onRequestRemoveSubDataset(DatasetsFragment frag, SubDataset sd);

        /** Called when the fragment ask to add a new SubDataset for a given dataset
         * @param frag the Fragment calling this method
         * @param d the Dataset to consider
         * @param publicSD should the SubDataset be public?*/
        void onRequestAddSubDataset(DatasetsFragment frag, Dataset d, boolean publicSD);

        /** Called when the fragment ask to make a SubDataset public
         * @param frag the Fragment calling this method
         * @param sd the SubDataset to consider*/
        void onRequestMakeSubDatasetPublic(DatasetsFragment frag, SubDataset sd);

        /** Called when the fragment ask to change the visibility of the map associated to a particular SubDataset
         * @param frag the Fragment calling this method
         * @param sd the SubDataset to consider
         * @param visibility the new map visibility to apply*/
        void onRequestChangeSubDatasetMapVisibility(DatasetsFragment frag, SubDataset sd, boolean visibility);

        /** Called when the fragment ask to merge two different subdatasets
         * @param frag the Fragment calling this method
         * @param sd1 the first SubDataset to merge
         * @param sd2 the second SubDataset to merge*/
        void onMergeSubDatasets(DatasetsFragment frag, SubDataset sd1, SubDataset sd2);

        /** Reset the volumetric selection performed on one particular subdataset
         * @param frag the Fragment calling this method
         * @param sd the subdataset to reset the selection on*/
        void onResetVolumetricSelection(DatasetsFragment frag, SubDataset sd);
    }

    public static final float INCH_TO_METER   = 0.0254f;
    public static final int   TIME_SLIDER_MAX = 10;

    private VFVSurfaceView   m_surfaceView       = null;  /*!< The surface view displaying the vector field*/
    private ViewGroup m_surfaceViewVolumeSelectLayout = null; /*!< The layout containing all the widgets to display during a volume selection process*/
    private ViewGroup m_tangibleLayout = null; /*!< The layout containing all the tangible button*/
    private TreeView         m_previewLayout     = null;  /*!< The preview layout*/
    private Bitmap           m_noSnapshotBmp     = null;  /*!< The bitmap used when no preview is available*/
    private ImageView        m_headsetColor      = null;  /*!< Image view representing the headset color*/
    private ApplicationModel m_model             = null;  /*!< The application model to use*/
    private Context          m_ctx               = null;  /*!< The application context*/
    private boolean          m_modelBound        = false; /*!< Is the model bound?*/

    private HashMap<SubDataset, Tree<View>> m_sdTrees      = new HashMap<>(); /*!< HashMap binding subdataset to their represented Tree*/
    private HashMap<Dataset, Tree<View>>    m_datasetTrees = new HashMap<>(); /*!< HashMap binding dataset to their represented Tree*/

    private HashMap<SubDataset, ImageView> m_sdImages  = new HashMap<>();     /*!< HashMap binding subdataset to their represented ImageView*/

    private ArrayList<IDatasetsFragmentListener> m_dfListeners = new ArrayList<>(); /*!< Object that registered to this DatasetFragment events*/

    private SubDataset m_inMergedSubDataset = null; /*!< Is there an object that requested to be merged?*/

    /** Selection menu buttons*/
    private Button m_startSelectionBtn   = null;
    private Button m_endSelectionBtn     = null;
    private Button m_confirmSelectionBtn = null;

    /** Boolean buttons*/
    private ImageButton m_unionBtn = null;
    private ImageButton m_interBtn = null;
    private ImageButton m_minusBtn = null;

    /** Time buttons*/
    private ImageButton m_playTimeBtn  = null;
    private ImageButton m_pauseTimeBtn = null;
    private SeekBar     m_timeSlider   = null;

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
            for (VectorFieldDataset d : m_model.getVectorFieldDatasets())
                onAddVectorFieldDataset(m_model, d);
            for (VTKDataset d : m_model.getVTKDatasets())
                onAddVTKDataset(m_model, d);
            for (CloudPointDataset d : model.getCloudPointDataset())
                onAddCloudPointDataset(m_model, d);
            onChangeCurrentSubDataset(m_model, m_model.getCurrentSubDataset());

            if (m_surfaceView != null)
            {
                m_surfaceView.onUpdateBindingInformation(m_model, m_model.getBindingInfo());
                m_surfaceView.onUpdateHeadsetsStatus(m_model, m_model.getHeadsetsStatus());
                for (VectorFieldDataset d : m_model.getVectorFieldDatasets())
                    m_surfaceView.onAddVectorFieldDataset(m_model, d);
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
    public void onAddVectorFieldDataset(ApplicationModel model, VectorFieldDataset d)
    {
        addDataset(d);
    }

    @Override
    public void onAddCloudPointDataset(ApplicationModel model, CloudPointDataset d)
    {
        addDataset(d);
    }

    @Override
    public void onAddVTKDataset(ApplicationModel model, VTKDataset d)
    {
        addDataset(d);
    }

    @Override
    public void onRemoveSubDataset(Dataset dataset, SubDataset sd)
    {}

    @Override
    public void onAddSubDataset(Dataset dataset, final SubDataset sd)
    {
        Tree<View> dataView = m_datasetTrees.get(sd.getParent());

        //Set the color range listener
        View layout = getLayoutInflater().inflate(R.layout.dataset_icon_layout, null);
        layout.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));

        final ImageView snapImg = (ImageView)layout.findViewById(R.id.snapshotImageView);
        final GestureDetector gestureDetector = new GestureDetector(getContext(), new GestureDetector.OnGestureListener() {
            @Override
            public boolean onDown(MotionEvent motionEvent)
            {
                if(m_inMergedSubDataset != null && m_inMergedSubDataset != sd)
                {
                    for(IDatasetsFragmentListener listener : m_dfListeners)
                        listener.onMergeSubDatasets(DatasetsFragment.this, m_inMergedSubDataset, sd);
                    m_inMergedSubDataset = null;
                    return true;
                }
                else if(m_inMergedSubDataset != null)
                {
                    Toast.makeText(getContext(),"Merging Cancelled", Toast.LENGTH_SHORT).show();
                    m_inMergedSubDataset = null;
                }
                return false;
            }

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

                //Remove useless items
                if(!sd.getCanBeModified() || sd.getOwnerID() == -1 || //Cannot be modified or already public
                   (sd.getOwnerID() != -1 && sd.getOwnerID() != m_model.getBindingInfo().getHeadsetID())) //If not public but not our subdataset
                    popup.getMenu().findItem(R.id.makePublicSD_item).setVisible(false);

                //Toggle the correct visibility button
                if(sd.getMapVisibility())
                    popup.getMenu().findItem(R.id.enableMap_item).setVisible(false);
                else
                    popup.getMenu().findItem(R.id.disableMap_item).setVisible(false);


                //registering popup with OnMenuItemClickListener
                popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
                    @Override
                    public boolean onMenuItemClick(MenuItem item) {
                        Toast.makeText(getContext(),"You Clicked : " + item.getTitle(), Toast.LENGTH_SHORT).show();
                        switch(item.getItemId())
                        {
                            case R.id.rename_item:
                                //Open an alert dialog to ask for the new name
                                AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
                                builder.setTitle(getResources().getString(R.string.renameTitle));

                                final EditText input = new EditText(getContext());
                                builder.setView(input);

                                builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        for(IDatasetsFragmentListener listener : m_dfListeners)
                                            listener.onRenameSubDataset(DatasetsFragment.this, sd, input.getText().toString());
                                    }
                                });
                                builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        dialog.cancel();
                                    }
                                });

                                builder.show();
                                break;

                            case R.id.duplicateSD_item:
                                for(IDatasetsFragmentListener listener : m_dfListeners)
                                    listener.onDuplicateSubDataset(DatasetsFragment.this, sd);
                                break;

                            case R.id.removeSD_item:
                                for(IDatasetsFragmentListener listener : m_dfListeners)
                                    listener.onRequestRemoveSubDataset(DatasetsFragment.this, sd);
                                break;

                            case R.id.makePublicSD_item:
                                for(IDatasetsFragmentListener listener : m_dfListeners)
                                    listener.onRequestMakeSubDatasetPublic(DatasetsFragment.this, sd);
                                break;

                            case R.id.disableMap_item:
                                for(IDatasetsFragmentListener listener : m_dfListeners)
                                    listener.onRequestChangeSubDatasetMapVisibility(DatasetsFragment.this, sd, false);
                                break;

                            case R.id.enableMap_item:
                                for(IDatasetsFragmentListener listener : m_dfListeners)
                                    listener.onRequestChangeSubDatasetMapVisibility(DatasetsFragment.this, sd, true);
                                break;

                            case R.id.mergeWith_item:
                                m_inMergedSubDataset = sd;
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

        //Handle the privacy icons
        final ImageView publicIcon  = (ImageView)layout.findViewById(R.id.datasetPublicIcon);
        final ImageView privateIcon = (ImageView)layout.findViewById(R.id.datasetPrivateIcon);

        //Snapshot event
        m_sdImages.put(sd, snapImg);

        final Tree<View> layoutTree = new Tree<View>(layout);
        dataView.addChild(layoutTree, -1);
        m_sdTrees.put(sd, layoutTree);

        SubDataset.ISubDatasetListener snapEvent = new SubDataset.ISubDatasetListener()
        {
            @Override
            public void onRotationEvent(SubDataset dataset, float[] quaternion) {}

            @Override
            public void onPositionEvent(SubDataset dataset, float[] position) {}

            @Override
            public void onScaleEvent(SubDataset dataset, float[] scale) {}

            @Override
            public void onSnapshotEvent(final SubDataset dataset, Bitmap snapshot)
            {
                final Bitmap s = snapshot;
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if(dataset.getParent().isLoaded())
                            snapImg.setImageBitmap(s);
                    }
                });
            }

            @Override
            public void onAddCanvasAnnotation(SubDataset dataset, AnnotationCanvasData annotation) {}

            @Override
            public void onRemove(SubDataset dataset)
            {
                if(!m_sdTrees.containsKey(dataset))
                    return;
                layoutTree.setParent(null, 0);
                m_sdTrees.remove(dataset);
                if(m_sdImages.containsKey(dataset))
                    m_sdImages.remove(dataset);
            }

            @Override
            public void onRemoveCanvasAnnotation(SubDataset dataset, AnnotationCanvasData annotation) {}

            @Override
            public void onUpdateTF(SubDataset dataset)
            {
                if(dataset == m_model.getCurrentSubDataset())
                    updateTimeWidgets();
            }

            @Override
            public void onSetCurrentHeadset(SubDataset dataset, int headsetID) {}

            @Override
            public void onSetOwner(SubDataset dataset, int headsetID)
            {
                //Show public / our datasets, hide the others.
                if(headsetID == -1 || headsetID == m_model.getBindingInfo().getHeadsetID())
                    layoutTree.value.setVisibility(View.VISIBLE);
                else
                    layoutTree.value.setVisibility(View.GONE);

                if(sd.getOwnerID() != -1)
                {
                    publicIcon.setVisibility(View.GONE);
                    privateIcon.setVisibility(View.VISIBLE);
                }
                else
                {
                    publicIcon.setVisibility(View.VISIBLE);
                    privateIcon.setVisibility(View.GONE);
                }
            }

            @Override
            public void onSetCanBeModified(SubDataset dataset, boolean status)
            {}

            @Override
            public void onSetMapVisibility(SubDataset dataset, boolean visibility)
            {}

            @Override
            public void onSetVolumetricMask(SubDataset dataset)
            {}
        };
        sd.addListener(snapEvent);
        snapEvent.onSetOwner(sd, sd.getOwnerID());
    }

    @Override
    public void onLoadDataset(Dataset dataset, boolean success) {
        updateTimeWidgets();
    }

    @Override
    public void onLoadCPCPTexture(Dataset dataset, CPCPTexture texture) {}

    @Override
    public void onLoad1DHistogram(Dataset dataset, float[] values, int pID) {}

    @Override
    public void onAddCanvasAnnotation(ApplicationModel model, AnnotationCanvasData annot, ApplicationModel.AnnotationMetaData metaData)
    {}

    @Override
    public void onPendingCanvasAnnotation(ApplicationModel model, SubDataset sd) {

    }

    @Override
    public void onEndPendingCanvasAnnotation(ApplicationModel model, SubDataset sd, boolean cancel) {

    }

    @Override
    public void onChangeCurrentAction(ApplicationModel model, int action) {
    }


    @Override
    public void onChangeCurrentSubDataset(ApplicationModel model, SubDataset sd)
    {
        for(ImageView v : m_sdImages.values())
            v.setBackgroundResource(0);
        if(sd != null && m_sdImages.containsKey(sd))
            m_sdImages.get(sd).setBackgroundResource(R.drawable.round_rectangle_background);
        updateTimeWidgets();
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
    public void onRemoveAnnotationLog(ApplicationModel model, AnnotationLogContainer annot) {}

    @Override
    public void onUpdatePointingTechnique(ApplicationModel model, int pt) {}

    @Override
    public void onChangeTimeAnimationStatus(ApplicationModel model, boolean isInPlay, int speed, float step)
    {
        updateTimeWidgets();
    }

    @Override
    public void onSetSelectionMode(ApplicationModel model, int selectMode) {}

    @Override
    public void onAddAnnotationLog(ApplicationModel model, AnnotationLogContainer container)
    {}

    @Override
    public void onSetLocation(ApplicationModel model, float[] pos, float[] rot) {}

    @Override
    public void onSetTabletScale(ApplicationModel model, float scale, float width, float height, float posx, float posy) {}

    @Override
    public void onSetLasso(ApplicationModel model, float[] lasso) {}

    @Override
    public void onConfirmSelection(ApplicationModel model) {}

    @Override
    public void onSetCurrentBooleanOperation(ApplicationModel model, int op)
    {
        //Change the boolean button appearance
        m_unionBtn.setBackgroundResource(R.drawable.add_button_unchecked);
        m_minusBtn.setBackgroundResource(R.drawable.minus_button_unchecked);
        m_interBtn.setBackgroundResource(R.drawable.inter_button_unchecked);

        switch(op)
        {
            case ApplicationModel.BOOLEAN_UNION:
                m_unionBtn.setBackgroundResource(R.drawable.add_button_checked);
                break;

            case ApplicationModel.BOOLEAN_INTERSECTION:
                m_interBtn.setBackgroundResource(R.drawable.inter_button_checked);
                break;

            case ApplicationModel.BOOLEAN_MINUS:
                m_minusBtn.setBackgroundResource(R.drawable.minus_button_checked);
                break;
        }
    }

    @Override
    public void onSetTangibleMode(ApplicationModel model, int inTangibleMode)
    {}

    /** Update all the widgets for time-manipulations*/
    private void updateTimeWidgets()
    {
        SubDataset sd = m_model.getCurrentSubDataset();
        if(sd == null)
            return;

        TransferFunction tf = sd.getTransferFunction();
        if(tf == null)
            return;

        //Update the time slider
        m_timeSlider.setMax(TIME_SLIDER_MAX*sd.getParent().getNbTimesteps());
        m_timeSlider.setProgress((int)(tf.getTimestep()*TIME_SLIDER_MAX));

        //Update the play/pause buttons
        if(m_model.isTimeAnimationPlaying())
        {
            m_playTimeBtn.setVisibility(View.GONE);
            m_pauseTimeBtn.setVisibility(View.VISIBLE);
        }
        else
        {
            m_pauseTimeBtn.setVisibility(View.GONE);
            m_playTimeBtn.setVisibility(View.VISIBLE);
        }
    }

    /** Set up the main layout
     * @param v the main view containing all the Widgets*/
    private void setUpMainLayout(View v)
    {
        m_surfaceView   = (VFVSurfaceView)v.findViewById(R.id.surfaceView);
        m_previewLayout = (TreeView)v.findViewById(R.id.previewLayout);
        m_headsetColor  = (ImageView)v.findViewById(R.id.headsetColor);


        /** Setup the selection menu*/
        final SeekBar tabletScaleBar = (SeekBar)v.findViewById(R.id.tabletScaleBar);
        tabletScaleBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if(fromUser) {
                    updateScale(progress);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

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

            @Override
            public void onSetLasso(final float[] data)
            {
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if(m_model != null)
                            m_model.setLasso(data);
                    }
                });
            }
        });

        m_surfaceView.addListener(new GLSurfaceView.GLSurfaceViewListener()
        {
            @Override
            public void onSurfaceChanged(GLSurfaceView view, int format, int width, int height)
            {
                updateScale(tabletScaleBar.getProgress());
            }

            @Override
            public void onSurfaceDestroyed(GLSurfaceView view)
            {}

            @Override
            public void onSurfaceCreated(GLSurfaceView view)
            {}
        });


        m_tangibleLayout = (ViewGroup) v.findViewById(R.id.tangibleLayout);
        m_tangibleLayout.setVisibility(View.GONE);

        m_startSelectionBtn      = (Button) v.findViewById(R.id.startSelection);
        m_endSelectionBtn        = (Button) v.findViewById(R.id.endSelection);
        m_confirmSelectionBtn    = (Button) v.findViewById(R.id.confirmSelection);
        ImageButton tangibleBtn  = (ImageButton) v.findViewById(R.id.tangibleButton);
        ImageButton setOriginBtn = (ImageButton) v.findViewById(R.id.originButton);

        m_startSelectionBtn.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                m_surfaceView.setSelection(true);
                setSVFullScreen(true);
                m_surfaceViewVolumeSelectLayout.setVisibility(View.VISIBLE);
                m_confirmSelectionBtn.setVisibility(view.GONE);
                m_endSelectionBtn.setText(R.string.endSelection);
                updateScale(tabletScaleBar.getProgress());
                m_model.setCurrentBooleanOperation(ApplicationModel.BOOLEAN_UNION); //Default == Union
            }
        });

        m_endSelectionBtn.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                if(m_model.getCurrentAction() == m_model.CURRENT_ACTION_SELECTING)
                {
                    m_model.setCurrentAction(m_model.CURRENT_ACTION_REVIEWING_SELECTION);
                    m_confirmSelectionBtn.setVisibility(View.VISIBLE);
                    m_endSelectionBtn.setText(R.string.cancelSelection);
                }else{
                    m_surfaceView.setSelection(false);
                    setSVFullScreen(false);
                    m_surfaceViewVolumeSelectLayout.setVisibility(View.GONE);
                }
            }
        });

        m_confirmSelectionBtn.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                m_model.confirmSelection();
                m_surfaceView.setSelection(false);
                setSVFullScreen(false);
                m_surfaceViewVolumeSelectLayout.setVisibility(View.GONE);
            }
        });

        tangibleBtn.setOnTouchListener(new View.OnTouchListener()
        {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent)
            {
                if(motionEvent.getPointerCount() == 1)
                {
                    if(motionEvent.getAction() == MotionEvent.ACTION_DOWN)
                        m_model.setTangibleMode(ApplicationModel.TANGIBLE_MODE_MOVE);
                    else if(motionEvent.getAction() == MotionEvent.ACTION_UP || motionEvent.getAction() == MotionEvent.ACTION_CANCEL)
                        m_model.setTangibleMode(ApplicationModel.TANGIBLE_MODE_NONE);
                }
                return false;
            }
        });

        setOriginBtn.setOnTouchListener(new View.OnTouchListener()
        {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent)
            {
                if(motionEvent.getPointerCount() == 1)
                {
                    if(motionEvent.getAction() == MotionEvent.ACTION_DOWN)
                        m_model.setTangibleMode(ApplicationModel.TANGIBLE_MODE_ORIGIN);
                    else if(motionEvent.getAction() == MotionEvent.ACTION_UP || motionEvent.getAction() == MotionEvent.ACTION_CANCEL)
                        m_model.setTangibleMode(ApplicationModel.TANGIBLE_MODE_NONE);
                }
                return false;
            }
        });

        m_surfaceViewVolumeSelectLayout = v.findViewById(R.id.volumeLayoutInMV);
        m_surfaceViewVolumeSelectLayout.setVisibility(View.GONE);

        m_unionBtn = (ImageButton)v.findViewById(R.id.unionButton);
        m_minusBtn = (ImageButton)v.findViewById(R.id.minusButton);
        m_interBtn = (ImageButton)v.findViewById(R.id.intersectionButton);

        m_unionBtn.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                m_model.setCurrentBooleanOperation(ApplicationModel.BOOLEAN_UNION);
            }
        });

        m_minusBtn.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                m_model.setCurrentBooleanOperation(ApplicationModel.BOOLEAN_MINUS);
            }
        });

        m_interBtn.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                m_model.setCurrentBooleanOperation(ApplicationModel.BOOLEAN_INTERSECTION);
            }
        });

        //Handle constrain volume selection mode
        ToggleButton constrainSelection = v.findViewById(R.id.constraintSelection);
        constrainSelection.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener()
        {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b)
            {
                m_model.changeConstrainVolumeSelectionMode(b);
            }
        });

        v.findViewById(R.id.resetSelection).setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                for(IDatasetsFragmentListener l : m_dfListeners)
                    l.onResetVolumetricSelection(DatasetsFragment.this, m_model.getCurrentSubDataset());
            }
        });

        updateScale(tabletScaleBar.getProgress());

        //Handle time buttons
        m_pauseTimeBtn = v.findViewById(R.id.pauseTimeButton);
        m_playTimeBtn  = v.findViewById(R.id.playTimeButton);
        m_timeSlider   = v.findViewById(R.id.timeSlider);

        m_pauseTimeBtn.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                m_model.setTimeAnimationStatus(false, 500, 1.0f/3.0f);
            }
        });

        m_playTimeBtn.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                m_model.setTimeAnimationStatus(true, 500, 1.0f/3.0f);
            }
        });

        m_timeSlider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
        {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b)
            {
                SubDataset sd = m_model.getCurrentSubDataset();
                float t = (float)i/TIME_SLIDER_MAX;
                if(sd != null && sd.getTransferFunction() != null && sd.getParent().isLoaded() && t != sd.getTransferFunction().getTimestep())
                    sd.getTransferFunction().setTimestep(t);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar)
            {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar)
            {

            }
        });
    }

    public void updateScale(int progress)
    {
        float xdpi = getResources().getDisplayMetrics().xdpi;
        float ydpi = getResources().getDisplayMetrics().ydpi;
        int[] position = new int[2];
        m_surfaceView.getLocationOnScreen(position);
        m_model.setTabletScale(progress/xdpi*INCH_TO_METER,
                m_surfaceView.getWidth(),
                m_surfaceView.getHeight(),
                position[0],
                position[1]);
    }

    /** Set or unset the "fullscreen" parameter of the main SurfaceView Widget.
     * This function will in fact make everything else "disappear" (View.GONE) or "appear" (View.VISIBLE)
     * @param fullScreen true to set the SurfaceView in fullscreen, false to reput everything back to "normal"*/
    public void setSVFullScreen(boolean fullScreen)
    {
        View view = getView();
        if(view instanceof ViewGroup)
        {
            ViewGroup viewGroup = (ViewGroup) view;
            for(int i = 0; i < viewGroup.getChildCount(); ++i)
            {
                View child = viewGroup.getChildAt(i);
                child.setVisibility(fullScreen ? View.GONE : View.VISIBLE);
            }
        }

        if(fullScreen && view != null)
        {
            view.findViewById(R.id.mainView).setVisibility(View.VISIBLE);
            m_surfaceView.setVisibility(View.VISIBLE);
            m_tangibleLayout.setVisibility(View.VISIBLE);
        }

        else
        {
            m_tangibleLayout.setVisibility(View.GONE);
        }
    }

    /** Generic function called when a new Dataset is being added
     * @param d the Dataset being added*/
    private void addDataset(final Dataset d)
    {
        d.addListener(this);

        //Add the corresponding TreeView.
        //First initialize the View displayed as "header" of the TreeView
        View l = getActivity().getLayoutInflater().inflate(R.layout.dataset_key_entry, null);
        TextView dataText = l.findViewById(R.id.dataset_key_entry_name); //The text
        dataText.setText(d.getName());
        l.findViewById(R.id.dataset_key_entry_add).setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                if (motionEvent.getAction() == MotionEvent.ACTION_DOWN) {
                    //Create a dialog asking for the properties of the
                    final OpenCreateSDDialog sdDialog = new OpenCreateSDDialog(m_ctx, d);
                    sdDialog.open(new INoticeCreateSDDialogListener()
                    {
                        @Override
                        public void onDialogPositiveClick(OpenCreateSDDialog dialog)
                        {
                            for(IDatasetsFragmentListener l : m_dfListeners)
                                l.onRequestAddSubDataset(DatasetsFragment.this, d, sdDialog.isSDPublic());
                        }

                        @Override
                        public void onDialogNegativeClick(OpenCreateSDDialog dialog)
                        {}
                    });

                    return true;
                }
                return false;
            }
        });

        Tree<View> dataView = new Tree<View>(l);
        m_previewLayout.getModel().addChild(dataView, -1);
        m_datasetTrees.put(d, dataView);
    }
}
