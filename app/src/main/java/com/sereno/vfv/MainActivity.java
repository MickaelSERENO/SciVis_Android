package com.sereno.vfv;

import android.app.DialogFragment;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.provider.ContactsContract;
import android.support.design.widget.TabLayout;
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
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;

import com.sereno.Tree;
import com.sereno.color.ColorMode;
import com.sereno.gl.VFVSurfaceView;
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
import com.sereno.vfv.Network.MessageBuffer;
import com.sereno.vfv.Network.MoveDatasetMessage;
import com.sereno.vfv.Network.RotateDatasetMessage;
import com.sereno.vfv.Network.SocketManager;
import com.sereno.view.AnnotationData;
import com.sereno.view.RangeColorView;
import com.sereno.view.TreeView;

import org.w3c.dom.Text;

import java.io.File;
import java.util.ArrayDeque;
import java.util.ArrayList;

/* \brief The MainActivity. First Activity to be launched*/
public class MainActivity extends AppCompatActivity
                          implements ApplicationModel.IDataCallback, SubDataset.ISubDatasetListener, MessageBuffer.IMessageBufferCallback, DatasetsFragment.IDatasetFragmentListener
{
    public static final String TAG="VFV";

    private ApplicationModel m_model;             /*!< The application data model */
    private DrawerLayout     m_drawerLayout;      /*!< The root layout. DrawerLayout permit to have a left menu*/
    private Button           m_deleteDataBtn;     /*!< The delete data button*/
    private RangeColorView   m_rangeColorView;    /*!< The range color view*/
    private SocketManager    m_socket;            /*!< Connection with the server application*/
    private ArrayDeque<Dataset> m_pendingDataset = new ArrayDeque<>(); /*!< The Dataset pending to be updated by the Server*/


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

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        getMenuInflater().inflate(R.menu.toolbar_menu, menu);
        return true;
    }

    public void changeCurrentSubDataset(SubDataset sd)
    {}

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
    public void onRangeColorChange(SubDataset sd, float min, float max, int mode)
    {}

    @Override
    public void onRotationEvent(SubDataset dataset, float dRoll, float dPitch, float dYaw)
    {
        Dataset parent       = null;
        int     subDatasetID = -1;

        //Fetch to which Dataset this SubDataset belongs to
        for(VTKDataset d : m_model.getVTKDatasets())
        {
            if(parent != null)
                break;
            for(int i = 0; i < d.getNbSubDataset(); i++)
            {
                if(d.getSubDataset(i).getNativePtr() == dataset.getNativePtr())
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
                    if(d.getSubDataset(i).getNativePtr() == dataset.getNativePtr())
                    {
                        parent       = d;
                        subDatasetID = i;
                        break;
                    }
                }
            }
        }

        //If everything is correct, send the rotation event
        if(subDatasetID != -1 && parent != null && parent.getID() >= 0)
            m_socket.push(SocketManager.createRotationEvent(parent, subDatasetID, dataset.getRotation()));
    }

    @Override
    public void onRotationEvent(SubDataset dataset, float[] quaternion)
    {
        //We made this changement... do nothing here
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
        VTKDataset dataset = new VTKDataset(parser, ptValues, cellValues, vtkFile.getName());
        dataset.setID(msg.getDataID());
        synchronized(m_model)
        {
            m_model.addVTKDataset(dataset);
        }
    }

    @Override
    public void onRotateDatasetMessage(RotateDatasetMessage msg)
    {
        //Find the dataset
        Dataset dataset = null;
        synchronized(m_model)
        {
            for(BinaryDataset d : m_model.getBinaryDatasets())
            {
                if(d.getID() == msg.getDatasetID())
                {
                    dataset = d;
                    break;
                }
            }

            if(dataset == null)
            for(VTKDataset d : m_model.getVTKDatasets())
            {
                if(d.getID() == msg.getDatasetID())
                {
                    dataset = d;
                    break;
                }
            }
        }

        if(dataset != null)
        {
            dataset.getSubDataset(msg.getSubDatasetID()).setRotation(msg.getRotation());
        }
    }

    @Override
    public void onMoveDatasetMessage(MoveDatasetMessage msg)
    {
        //TODO
    }

    @Override
    public void onHeadsetBindingInfoMessage(HeadsetBindingInfoMessage msg)
    {}

    @Override
    public void onChangeCurrentSubDataset(SubDataset sd)
    {
        m_rangeColorView.getModel().setColorMode(sd.getColorMode());
        m_rangeColorView.getModel().setRange(sd.getMinClampingColor(), sd.getMaxClampingColor());
    }

    /** Set up the main layout*/
    private void setUpMainLayout()
    {
        ViewPager        viewPager    = (ViewPager)findViewById(R.id.viewpager);
        ViewPagerAdapter adapter      = new ViewPagerAdapter(getSupportFragmentManager());

        //Add "Datasets" tab
        DatasetsFragment dataFragment = new DatasetsFragment();
        dataFragment.setUpModel(m_model);
        adapter.addFragment(dataFragment, "Datasets");

        //Add "Annotations" tab
        AnnotationsFragment annotationsFragment = new AnnotationsFragment();
        annotationsFragment.setUpModel(m_model);
        adapter.addFragment(annotationsFragment, "Annotations");

        viewPager.setAdapter(adapter);
        TabLayout tabLayout = (TabLayout)findViewById(R.id.tabs);
        tabLayout.setupWithViewPager(viewPager);
    }

    /** \brief Set up the drawer layout (root layout)*/
    private void setUpDrawerLayout()
    {
        m_drawerLayout  = (DrawerLayout)findViewById(R.id.rootLayout);

        m_rangeColorView = (RangeColorView)findViewById(R.id.rangeColorView);
        m_model.setRangeColorModel(m_rangeColorView.getModel());

        //Configure the spinner color mode
        Spinner colorModeSpinner = (Spinner)findViewById(R.id.colorModeSpinner);
        colorModeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()
        {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l)
            {}

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
                    BinaryDataset fd = new BinaryDataset(df.getFile());
                    synchronized(m_model)
                    {
                        m_model.addBinaryDataset(fd);
                    }
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
                            VTKFieldValue[] ptValues = new VTKFieldValue[ptValuesList.size()];
                            ptValues = ptValuesList.toArray(ptValues);
                            ArrayList<VTKFieldValue> cellValuesList = dialog.getSelectedCellFieldValues();
                            VTKFieldValue[] cellValues = new VTKFieldValue[cellValuesList.size()];
                            cellValues = cellValuesList.toArray(cellValues);

                            //Add into the model
                            synchronized(m_model)
                            {
                                m_model.addVTKDataset(new VTKDataset(dialog.getVTKParser(), ptValues, cellValues, df.getFile().getName()));
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
    private void onAddDataset(Dataset d)
    {
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