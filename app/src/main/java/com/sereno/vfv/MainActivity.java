package com.sereno.vfv;

import android.app.DialogFragment;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.support.design.widget.TabLayout;
import android.support.v4.app.Fragment;
import android.support.v4.view.GravityCompat;
import android.support.v4.view.ViewPager;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.ToggleButton;

import com.sereno.VFVViewPager;
import com.sereno.math.Quaternion;
import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.CPCPTexture;
import com.sereno.vfv.Data.CloudPointDataset;
import com.sereno.vfv.Data.DataFile;
import com.sereno.vfv.Data.TF.MergeTFData;
import com.sereno.vfv.Data.VectorFieldDataset;
import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.PointFieldDesc;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Data.VTKDataset;
import com.sereno.vfv.Data.VTKFieldValue;
import com.sereno.vfv.Data.VTKParser;
import com.sereno.vfv.Dialog.OpenConnectDialogFragment;
import com.sereno.vfv.Dialog.OpenDatasetDialogFragment;
import com.sereno.vfv.Dialog.OpenVTKDatasetDialog;
import com.sereno.vfv.Dialog.Listener.INoticeDialogListener;
import com.sereno.vfv.Dialog.Listener.INoticeVTKDialogListener;
import com.sereno.vfv.Network.AddCloudPointDatasetMessage;
import com.sereno.vfv.Network.AddSubDatasetMessage;
import com.sereno.vfv.Network.AddVTKDatasetMessage;
import com.sereno.vfv.Network.AnchorAnnotationMessage;
import com.sereno.vfv.Network.ClearAnnotationsMessage;
import com.sereno.vfv.Network.EmptyMessage;
import com.sereno.vfv.Network.HeadsetBindingInfoMessage;
import com.sereno.vfv.Network.HeadsetsStatusMessage;
import com.sereno.vfv.Network.LocationTabletMessage;
import com.sereno.vfv.Network.MessageBuffer;
import com.sereno.vfv.Network.MoveDatasetMessage;
import com.sereno.vfv.Network.NextTBTrialMessage;
import com.sereno.vfv.Network.RemoveSubDatasetMessage;
import com.sereno.vfv.Network.ResetVolumetricSelectionMessage;
import com.sereno.vfv.Network.RotateDatasetMessage;
import com.sereno.vfv.Network.ScaleDatasetMessage;
import com.sereno.vfv.Network.SocketManager;
import com.sereno.vfv.Network.SubDatasetLockOwnerMessage;
import com.sereno.vfv.Network.SubDatasetOwnerMessage;
import com.sereno.vfv.Network.SubDatasetVolumetricMaskMessage;
import com.sereno.vfv.Network.TFDatasetMessage;
import com.sereno.vfv.Network.ToggleMapVisibilityMessage;
import com.sereno.view.AnnotationData;
import com.sereno.view.AnnotationStroke;
import com.sereno.view.AnnotationText;
import com.sereno.vfv.Data.TF.GTFData;
import com.sereno.view.GTFView;
import com.sereno.view.RangeColorView;
import com.sereno.view.SeekBarHintView;
import com.sereno.vfv.Data.TF.TransferFunction;

import java.io.File;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

/* \brief The MainActivity. First Activity to be launched*/
public class MainActivity extends AppCompatActivity
                          implements ApplicationModel.IDataCallback, SubDataset.ISubDatasetListener,
                                     MessageBuffer.IMessageBufferCallback, VFVFragment.IFragmentListener, AnnotationData.IAnnotationDataListener,
                                     SocketManager.ISocketManagerListener, Dataset.IDatasetListener, DatasetsFragment.IDatasetsFragmentListener
{
    /** Dataset Binding structure containing data permitting the remote server to identify which dataset we are performing operations*/
    public static class DatasetIDBinding
    {
        /** The subdataset server ID*/
        public int subDatasetID;

        /** The parent dataset*/
        public Dataset dataset;

        /** Constructor
         * @param dataset the parent dataset
         * @param subDatasetID the subdataset ID*/
        public DatasetIDBinding(Dataset dataset, int subDatasetID)
        {
            this.dataset      = dataset;
            this.subDatasetID = subDatasetID;
        }
    }

    public static final String TAG="VFV";

    private ApplicationModel m_model;                    /*!< The application data model */
    private DrawerLayout     m_drawerLayout;             /*!< The root layout. DrawerLayout permit to have a left menu*/
    private Button           m_deleteDataBtn;            /*!< The delete data button*/
    private SocketManager    m_socket;                   /*!< Connection with the server application*/
    private VFVViewPager     m_viewPager;                /*!< The view pager handling all our fragments*/
    private DatasetsFragment m_dataFragment = null;      /*!< The Dataset windows*/
    private Menu             m_menu = null;              /*!< The menu item (toolbar menu)*/
    private int              m_currentTFViewType = SubDataset.TRANSFER_FUNCTION_NONE;
    private ViewGroup        m_currentTFView = null;     /*!< The Current transfer function view to use*/
    private HashMap<Integer, View>  m_gtfSizeViews = new HashMap<>(); /*!< The views handling the size of the GTF*/
    private SubDataset       m_currentTFSubDataset = null;

    /** @brief OnCreate function. Called when the activity is on creation*/
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        m_model = new ApplicationModel(this);
        m_model.addListener(this);

        setContentView(R.layout.main_activity);

        //Set up all internal components
        setUpDrawerLayout();
        setUpMainLayout();
        setUpToolbar();
        setUpHiddenMenu();

        m_socket = new SocketManager(m_model.getConfiguration().getServerIP(), m_model.getConfiguration().getServerPort());
        m_socket.getMessageBuffer().addListener(this);
        m_socket.addListener(this);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        super.onWindowFocusChanged(hasFocus);
        if(hasFocus)
        {
            getWindow().getDecorView().setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_IMMERSIVE
                            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
        }
    }

    /** @brief Function called when the options items from the Toolbar are selected
     * @param item the MenuItem selected*/
    public boolean onOptionsItemSelected(MenuItem item)
    {
        switch (item.getItemId())
        {
            //Open the drawer (the home button)
            case android.R.id.home:
                m_drawerLayout.openDrawer(GravityCompat.START);
                return true;

            //Handles the connect dialog (connection to the server and hololens)
            case R.id.connect_item:
            {
                OpenConnectDialogFragment dialogFragment = new OpenConnectDialogFragment();
                dialogFragment.setNoticeDialogListener(new INoticeDialogListener()
                {
                    @Override
                    public void onDialogPositiveClick(DialogFragment dialogFrag, View view)
                    {
                        EditText txt = (EditText)view.findViewById(R.id.hololensIP);
                        Spinner handedness = (Spinner)view.findViewById(R.id.handedness);
                        m_socket.setIdentInformation(txt.getText().toString(), (int)handedness.getSelectedItemId(), m_model.getConfiguration().getTabletID());
                    }

                    @Override
                    public void onDialogNegativeClick(DialogFragment dialogFrag, View view)
                    {
                    }
                });
                dialogFragment.show(getFragmentManager(), "dialog");
                return true;
            }

            case R.id.manualPointing_item:
                m_model.setCurrentPointingTechnique(ApplicationModel.POINTING_MANUAL);
                break;

            case R.id.wimPointing_item:
                m_model.setCurrentPointingTechnique(ApplicationModel.POINTING_WIM);
                break;

            case R.id.wimRayPointing_item:
                m_model.setCurrentPointingTechnique(ApplicationModel.POINTING_WIM_POINTER);
                break;

            case R.id.gogoPointing_item:
                m_model.setCurrentPointingTechnique(ApplicationModel.POINTING_GOGO);
                break;

            case R.id.absoluteSelectionMode_item:
                m_model.setCurrentSelectionMode(ApplicationModel.SELECTION_MODE_ABSOLUTE);
                break;

            case R.id.relativeSelectionModeAligned_item:
                m_model.setCurrentSelectionMode(ApplicationModel.SELECTION_MODE_RELATIVE_ALIGNED);
                break;

            case R.id.relativeSelectionModeFull_item:
                m_model.setCurrentSelectionMode(ApplicationModel.SELECTION_MODE_RELATIVE_FULL);
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    private DatasetIDBinding getDatasetIDBinding(SubDataset sd)
    {
        Dataset parent       = null;
        int     subDatasetID = -1;

        if(sd != null)
        {
            //Fetch to which Dataset this SubDataset belongs to
            for(Dataset d : m_model.getDatasets())
            {
                if(parent != null)
                    break;

                for(int i = 0; i < d.getNbSubDataset(); i++)
                {
                    if(d.getSubDatasets().get(i).getNativePtr() == sd.getNativePtr())
                    {
                        parent       = d;
                        subDatasetID = d.getSubDatasets().get(i).getID();
                    }
                }
            }
        }
        return new DatasetIDBinding(parent, subDatasetID);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        getMenuInflater().inflate(R.menu.toolbar_menu, menu);
        m_menu = menu;
        super.onCreateOptionsMenu(menu);
        onUpdatePointingTechnique(m_model, m_model.getCurrentPointingTechnique());
        onSetSelectionMode(m_model, m_model.getCurrentSelectionMode());

        //Hide the WIM pointer during CHI 2020 experiment
        m_menu.findItem(R.id.wimRayPointing_item).setVisible(false);

        return true;
    }

    @Override
    public void onAddVectorFieldDataset(ApplicationModel model, VectorFieldDataset d)
    {
        onAddDataset(d);
    }

    @Override
    public void onAddCloudPointDataset(ApplicationModel model, CloudPointDataset d)
    {
        onAddDataset(d);
    }

    @Override
    public void onAddVTKDataset(ApplicationModel model, VTKDataset d)
    {
        onAddDataset(d);
    }

    @Override
    public void onRemoveSubDataset(Dataset dataset, SubDataset sd){}

    @Override
    public void onAddSubDataset(Dataset dataset, SubDataset sd)
    {
         sd.addListener(this);
    }

    @Override
    public void onLoadDataset(final Dataset dataset, boolean success)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                //If the current subdataset is inside, reload data
                for (SubDataset sd : dataset.getSubDatasets())
                {
                    if (sd == m_model.getCurrentSubDataset())
                    {
                        redoTFWidget();
                        break;
                    }
                }
            }
        });
    }

    @Override
    public void onLoadCPCPTexture(Dataset dataset, CPCPTexture texture) {}

    @Override
    public void onLoad1DHistogram(Dataset dataset, float[] values, int pID) {}

    @Override
    public void onAddAnnotation(ApplicationModel model, AnnotationData annot, ApplicationModel.AnnotationMetaData metaData)
    {
        annot.addListener(this);
    }

    @Override
    public void onPendingAnnotation(ApplicationModel model, SubDataset sd)
    {
        if(m_model.getBindingInfo() != null && m_model.getBindingInfo().getHeadsetID() != -1)
        {
            DatasetIDBinding idBinding = getDatasetIDBinding(sd);
            if(idBinding.subDatasetID != -1 && idBinding.dataset != null && idBinding.dataset.getID() >= 0)
                m_socket.push(SocketManager.createStartAnnotationEvent(idBinding, m_model.getCurrentPointingTechnique()));
        }
    }

    @Override
    public void onEndPendingAnnotation(ApplicationModel model, SubDataset sd, boolean cancel)
    {}

    @Override
    public void onChangeCurrentAction(ApplicationModel model, int action) {
        m_socket.push(SocketManager.createCurrentActionEvent(action));
        if(action != ApplicationModel.CURRENT_ACTION_REVIEWING_SELECTION &&
           action != ApplicationModel.CURRENT_ACTION_LASSO &&
           action != ApplicationModel.CURRENT_ACTION_SELECTING)
            m_model.setPostReviewRotation(new Quaternion());
    }

    @Override
    public void onRotationEvent(SubDataset dataset, float[] quaternion)
    {
        if(!m_model.canModifySubDataset(dataset)) //forget about it
            return;

        DatasetIDBinding idBinding = getDatasetIDBinding(dataset);

        //If everything is correct, send the rotation event
        if(idBinding.subDatasetID != -1 && idBinding.dataset != null && idBinding.dataset.getID() >= 0)
            m_socket.push(SocketManager.createRotationEvent(idBinding, dataset.getRotation()));
    }

    @Override
    public void onPositionEvent(SubDataset dataset, float[] position)
    {
        if(!m_model.canModifySubDataset(dataset)) //forget about it
            return;

        DatasetIDBinding idBinding = getDatasetIDBinding(dataset);

        //If everything is correct, send the position event
        if(idBinding.subDatasetID != -1 && idBinding.dataset != null && idBinding.dataset.getID() >= 0)
            m_socket.push(SocketManager.createPositionEvent(idBinding, dataset.getPosition()));
    }

    @Override
    public void onScaleEvent(SubDataset dataset, float[] scale)
    {
        if(!m_model.canModifySubDataset(dataset)) //forget about it
            return;

        DatasetIDBinding idBinding = getDatasetIDBinding(dataset);

        //If everything is correct, send the rotation event
        if(idBinding.subDatasetID != -1 && idBinding.dataset != null && idBinding.dataset.getID() >= 0)
            m_socket.push(SocketManager.createScaleEvent(idBinding, dataset.getScale()));
    }

    @Override
    public void onSnapshotEvent(SubDataset dataset, Bitmap snapshot) {}

    @Override
    public void onAddAnnotation(SubDataset dataset, AnnotationData annotation) {}

    @Override
    public void onRemove(SubDataset dataset)
    {}

    @Override
    public void onRemoveAnnotation(SubDataset dataset, AnnotationData annotation) {}

    @Override
    public void onUpdateTF(SubDataset dataset)
    {
        if(m_model.canModifySubDataset(dataset) && dataset.getTransferFunctionType() != SubDataset.TRANSFER_FUNCTION_NONE)
            m_socket.push(SocketManager.createTFEvent(getDatasetIDBinding(dataset), dataset.getTransferFunction()));

        if(dataset == m_model.getCurrentSubDataset())
            redoTFWidget();
    }

    /** Redo all the transfer function widgets and/or update them accordingly*/
    private void redoTFWidget()
    {
        SubDataset dataset = m_model.getCurrentSubDataset();
        if(dataset == null)
        {
            removeCurrentTFView();
            return;
        }

        if(dataset.getTransferFunctionType() != m_currentTFViewType || m_currentTFSubDataset != dataset)
        {
            recreateCurrentTFView();
        }

        if(dataset.getTransferFunctionType() == SubDataset.TRANSFER_FUNCTION_GTF ||
           dataset.getTransferFunctionType() == SubDataset.TRANSFER_FUNCTION_TGTF)
        {
            updateGTFWidgets();
        }
        else if(dataset.getTransferFunctionType() == SubDataset.TRANSFER_FUNCTION_MERGE)
        {
            updateMergeTFWidgets();
        }

        updateCommonTFWidgets();
    }

    /** Update all the widgets common to all transfer functions, such as time*/
    private void updateCommonTFWidgets()
    {

    }

    /** Remove the current transfer function view*/
    private void removeCurrentTFView()
    {
        if(m_currentTFViewType == SubDataset.TRANSFER_FUNCTION_GTF || m_currentTFViewType == SubDataset.TRANSFER_FUNCTION_TGTF)
            m_gtfSizeViews.clear();

        if(m_currentTFView != null)
        {
            if(m_currentTFView.getParent() != null)
                ((ViewGroup)m_currentTFView.getParent()).removeView(m_currentTFView);
            m_currentTFView       = null;
            m_currentTFViewType   = SubDataset.TRANSFER_FUNCTION_NONE;
            m_currentTFSubDataset = null;
        }
    }

    /** Recreate the current transfer function view based on the current SubDataset's TF type*/
    private void recreateCurrentTFView()
    {
        removeCurrentTFView();

        ViewGroup layout = m_drawerLayout.findViewById(R.id.tfViewLayout);
        int type = m_model.getCurrentSubDataset().getTransferFunctionType();

        switch(type)
        {
            case SubDataset.TRANSFER_FUNCTION_GTF:
            case SubDataset.TRANSFER_FUNCTION_TGTF:
            {
                View gtfLayout = getLayoutInflater().inflate(R.layout.gtf_layout, null);
                layout.addView(gtfLayout);


                //Configure the spinner color mode
                Spinner colorModeSpinner = (Spinner)gtfLayout.findViewById(R.id.colorModeSpinner);
                final GTFView gtfView = gtfLayout.findViewById(R.id.gtfView);

                GTFData gtfData = (GTFData)m_model.getCurrentSubDataset().getTransferFunction();
                gtfView.setModel(gtfData);

                colorModeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()
                {
                    @Override
                    public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l)
                    {
                        gtfView.getModel().setColorMode(i);
                    }

                    @Override
                    public void onNothingSelected(AdapterView<?> adapterView)
                    {
                        return;
                    }
                });

                //Configure the gaussian transfer function widget
                gtfView.setOnTouchListener(new View.OnTouchListener()
                {
                    @Override
                    public boolean onTouch(View view, MotionEvent motionEvent)
                    {
                        if(motionEvent.getAction() == MotionEvent.ACTION_UP)
                            m_drawerLayout.setDrawerLockMode(DrawerLayout.LOCK_MODE_UNLOCKED);
                        else if(motionEvent.getAction() == MotionEvent.ACTION_DOWN ||
                                motionEvent.getAction() == MotionEvent.ACTION_MOVE)
                            m_drawerLayout.setDrawerLockMode(DrawerLayout.LOCK_MODE_LOCKED_OPEN);
                        return false;
                    }
                });

                //Configure the checkbox about whether the GTF is a triangular one or not (enable gradient)
                CheckBox gtfEnableGradient = (CheckBox)gtfLayout.findViewById(R.id.enableGradientCheckBox);
                gtfEnableGradient.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener()
                   {
                       @Override
                       public void onCheckedChanged(CompoundButton compoundButton, boolean b)
                       {
                           //Update the GTF type (TGTF or GTF)
                           GTFData gtfData = (GTFData)m_model.getCurrentSubDataset().getTransferFunction();
                           gtfData.setGradient(b);
                       }
                   }
                );

                //Disable the possibility to change the checkbox if the user cannot modify the subdataset
                gtfEnableGradient.setOnTouchListener(new View.OnTouchListener()
                {
                    @Override
                    public boolean onTouch(View view, MotionEvent motionEvent)
                    {
                        if(m_model.getCurrentSubDataset().getTransferFunctionType() != SubDataset.TRANSFER_FUNCTION_GTF &&
                           m_model.getCurrentSubDataset().getTransferFunctionType() != SubDataset.TRANSFER_FUNCTION_TGTF)
                            return true;

                        GTFData gtf = (GTFData)m_model.getCurrentSubDataset().getTransferFunction();
                        if(gtf != null && gtf.getDataset() != null && gtf.getDataset().getCanBeModified())
                            return false;
                        return true;
                    }
                });
                m_currentTFView = (ViewGroup)gtfLayout;
                break;
            }
            case SubDataset.TRANSFER_FUNCTION_MERGE:
            {
                m_currentTFView = (ViewGroup)getLayoutInflater().inflate(R.layout.merging_tf_view, null);
                layout.addView(m_currentTFView);
                MergeTFData merge = (MergeTFData)m_model.getCurrentSubDataset().getTransferFunction();

                SeekBarHintView seekBar = m_currentTFView.findViewById(R.id.tSeekBar);
                seekBar.setMax(1000); //For doing some math. 1000 == 1.0
                seekBar.setProgress((int)(1000*merge.getInterpolationParameter()));

                seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
                {
                    @Override
                    public void onProgressChanged(SeekBar seekBar, int prog, boolean b)
                    {
                        if(m_model.getCurrentSubDataset().getTransferFunctionType() == SubDataset.TRANSFER_FUNCTION_MERGE)
                        {
                            MergeTFData merge = (MergeTFData) m_model.getCurrentSubDataset().getTransferFunction();
                            merge.setInterpolationParameter((float) (seekBar.getProgress()) / seekBar.getMax());
                        }
                    }

                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar){}

                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar){}
                });

                seekBar.setOnTouchListener(new View.OnTouchListener()
                {
                    @Override
                    public boolean onTouch(View view, MotionEvent motionEvent)
                    {
                        if(motionEvent.getAction() == MotionEvent.ACTION_UP)
                            m_drawerLayout.setDrawerLockMode(DrawerLayout.LOCK_MODE_UNLOCKED);
                        else if(motionEvent.getAction() == MotionEvent.ACTION_DOWN ||
                                motionEvent.getAction() == MotionEvent.ACTION_MOVE)
                            m_drawerLayout.setDrawerLockMode(DrawerLayout.LOCK_MODE_LOCKED_OPEN);

                        return !m_model.getCurrentSubDataset().getCanBeModified(); //Disable the scrolling if needed
                    }
                });
                break;
            }
        }

        m_currentTFSubDataset = m_model.getCurrentSubDataset();
        m_currentTFViewType = type;
    }

    private void updateGTFWidgets()
    {
        //If no subdataset
        if(m_model.getCurrentSubDataset() == null)
            return;

        if(m_model.getCurrentSubDataset().getTransferFunctionType() != SubDataset.TRANSFER_FUNCTION_GTF &&
           m_model.getCurrentSubDataset().getTransferFunctionType() != SubDataset.TRANSFER_FUNCTION_TGTF)
            return;

        GTFData        gtf               = (GTFData)m_model.getCurrentSubDataset().getTransferFunction();
        RangeColorView rangeColorView    = m_currentTFView.findViewById(R.id.colorRange);
        CheckBox       gtfEnableGradient = m_currentTFView.findViewById(R.id.enableGradientCheckBox);
        Spinner        colorModeSpinner  = m_currentTFView.findViewById(R.id.colorModeSpinner);

        if(gtf.getCPCPOrder().length == 1)
        {
            rangeColorView.setVisibility(View.VISIBLE);
            for(PointFieldDesc desc : gtf.getDataset().getParent().getPointFieldDescs())
            {
                if(desc.getID() == gtf.getCPCPOrder()[0])
                {
                    rangeColorView.getModel().setRawRange(desc.getMin(), desc.getMax(), false);
                    break;
                }
            }
        }
        else
            rangeColorView.getModel().setRawRange(0.0f, 1.0f, false);
        rangeColorView.getModel().setColorMode(gtf.getColorMode());

        boolean isTriangularGTF = m_model.getCurrentSubDataset().getTransferFunctionType() == SubDataset.TRANSFER_FUNCTION_TGTF;
        gtfEnableGradient.setChecked(isTriangularGTF);
        colorModeSpinner.setSelection(m_model.getCurrentSubDataset().getTransferFunction().getColorMode());
        redoGTFSizeLayout();
        redoGTFSizeRanges();
    }

    private void updateMergeTFWidgets()
    {
        //If no subdataset
        if(m_model.getCurrentSubDataset() == null)
            return;

        if(m_model.getCurrentSubDataset().getTransferFunctionType() != SubDataset.TRANSFER_FUNCTION_MERGE)
            return;

        MergeTFData     merge   = (MergeTFData)m_model.getCurrentSubDataset().getTransferFunction();
        SeekBarHintView seekBar = m_currentTFView.findViewById(R.id.tSeekBar);
        seekBar.setProgress((int)(seekBar.getMax()*merge.getInterpolationParameter()));
    }

    @Override
    public void onSetCurrentHeadset(SubDataset dataset, int headsetID)
    {}

    @Override
    public void onSetOwner(SubDataset dataset, int headsetID)
    {}

    @Override
    public void onSetCanBeModified(SubDataset dataset, boolean status)
    {}

    @Override
    public void onSetMapVisibility(SubDataset dataset, boolean visibility)
    {}

    @Override
    public void onSetVolumetricMask(SubDataset dataset)
    {}

    @Override
    public void onEmptyMessage(EmptyMessage msg)
    {
    }

    @Override
    public void onAddVTKDatasetMessage(AddVTKDatasetMessage msg)
    {
        //Parse the message information
        Log.i(TAG, "opening " + msg.getPath());
        File vtkFile = new File(new File(getExternalFilesDir(null), "Datas"), msg.getPath());
        VTKParser parser = new VTKParser(vtkFile);

        ArrayList<VTKFieldValue> ptValuesList = new ArrayList<>();
        ArrayList<VTKFieldValue> cellValuesList = new ArrayList<>();

        for(int i : msg.getPtFieldValueIndices())
            if(i < parser.getPointFieldValues().length)
                ptValuesList.add(parser.getPointFieldValues()[i]);

        for(int i : msg.getCellFieldValueIndices())
            if(i < parser.getCellFieldValues().length)
                cellValuesList.add(parser.getCellFieldValues()[i]);

        VTKFieldValue[] ptValues = new VTKFieldValue[ptValuesList.size()];
        ptValues = ptValuesList.toArray(ptValues);
        VTKFieldValue[] cellValues = new VTKFieldValue[cellValuesList.size()];
        cellValues = cellValuesList.toArray(cellValues);

        //Add into the model
        final VTKDataset dataset = new VTKDataset(parser, ptValues, cellValues, vtkFile.getName());
        dataset.setID(msg.getDataID());
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                m_model.addVTKDataset(dataset);
            }
        });
    }

    /** Get the SubDataset from its ID
     * @param dID the dataset ID
     * @param sdID the subdataset ID*/
    private SubDataset getSubDatasetFromID(int dID, int sdID)
    {
        Dataset dataset = null;
        for(Dataset d : m_model.getDatasets())
        {
            if(d.getID() == dID)
            {
                dataset = d;
                break;
            }
        }

        if(dataset == null)
            return null;
        return dataset.getSubDataset(sdID);
    }

    @Override
    public void onRotateDatasetMessage(final RotateDatasetMessage msg)
    {
        //Find the dataset
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
            SubDataset sd = getSubDatasetFromID(msg.getDatasetID(), msg.getSubDatasetID());

            if(sd != null)
            {
                //Remove and re add the listener for not ending in a while loop
                sd.removeListener(MainActivity.this);
                    sd.setRotation(msg.getRotation());
                sd.addListener(MainActivity.this);
            }
            }
        });
    }

    @Override
    public void onMoveDatasetMessage(final MoveDatasetMessage msg)
    {
        //Find the dataset
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                SubDataset sd = getSubDatasetFromID(msg.getDatasetID(), msg.getSubDatasetID());

                if(sd != null)
                {
                    //Remove and re add the listener for not ending in a while loop
                    sd.removeListener(MainActivity.this);
                        sd.setPosition(msg.getPosition());
                    sd.addListener(MainActivity.this);
                }
            }
        });
    }

    @Override
    public void onScaleDatasetMessage(final ScaleDatasetMessage msg)
    {
        //Find the dataset
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                SubDataset sd = getSubDatasetFromID(msg.getDatasetID(), msg.getSubDatasetID());

                if(sd != null)
                {
                    //Remove and re add the listener for not ending in a while loop
                    sd.removeListener(MainActivity.this);
                        sd.setScale(msg.getScale());
                    sd.addListener(MainActivity.this);
                }
            }
        });
    }

    @Override
    public void onTFDatasetMessage(final TFDatasetMessage msg)
    {
        runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                SubDataset sd = getSubDatasetFromID(msg.getDatasetID(), msg.getSubDatasetID());
                if(sd != null)
                {
                    //Remove and re add the listener for not ending in a while loop
                    sd.removeListener(MainActivity.this);
                        sd.setTransferFunction(tfMessageToTFObject(sd, msg));
                        redoTFWidget();
                    sd.addListener(MainActivity.this);
                }
            }
        });
    }

    /** Parse a TFDatasetMessage to an exploitable TransferFunction object
     * @param sd the SubDataset associated to this (in)complete message
     * @param msg the network message to convert
     * @return an exploitable TransferFunction, or null if an issue occured*/
    private TransferFunction tfMessageToTFObject(SubDataset sd, TFDatasetMessage msg)
    {
        TransferFunction tf = null;
        switch(msg.getTFType())
        {
            case SubDataset.TRANSFER_FUNCTION_GTF:
            case SubDataset.TRANSFER_FUNCTION_TGTF:
            {
                GTFData gtf = new GTFData(sd);
                gtf.setGradient(msg.getTFType() == SubDataset.TRANSFER_FUNCTION_TGTF);

                for(TFDatasetMessage.GTFData.PropData prop : msg.getGTFData().propData)
                {
                    if(!gtf.setRange(prop.propID, new GTFData.GTFPoint(prop.center, prop.scale)))
                        Log.e(MainActivity.TAG, "Could not set the GTF property " + prop.propID);
                }
                tf = gtf;
                break;
            }

            case SubDataset.TRANSFER_FUNCTION_MERGE:
            {
                TFDatasetMessage.MergeTFData merge = msg.getMergeTFData();
                MergeTFData mergeTF = new MergeTFData(sd, tfMessageToTFObject(sd, merge.tf1Msg), tfMessageToTFObject(sd, merge.tf2Msg));
                mergeTF.setInterpolationParameter(merge.t);

                tf=mergeTF;
                break;
            }
        }

        if(tf != null)
        {
            tf.setTimestep(msg.getTimestep());
            tf.setColorMode(msg.getColorMode());
        }
        return tf;
    }

    @Override
    public void onHeadsetBindingInfoMessage(final HeadsetBindingInfoMessage msg)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                m_model.setBindingInfo(msg);
                if(msg.getHeadsetID() == -1)
                    m_model.endPendingAnnotation(true);

                //Send tablet's interactive data
                onSetLocation(m_model, m_model.getTabletPosition(), m_model.getTabletRotation());
                onSetTabletScale(m_model, m_model.getTabletScale(), m_model.getTabletWidth(), m_model.getTabletHeight(), m_model.getTabletX(), m_model.getTabletY());
            }
        });
    }

    @Override
    public void onSubDatasetLockOwnerMessage(final SubDatasetLockOwnerMessage msg)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                SubDataset sd = getSubDatasetFromID(msg.getDatasetID(), msg.getSubDatasetID());

                if(sd != null)
                {
                    sd.setCurrentHeadset(msg.getHeadsetID());
                }
            }
        });
    }

    @Override
    public void onSubDatasetOwnerMessage(final SubDatasetOwnerMessage msg)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                SubDataset sd = getSubDatasetFromID(msg.getDatasetID(), msg.getSubDatasetID());

                if(sd != null)
                {
                    sd.setOwnerID(msg.getHeadsetID());
                }
            }
        });
    }

    @Override
    public void onLocationTabletMessage(final LocationTabletMessage msg)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(m_model.getCurrentTangibleMode() != ApplicationModel.TANGIBLE_MODE_NONE)
                    m_model.setLocation(msg.getPosition(), msg.getRotation());
            }
        });
    }

    @Override
    public void onAddCloudPointDatasetMessage(AddCloudPointDatasetMessage msg)
    {
        //Parse the message information
        Log.i(TAG, "opening " + msg.getPath());
        File file = new File(new File(getExternalFilesDir(null), "Datas"), msg.getPath());

        //Add into the model
        final CloudPointDataset dataset = new CloudPointDataset(file);
        dataset.setID(msg.getDataID());
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                m_model.addCloudPointDataset(dataset);
            }
        });
    }

    @Override
    public void onToggleMapVisibilityMessage(final ToggleMapVisibilityMessage msg)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                SubDataset sd = getSubDatasetFromID(msg.getDatasetID(), msg.getSubDatasetID());
                if(sd != null)
                    sd.setMapVisibility(msg.getVisibility());
            }
        });
    }

    @Override
    public void onResetVolumetricSelectionMessage(final ResetVolumetricSelectionMessage msg)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run(){
                SubDataset sd = getSubDatasetFromID(msg.getDatasetID(), msg.getSubDatasetID());
                if (sd != null)
                    sd.resetVolumetricMask();
            }
        });
    }

    public void onSubDatasetVolumetricMaskMessage(final SubDatasetVolumetricMaskMessage msg)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run(){
                SubDataset sd = getSubDatasetFromID(msg.getDatasetID(), msg.getSubDatasetID());
                if (sd != null)
                {
                    sd.setVolumetricMask(msg.getMask());
                    sd.enableVolumetricMask(msg.isEnabled());
                }
            }
        });
    }

    @Override
    public void onNextTBTrialMessage(NextTBTrialMessage msg)
    {
        if(msg.getTangibleMode() == ApplicationModel.TANGIBLE_BRUSH_STUDY_3D)
        {
            m_model.setCurrentSelectionMode(ApplicationModel.SELECTION_MODE_RELATIVE_ALIGNED);
            m_model.setCurrentTBUserStudyMode(ApplicationModel.TANGIBLE_BRUSH_STUDY_3D);
        }
        else
        {
            m_model.setCurrentSelectionMode(ApplicationModel.SELECTION_MODE_RELATIVE_FULL);
            m_model.setCurrentTBUserStudyMode(ApplicationModel.TANGIBLE_BRUSH_STUDY_ORIGINAL);
        }

        Quaternion rot = Quaternion.lookAt(new float[]{-0.33f, 0.33f, -0.33f}, new float[]{0.0f, 0.0f, 0.0f}).multiplyBy(new Quaternion(new float[]{1.0f, 0.0f, 0.0f}, -(float)Math.PI/2.0f));
        m_model.setInternalTabletPositionAndRotation(new float[]{-0.33f, 0.33f, -0.33f}, rot.toFloatArray());
	    m_model.setCurrentTBTrial(msg.getTrialID());
        m_model.startTBTrial();
    }

    @Override
    public void onHeadsetsStatusMessage(final HeadsetsStatusMessage msg)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                m_model.setHeadsetsStatus(msg.getStatus());
            }
        });
    }

    @Override
    public void onAnchorAnnotation(final AnchorAnnotationMessage msg)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                SubDataset sd = getSubDatasetFromID(msg.getDatasetID(), msg.getSubDatasetID());

                AnnotationData data = new AnnotationData(320, 160);
                ApplicationModel.AnnotationMetaData annotMetaData = new ApplicationModel.AnnotationMetaData(sd, -1);
                m_model.addAnnotation(data, annotMetaData);

                if(msg.getHeadsetID() == m_model.getBindingInfo().getHeadsetID())
                {
                    m_model.endPendingAnnotation(false);
                }
            }
        });
    }

    @Override
    public void onClearAnnotations(final ClearAnnotationsMessage msg)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                SubDataset sd = getSubDatasetFromID(msg.getDatasetID(), msg.getSubDatasetID());

                if(sd == null)
                    return;

                while(sd.getAnnotations().size() > 0)
                    sd.removeAnnotation(sd.getAnnotations().get(sd.getAnnotations().size()-1));
            }
        });
    }

    @Override
    public void onAddSubDataset(final AddSubDatasetMessage msg)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                for(Dataset d : m_model.getDatasets())
                {
                    if(d.getID() == msg.getDatasetID())
                    {
                        SubDataset sd = SubDataset.createNewSubDataset(d, msg.getSubDatasetID(), msg.getSubDatasetName(), msg.getOwnerID());
                        d.addSubDataset(sd, false);

                        if(m_model.getCurrentSubDataset() == null)
                            m_model.setCurrentSubDataset(sd);
                        break;
                    }
                }
            }
        });
    }

    @Override
    public void onRemoveSubDataset(final RemoveSubDatasetMessage msg)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                SubDataset sd = getSubDatasetFromID(msg.getDatasetID(), msg.getSubDatasetID());
                sd.getParent().removeSubDataset(sd);
            }
        });
    }

    @Override
    public void onChangeCurrentSubDataset(ApplicationModel model, SubDataset sd)
    {
        redoTFWidget();
    }

    /** Redo the GTF layout. More specifically, redo all the widgets handling the "size" components of GTF*/
    private void redoGTFSizeLayout()
    {
        if(m_model.getCurrentSubDataset().getTransferFunctionType() != SubDataset.TRANSFER_FUNCTION_GTF &&
           m_model.getCurrentSubDataset().getTransferFunctionType() != SubDataset.TRANSFER_FUNCTION_TGTF)
            return;

        final GTFData gtf = (GTFData)m_model.getCurrentSubDataset().getTransferFunction();

        //Check if needed to redo the layout
        boolean redo = false;
        final SubDataset sd = gtf.getDataset();
        if(sd == null || m_gtfSizeViews.size() != gtf.getCPCPOrder().length)
            redo = true;
        else
        {
            for(final int i : gtf.getCPCPOrder())
            {
                if(redo)
                    break;
                for (PointFieldDesc desc : sd.getParent().getPointFieldDescs())
                {
                    if (i == desc.getID())
                    {
                        if (!m_gtfSizeViews.containsKey(i))
                        {
                            redo = true;
                            break;
                        }
                    }
                }
            }
        }
        if(!redo)
            return;

        //Remove all the views
        for(View v : m_gtfSizeViews.values())
        {
            ((ViewGroup)v.getParent()).removeView(v);
            v.setVisibility(View.GONE);
        }
        m_gtfSizeViews.clear();

        //If no current GTF: exit
        if(gtf == null)
            return;
        if(sd == null)
            return;

        //Recreate the layout. One layout per GTF component
        ViewGroup sizeLayout = m_currentTFView.findViewById(R.id.gtfSizeLayout);
        for(final int i : gtf.getCPCPOrder())
        {
            for(PointFieldDesc desc : sd.getParent().getPointFieldDescs())
            {
                if (i == desc.getID())
                {
                    View layout = getLayoutInflater().inflate(R.layout.gtf_size_prop_view, null);
                    sizeLayout.addView(layout);

                    //Label
                    TextView label = layout.findViewById(R.id.gtfLabel);
                    label.setText(desc.getName());

                    //Slider
                    SeekBarHintView seekBar = layout.findViewById(R.id.gtfSeekBar);
                    seekBar.setMax(1000); //For doing some math. 1000 == 1.0
                    seekBar.setProgress(1000*(int)gtf.getRanges().get(i).scale);

                    seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
                    {
                        @Override
                        public void onProgressChanged(SeekBar seekBar, int prog, boolean b)
                        {
                            GTFData.GTFPoint gtfPoint = (GTFData.GTFPoint)gtf.getRanges().get(i).clone();
                            gtfPoint.scale = (float)seekBar.getProgress()/seekBar.getMax();
                            if(gtf != null)
                                gtf.setRange(i, gtfPoint);
                        }

                        @Override
                        public void onStartTrackingTouch(SeekBar seekBar){}

                        @Override
                        public void onStopTrackingTouch(SeekBar seekBar){}
                    });

                    seekBar.setOnTouchListener(new View.OnTouchListener()
                    {
                        @Override
                        public boolean onTouch(View view, MotionEvent motionEvent)
                        {
                            if(motionEvent.getAction() == MotionEvent.ACTION_UP)
                                m_drawerLayout.setDrawerLockMode(DrawerLayout.LOCK_MODE_UNLOCKED);
                            else if(motionEvent.getAction() == MotionEvent.ACTION_DOWN ||
                                    motionEvent.getAction() == MotionEvent.ACTION_MOVE)
                                m_drawerLayout.setDrawerLockMode(DrawerLayout.LOCK_MODE_LOCKED_OPEN);

                            return !sd.getCanBeModified(); //Disable the scrolling if needed
                        }
                    });

                    m_gtfSizeViews.put(i, layout);
                    break;
                }
            }
        }
    }

    @Override
    public void onUpdateHeadsetsStatus(ApplicationModel model, HeadsetsStatusMessage.HeadsetStatus[] headsetsStatus)
    {}

    @Override
    public void onUpdateBindingInformation(ApplicationModel model, HeadsetBindingInfoMessage info)
    {}

    @Override
    public void onRemoveDataset(ApplicationModel model, Dataset dataset) {}

    @Override
    public void onUpdatePointingTechnique(ApplicationModel model, int pt)
    {
        if(m_menu == null)
            return;

        MenuItem[] itms = new MenuItem[4];
        itms[ApplicationModel.POINTING_MANUAL] = m_menu.findItem(R.id.manualPointing_item);
        itms[ApplicationModel.POINTING_WIM] = m_menu.findItem(R.id.wimPointing_item);
        itms[ApplicationModel.POINTING_WIM_POINTER] = m_menu.findItem(R.id.wimRayPointing_item);
        itms[ApplicationModel.POINTING_GOGO] = m_menu.findItem(R.id.gogoPointing_item);

        //pt can be "-1"
        if(pt >= 0 && pt <= 3)
            itms[pt].setChecked(true);
    }

    @Override
    public void onChangeTimeAnimationStatus(ApplicationModel model, boolean isInPlay, int speed, float step)
    {}

    @Override
    public void onSetLocation(ApplicationModel model, float[] pos, float[] rot)
    {
        m_socket.push(SocketManager.createLocationEvent(pos, rot));
    }

    @Override
    public void onSetTabletScale(ApplicationModel model, float scale, float width, float height, float posx, float posy) {
        m_socket.push(SocketManager.createTabletScaleEvent(scale, width, height, posx, posy));
    }

    @Override
    public void onSetLasso(ApplicationModel model, float[] lasso) {
        m_socket.push(SocketManager.createLassoEvent(lasso));
    }

    @Override
    public void onConfirmSelection(ApplicationModel model) {
        m_socket.push(SocketManager.createConfirmSelectionEvent(getDatasetIDBinding(model.getCurrentSubDataset())));
        m_model.setIsInSelection(false);
    }

    @Override
    public void onSetCurrentBooleanOperation(ApplicationModel model, int op)
    {}

    @Override
    public void onSetTangibleMode(ApplicationModel model, int tangibleMode)
    {
        if(m_model.getCurrentAction() == ApplicationModel.CURRENT_ACTION_SELECTING)
        {
            if(tangibleMode == ApplicationModel.TANGIBLE_MODE_MOVE)
            {
                m_socket.push(SocketManager.createAddNewSelectionInputEvent(m_model.getCurrentBooleanOperation(), m_model.isVolumeSelectionConstrained()));
                m_model.setIsInSelection(true);
            }
            else
                m_socket.push(SocketManager.createAddNewSelectionInputEvent(ApplicationModel.BOOLEAN_NONE, m_model.isVolumeSelectionConstrained())); //Specify that we are not in an operation
        }

        //if(m_model.getCurrentAction() != ApplicationModel.CURRENT_ACTION_REVIEWING_SELECTION)
        //    m_model.setPostReviewRotation(new Quaternion());
    }

    @Override
    public void onStopCapturingTangible(ApplicationModel model, boolean stop) {}

    @Override
    public void onSetSelectionMode(ApplicationModel model, int selectMode)
    {
        if(m_menu == null)
            return;

        MenuItem[] itms = new MenuItem[3];
        itms[ApplicationModel.SELECTION_MODE_ABSOLUTE]         = m_menu.findItem(R.id.absoluteSelectionMode_item);
        itms[ApplicationModel.SELECTION_MODE_RELATIVE_ALIGNED] = m_menu.findItem(R.id.relativeSelectionModeAligned_item);
        itms[ApplicationModel.SELECTION_MODE_RELATIVE_FULL]    = m_menu.findItem(R.id.relativeSelectionModeFull_item);

        //selectMode can be "-1"
        if(selectMode >= 0 && selectMode <= 2)
            itms[selectMode].setChecked(true);
    }

    @Override
    public void onSetTBUserStudyMode(ApplicationModel model, int tbMode)
    {}

    @Override
    public void onEndTBTrial(ApplicationModel model)
    {
        m_socket.push(SocketManager.createEndTBTask());
    }

    @Override
    public void onStartNextTrial(ApplicationModel model)
    {}

    @Override
    public void onSetPostReviewRotation(ApplicationModel model, Quaternion rot)
    {
        DatasetIDBinding idBinding = null;
        if(m_model.getCurrentAction() == ApplicationModel.CURRENT_ACTION_REVIEWING_SELECTION ||
           m_model.getCurrentAction() == ApplicationModel.CURRENT_ACTION_LASSO ||
           m_model.getCurrentAction() == ApplicationModel.CURRENT_ACTION_SELECTING)
        {
            idBinding = getDatasetIDBinding(model.getCurrentSubDataset());
            if (idBinding.dataset == null)
                idBinding = null;
        }

        m_socket.push(SocketManager.createPostReviewRotationEvent(idBinding, rot.toFloatArray()));
    }

    @Override
    public void onEnableSwipping(Fragment fragment)
    {
        m_viewPager.setPagingEnabled(true);
    }

    @Override
    public void onDisableSwipping(Fragment fragment)
    {
        m_viewPager.setPagingEnabled(false);
    }

    private void sendAnnotationToServer(AnnotationData annotation)
    {
        ApplicationModel.AnnotationMetaData metaData = m_model.getAnnotations().get(annotation);

        if(!m_model.canModifySubDataset(metaData.getSubDataset())) //forget about it
            return;

        DatasetIDBinding idBinding = getDatasetIDBinding(metaData.getSubDataset());
        if(idBinding.subDatasetID != -1 && idBinding.dataset != null && idBinding.dataset.getID() >= 0)
            m_socket.push(SocketManager.createAnnotationEvent(idBinding, annotation, m_model.getAnnotations().get(annotation)));
    }

    @Override
    public void onAddStroke(AnnotationData data, AnnotationStroke stroke)
    {
        sendAnnotationToServer(data);
    }

    @Override
    public void onAddText(AnnotationData data, AnnotationText text)
    {
        sendAnnotationToServer(data);
    }

    @Override
    public void onAddImage(AnnotationData data)
    {
        sendAnnotationToServer(data);
    }

    @Override
    public void onSetMode(AnnotationData data, AnnotationData.AnnotationMode mode)
    {}

    @Override
    public void onRequestFullScreen(DatasetsFragment frag, boolean inFullScreen)
    {
        TabLayout tabs = (TabLayout)findViewById(R.id.tabs);
        if(inFullScreen)
        {
            getSupportActionBar().hide();
            tabs.setVisibility(View.GONE);
        }
        else
        {
            getSupportActionBar().show();
            tabs.setVisibility(View.VISIBLE);
        }
    }

    @Override
    public void onRenameSubDataset(DatasetsFragment frag, SubDataset sd, String name)
    {

    }

    @Override
    public void onDuplicateSubDataset(DatasetsFragment frag, SubDataset sd)
    {
        m_socket.push(SocketManager.createDuplicateSubDatasetEvent(getDatasetIDBinding(sd)));
    }

    @Override
    public void onRequestRemoveSubDataset(DatasetsFragment frag, SubDataset sd)
    {
        m_socket.push(SocketManager.createRemoveSubDatasetEvent(getDatasetIDBinding(sd)));
    }

    @Override
    public void onRequestAddSubDataset(DatasetsFragment frag, Dataset d, boolean publicSD)
    {
        m_socket.push(SocketManager.createAddSubDatasetEvent(d.getID(), publicSD));
    }

    @Override
    public void onRequestMakeSubDatasetPublic(DatasetsFragment frag, SubDataset sd)
    {
        m_socket.push(SocketManager.createMakeSubDatasetPublicEvent(getDatasetIDBinding(sd)));
    }

    @Override
    public void onRequestChangeSubDatasetMapVisibility(DatasetsFragment frag, SubDataset sd, boolean visibility)
    {
        m_socket.push(SocketManager.createToggleMapVisibility(getDatasetIDBinding(sd), visibility));
    }

    @Override
    public void onMergeSubDatasets(DatasetsFragment frag, SubDataset sd1, SubDataset sd2)
    {
        m_socket.push(SocketManager.createMergeSubDatasetsEvent(sd1, sd2));
    }

    @Override
    public void onResetVolumetricSelection(DatasetsFragment frag, SubDataset sd)
    {
        m_socket.push(SocketManager.createResetVolumetricSelection(getDatasetIDBinding(sd)));
    }


    public void redoGTFSizeRanges()
    {
        if(m_model.getCurrentSubDataset().getTransferFunctionType() != SubDataset.TRANSFER_FUNCTION_GTF &&
                m_model.getCurrentSubDataset().getTransferFunctionType() != SubDataset.TRANSFER_FUNCTION_TGTF)
            return;

        final GTFData gtf = (GTFData)m_model.getCurrentSubDataset().getTransferFunction();

        //Update every points
        for(Map.Entry<Integer, GTFData.GTFPoint> value : gtf.getRanges().entrySet())
        {
            if(m_gtfSizeViews.containsKey(value.getKey()))
            {
                SeekBar seekBar = m_gtfSizeViews.get(value.getKey()).findViewById(R.id.gtfSeekBar);
                seekBar.setProgress((int)(seekBar.getMax()*value.getValue().scale));

                for(PointFieldDesc desc : gtf.getDataset().getParent().getPointFieldDescs())
                {
                    if (value.getKey() == desc.getID())
                    {
                        TextView textView = m_gtfSizeViews.get(value.getKey()).findViewById(R.id.gtfLabel);
                        textView.setText(desc.getName());
                        break;
                    }
                }
            }
        }
    }

    @Override
    public void onDisconnection(SocketManager socket)
    {
        runOnUiThread(
                new Runnable() {
                    @Override
                    public void run() {
                        m_model.setBindingInfo(null);
                        m_model.setHeadsetsStatus(null);

                        //Clean every
                        while(m_model.getDatasets().size() > 0)
                            m_model.removeDataset(m_model.getDatasets().get(0));
                    }
                }
        );
    }

    /** Set up the main layout*/
    private void setUpMainLayout()
    {
        m_viewPager = (VFVViewPager)findViewById(R.id.viewpager);
        ViewPagerAdapter adapter = new ViewPagerAdapter(getSupportFragmentManager());

        //Add "Datasets" tab
        m_dataFragment = new DatasetsFragment();
        m_dataFragment.addDFListener(this);
        adapter.addFragment(m_dataFragment, "Datasets");

        //Add "Annotations" tab
        final  AnnotationsFragment annotationsFragment = new AnnotationsFragment();
        adapter.addFragment(annotationsFragment, "Annotations");

        m_dataFragment.addListener((VFVFragment.IFragmentListener)this);
        annotationsFragment.addListener(this);

        m_viewPager.setAdapter(adapter);
        TabLayout tabLayout = (TabLayout)findViewById(R.id.tabs);
        tabLayout.setupWithViewPager(m_viewPager);

        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                m_dataFragment.setUpModel(m_model);
                annotationsFragment.setUpModel(m_model);
            }
        });

        m_viewPager.setOnPageChangeListener(new ViewPager.OnPageChangeListener() {
            @Override
            public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) {}

            @Override
            public void onPageSelected(final int position)
            {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if(position == 0)
                            m_dataFragment.setVisibility(true);
                        else
                            m_dataFragment.setVisibility(false);
                    }
                });
            }

            @Override
            public void onPageScrollStateChanged(int state) {}
        });
    }

    /** \brief Set up the drawer layout (root layout)*/
    private void setUpDrawerLayout()
    {
        m_drawerLayout  = (DrawerLayout)findViewById(R.id.rootLayout);
    }

    /** \brief Setup the toolbar */
    private void setUpToolbar()
    {
        //Set the support of toolbar
        Toolbar toolbar = (Toolbar)findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        //Add an home button
        ActionBar actionbar = getSupportActionBar();
        actionbar.setDisplayHomeAsUpEnabled(true);
    }

    /** \brief Setup the hidden menu (left menu) part of the drawer */
    private void setUpHiddenMenu()
    {
        m_deleteDataBtn  = (Button)findViewById(R.id.deleteDataBtn);
        Button addButton = (Button)findViewById(R.id.addNewDataBtn);
        addButton.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view){openNewDataDialog();}
        });
    }

    /** \brief Dialog about opening a new dataset */
    private void openNewDataDialog()
    {
        OpenDatasetDialogFragment dialogFragment = new OpenDatasetDialogFragment();
        dialogFragment.setNoticeDialogListener(new INoticeDialogListener()
        {
            @Override
            public void onDialogPositiveClick(DialogFragment dialogFrag, View view)
            {
                Spinner dataSpinner = (Spinner)view.findViewById(R.id.openDatasetSpinner);
                final DataFile df   = (DataFile)dataSpinner.getSelectedItem();

                String fileName = df.getFile().getName();

                //VectorField dataset
                if(fileName.endsWith(".vf"))
                {
                    /*final VectorFieldDataset fd = new VectorFieldDataset(df.getFile());
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            m_model.addVectorFieldDataset(fd);
                        }
                    });*/
                }

                //Cloud Point
                else if(fileName.endsWith(".cp"))
                {
                    runOnUiThread(new Runnable()
                    {
                        @Override
                        public void run()
                        {
                            m_socket.push(SocketManager.createAddCloudPointDatasetEvent(df.getFile().getName()));
                        }
                    });
                }

                //VTK dataset
                else if(fileName.endsWith(".vtk"))
                {
                    VTKParser            parser    = new VTKParser(df.getFile());
                    OpenVTKDatasetDialog vtkDialog = new OpenVTKDatasetDialog(MainActivity.this, parser);
                    vtkDialog.open(new INoticeVTKDialogListener()
                    {
                        @Override
                        public void onDialogPositiveClick(OpenVTKDatasetDialog dialog)
                        {
                            //Get under Array shape the pt and cells field values desired
                            ArrayList<VTKFieldValue> ptValuesList = dialog.getSelectedPtFieldValues();
                            ArrayList<VTKFieldValue> cellValuesList = dialog.getSelectedCellFieldValues();

                            if(cellValuesList.size() + ptValuesList.size() > 0)
                            {
                                VTKFieldValue[] ptValues = new VTKFieldValue[ptValuesList.size()];
                                ptValues = ptValuesList.toArray(ptValues);
                                VTKFieldValue[] cellValues = new VTKFieldValue[cellValuesList.size()];
                                cellValues = cellValuesList.toArray(cellValues);

                                final VTKDataset dataset = new VTKDataset(dialog.getVTKParser(), ptValues, cellValues, df.getFile().getName());
                                m_socket.push(SocketManager.createAddVTKDatasetEvent(dataset));
                                try
                                {
                                    dataset.finalize();
                                }
                                catch(Throwable e) {}
                            }
                        }

                        @Override
                        public void onDialogNegativeClick(OpenVTKDatasetDialog dialog) {}
                    });
                }
            }

            @Override
            public void onDialogNegativeClick(DialogFragment dialogFrag, View view)
            {

            }
        });
        dialogFragment.show(getFragmentManager(), "dialog");
    }

    /** Function called for gathering common actions when adding a new Dataset
     * @param d the Dataset added*/
    private void onAddDataset(final Dataset d)
    {
        d.addListener(this);
    }

    static
    {
        System.loadLibrary("native-lib");
    }
}
