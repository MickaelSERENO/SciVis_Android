package com.sereno.vfv;

import android.app.DialogFragment;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.provider.ContactsContract;
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
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;

import com.sereno.VFVViewPager;
import com.sereno.color.ColorMode;
import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.DataFile;
import com.sereno.vfv.Data.BinaryDataset;
import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Data.VTKDataset;
import com.sereno.vfv.Data.VTKFieldValue;
import com.sereno.vfv.Data.VTKParser;
import com.sereno.vfv.Listener.INoticeDialogListener;
import com.sereno.vfv.Listener.INotiveVTKDialogListener;
import com.sereno.vfv.Network.AcknowledgeAddDatasetMessage;
import com.sereno.vfv.Network.AddVTKDatasetMessage;
import com.sereno.vfv.Network.EmptyMessage;
import com.sereno.vfv.Network.HeadsetBindingInfoMessage;
import com.sereno.vfv.Network.HeadsetsStatusMessage;
import com.sereno.vfv.Network.MessageBuffer;
import com.sereno.vfv.Network.MoveDatasetMessage;
import com.sereno.vfv.Network.RotateDatasetMessage;
import com.sereno.vfv.Network.ScaleDatasetMessage;
import com.sereno.vfv.Network.SocketManager;
import com.sereno.vfv.Network.SubDatasetOwnerMessage;
import com.sereno.view.AnnotationData;
import com.sereno.view.AnnotationStroke;
import com.sereno.view.AnnotationText;
import com.sereno.view.RangeColorData;
import com.sereno.view.RangeColorView;

import java.io.File;
import java.net.Socket;
import java.util.ArrayDeque;
import java.util.ArrayList;

/* \brief The MainActivity. First Activity to be launched*/
public class MainActivity extends AppCompatActivity
                          implements ApplicationModel.IDataCallback, SubDataset.ISubDatasetListener, MessageBuffer.IMessageBufferCallback,
                                     VFVFragment.IFragmentListener, AnnotationData.IAnnotationDataListener, SocketManager.ISocketManagerListener
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

    private ApplicationModel m_model;             /*!< The application data model */
    private DrawerLayout     m_drawerLayout;      /*!< The root layout. DrawerLayout permit to have a left menu*/
    private Button           m_deleteDataBtn;     /*!< The delete data button*/
    private RangeColorView   m_rangeColorView;    /*!< The range color view*/
    private SocketManager    m_socket;            /*!< Connection with the server application*/
    private ArrayDeque<Dataset> m_pendingDataset = new ArrayDeque<>(); /*!< The Dataset pending to be updated by the Server*/
    private VFVViewPager     m_viewPager;
    private DatasetsFragment m_dataFragment = null;

    /** @brief OnCreate function. Called when the activity is on creation*/
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        m_model = new ApplicationModel(this);
        m_model.addListener(this);

        setContentView(R.layout.main_activity);

        //Set up all internal components
        setUpMainLayout();
        setUpDrawerLayout();
        setUpToolbar();
        setUpHiddenMenu();

        m_socket = new SocketManager(m_model.getConfiguration().getServerIP(), m_model.getConfiguration().getServerPort());
        m_socket.getMessageBuffer().addListener(this);
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
                        m_socket.setHololensIP(txt.getText().toString());
                    }

                    @Override
                    public void onDialogNegativeClick(DialogFragment dialogFrag, View view)
                    {}
                });
                dialogFragment.show(getFragmentManager(), "dialog");
                return true;
            }
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
            for(VTKDataset d : m_model.getVTKDatasets())
            {
                if(parent != null)
                    break;
                for(int i = 0; i < d.getNbSubDataset(); i++)
                {
                    if(d.getSubDataset(i).getNativePtr() == sd.getNativePtr())
                    {
                        parent       = d;
                        subDatasetID = i;
                    }
                }
            }

            if(parent == null)
            {
                for(BinaryDataset d : m_model.getBinaryDatasets())
                {
                    if(parent != null)
                        break;

                    for(int i = 0; i < d.getNbSubDataset(); i++)
                    {
                        if(d.getSubDataset(i).getNativePtr() == sd.getNativePtr())
                        {
                            parent       = d;
                            subDatasetID = i;
                            break;
                        }
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
        return true;
    }

    @Override
    public void onAddBinaryDataset(ApplicationModel model, BinaryDataset d)
    {
        m_deleteDataBtn.setVisibility(View.VISIBLE);
        onAddDataset(d);
    }

    @Override
    public void onAddVTKDataset(ApplicationModel model, VTKDataset d)
    {
        if(d.getID() < 0)
            m_socket.push(SocketManager.createAddVTKDatasetEvent(d));
        onAddDataset(d);
    }

    @Override
    public void onAddAnnotation(ApplicationModel model, AnnotationData annot, ApplicationModel.AnnotationMetaData metaData)
    {
        annot.addListener(this);
    }

    @Override
    public void onChangeCurrentAction(ApplicationModel model, int action) {
        m_socket.push(SocketManager.createCurrentActionEvent(action));
    }

    @Override
    public void onRangeColorChange(SubDataset sd, float min, float max, int mode)
    {}

    @Override
    public void onRotationEvent(SubDataset dataset, float[] quaternion)
    {
        DatasetIDBinding idBinding = getDatasetIDBinding(dataset);

        //If everything is correct, send the rotation event
        if(idBinding.subDatasetID != -1 && idBinding.dataset != null && idBinding.dataset.getID() >= 0)
            m_socket.push(SocketManager.createRotationEvent(idBinding, dataset.getRotation()));
    }

    @Override
    public void onPositionEvent(SubDataset dataset, float[] position)
    {
        DatasetIDBinding idBinding = getDatasetIDBinding(dataset);

        //If everything is correct, send the rotation event
        if(idBinding.subDatasetID != -1 && idBinding.dataset != null && idBinding.dataset.getID() >= 0)
            m_socket.push(SocketManager.createPositionEvent(idBinding, dataset.getPosition()));
    }

    @Override
    public void onScaleEvent(SubDataset dataset, float[] scale)
    {
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
    public void onEmptyMessage(EmptyMessage msg)
    {}

    @Override
    public void onAcknowledgeAddDatasetMessage(final AcknowledgeAddDatasetMessage msg)
    {
        switch(msg.getType())
        {
            case MessageBuffer.GET_ADD_DATASET_ACKNOWLEDGE:
            {
                Dataset d = m_pendingDataset.poll();
                d.setID(msg.getID());
                break;
            }
            default:
                break;
        }
    }

    @Override
    public void onAddVTKDatasetMessage(AddVTKDatasetMessage msg)
    {
        //Parse the message information
        File vtkFile = new File(new File(getExternalFilesDir(null), "Datas"), msg.getPath());
        VTKParser parser = new VTKParser(vtkFile);

        ArrayList<VTKFieldValue> ptValuesList = new ArrayList<>();
        ArrayList<VTKFieldValue> cellValuesList = new ArrayList<>();

        for(int i : msg.getPtFieldValueIndices())
            ptValuesList.add(parser.getPointFieldValues()[i]);

        for(int i : msg.getCellFieldValueIndices())
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
        for(BinaryDataset d : m_model.getBinaryDatasets())
        {
            if(d.getID() == dID)
            {
                dataset = d;
                break;
            }
        }

        if(dataset == null)
            for(VTKDataset d : m_model.getVTKDatasets())
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
    public void onHeadsetBindingInfoMessage(final HeadsetBindingInfoMessage msg)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                m_model.setBindingInfo(msg);
            }
        });
    }

    @Override
    public void onSubDatasetOwnerMessage(SubDatasetOwnerMessage msg)
    {
        //TODO
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
    public void onChangeCurrentSubDataset(ApplicationModel model, SubDataset sd)
    {}

    @Override
    public void onUpdateHeadsetsStatus(ApplicationModel model, HeadsetsStatusMessage.HeadsetStatus[] headsetsStatus)
    {}

    @Override
    public void onUpdateBindingInformation(ApplicationModel model, HeadsetBindingInfoMessage info) {}

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
    public void onReconnection(SocketManager socket)
    {
        socket.push(SocketManager.createCurrentActionEvent(m_model.getCurrentAction()));
        socket.push(SocketManager.createCurrentSubDatasetEvent(getDatasetIDBinding(m_model.getCurrentSubDataset())));
    }

    /** Set up the main layout*/
    private void setUpMainLayout()
    {
        m_viewPager = (VFVViewPager)findViewById(R.id.viewpager);
        ViewPagerAdapter adapter = new ViewPagerAdapter(getSupportFragmentManager());

        //Add "Datasets" tab
        m_dataFragment = new DatasetsFragment();
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

        getFragmentManager().beginTransaction().commit();
    }

    /** \brief Set up the drawer layout (root layout)*/
    private void setUpDrawerLayout()
    {
        m_drawerLayout  = (DrawerLayout)findViewById(R.id.rootLayout);

        m_rangeColorView = (RangeColorView)findViewById(R.id.rangeColorView);
        m_model.setRangeColorModel(m_rangeColorView.getModel());


        //Configure the spinner color mode
        final Spinner colorModeSpinner = (Spinner)findViewById(R.id.colorModeSpinner);

        m_rangeColorView.getModel().addOnRangeChangeListener(new RangeColorData.IOnRangeChangeListener()
        {
            @Override
            public void onRangeChange(RangeColorData data, float minVal, float maxVal, int mode)
            {
                colorModeSpinner.setSelection(mode);
            }
        });
        colorModeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()
        {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l)
            {
                m_rangeColorView.getModel().setColorMode(i);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView)
            {
                return;
            }
        });

        m_rangeColorView.setOnTouchListener(new View.OnTouchListener()
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

                //Binary dataset
                if(fileName.endsWith(".data"))
                {
                    final BinaryDataset fd = new BinaryDataset(df.getFile());
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            m_model.addBinaryDataset(fd);
                        }
                    });
                }

                //VTK dataset
                else if(fileName.endsWith(".vtk"))
                {
                    VTKParser            parser    = new VTKParser(df.getFile());
                    OpenVTKDatasetDialog vtkDialog = new OpenVTKDatasetDialog(MainActivity.this, parser);
                    vtkDialog.open(new INotiveVTKDialogListener()
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
                                //Add into the model
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        m_model.addVTKDataset(dataset);
                                    }
                                });
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
        m_pendingDataset.push(d);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                m_rangeColorView.getModel().setRange(0.0f, 1.0f);
                m_rangeColorView.getModel().setColorMode(ColorMode.RAINBOW);
            }
        });
    }

    static
    {
        System.loadLibrary("native-lib");
    }
}