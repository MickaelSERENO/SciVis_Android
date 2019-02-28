package com.sereno.vfv;

import android.app.DialogFragment;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.v4.view.GravityCompat;
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
import com.sereno.vfv.Network.EmptyMessage;
import com.sereno.vfv.Network.MessageBuffer;
import com.sereno.vfv.Network.SocketManager;
import com.sereno.view.RangeColorView;
import com.sereno.view.TreeView;

import org.w3c.dom.Text;

import java.util.ArrayDeque;
import java.util.ArrayList;

/* \brief The MainActivity. First Activity to be launched*/
public class MainActivity extends AppCompatActivity
                          implements ApplicationModel.IDataCallback, SubDataset.ISubDatasetCallback, MessageBuffer.IMessageBufferCallback
{
    public static final String TAG="VFV";

    private ApplicationModel m_model;             /*!< The application data model */
    private DrawerLayout     m_drawerLayout;      /*!< The root layout. DrawerLayout permit to have a left menu*/
    private Button           m_deleteDataBtn;     /*!< The delete data button*/
    private RangeColorView   m_rangeColorView;    /*!< The range color view*/
    private VFVSurfaceView   m_surfaceView;       /*!< The surface view displaying the vector field*/
    private TreeView         m_previewLayout;     /*!< The preview layout*/
    private Bitmap           m_noSnapshotBmp;     /*!< The bitmap used when no preview is available*/
    private SubDataset       m_currentSubDataset; /*!< The current application sub dataset*/
    private SocketManager    m_socket;            /*!< Connection with the server application*/
    private ArrayDeque<Dataset> m_pendingDataset = new ArrayDeque<>(); /*!< The Dataset pending to be updated by the Server*/


    /** @brief OnCreate function. Called when the activity is on creation*/
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        m_noSnapshotBmp = BitmapFactory.decodeResource(getResources(), R.drawable.no_snapshot);

        m_model = new ApplicationModel(this);
        m_model.addCallback(this);
        m_socket = new SocketManager(m_model.getConfiguration().getServerIP(),
                                     m_model.getConfiguration().getServerPort());
        m_socket.getMessageBuffer().addListener(this);

        setContentView(R.layout.main_activity);

        //Set up all internal components
        setUpDrawerLayout();
        setUpToolbar();
        setUpHiddenMenu();
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
                        EditText txt = view.findViewById(R.id.hololensIP);
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

    @Override
    public void onAddBinaryDataset(ApplicationModel model, BinaryDataset d)
    {
        m_deleteDataBtn.setVisibility(View.VISIBLE);
        addDataset(d);
    }

    @Override
    public void onAddVTKDataset(ApplicationModel model, VTKDataset d)
    {
        addDataset(d);
        m_socket.push(SocketManager.createAddVTKDatasetEvent(d));
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

        if(subDatasetID != -1 && parent != null)
            m_socket.push(SocketManager.createRotationEvent(parent, subDatasetID, dataset.getRotation()));
    }

    @Override
    public void onEmptyMessage(EmptyMessage msg)
    {
    }

    @Override
    public void onAcknowledgeAddDatasetMessage(AcknowledgeAddDatasetMessage msg)
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

    /** \brief Set up the drawer layout (root layout)*/
    private void setUpDrawerLayout()
    {
        ImageView noSnapshotView = new ImageView(this);

        m_drawerLayout  = findViewById(R.id.rootLayout);
        m_surfaceView   = findViewById(R.id.mainView);
        m_previewLayout = findViewById(R.id.previewLayout);
        m_model.addCallback(m_surfaceView);

        //Configure the spinner color mode
        m_rangeColorView = findViewById(R.id.rangeColorView);
        Spinner colorModeSpinner = findViewById(R.id.colorModeSpinner);
        colorModeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()
        {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l)
            {
                m_rangeColorView.setColorMode((int)l);
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
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        //Add an home button
        ActionBar actionbar = getSupportActionBar();
        actionbar.setDisplayHomeAsUpEnabled(true);
    }

    /** \brief Setup the hidden menu (left menu) part of the drawer */
    private void setUpHiddenMenu()
    {
        m_deleteDataBtn  = findViewById(R.id.deleteDataBtn);
        Button addButton = findViewById(R.id.addNewDataBtn);
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
                Spinner dataSpinner = view.findViewById(R.id.openDatasetSpinner);
                final DataFile df   = (DataFile)dataSpinner.getSelectedItem();

                String fileName = df.getFile().getName();

                //Binary dataset
                if(fileName.endsWith(".data"))
                {
                    BinaryDataset fd = new BinaryDataset(df.getFile());
                    m_model.addBinaryDataset(fd);
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
                            m_model.addVTKDataset(new VTKDataset(dialog.getVTKParser(), ptValues, cellValues, df.getFile().getName()));
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

    private void addDataset(Dataset d)
    {
        //Add the preview
        m_pendingDataset.add(d);
        TextView   dataText = new TextView(this);
        dataText.setText(d.getName());
        Tree<View> dataView = new Tree<View>(dataText);
        m_previewLayout.getData().addChild(dataView, -1);

        for(int i = 0; i < d.getNbSubDataset(); i++)
        {
            //Set the color range listener
            final SubDataset sd = d.getSubDataset(i);
            sd.addListener(this);

            m_rangeColorView.addOnRangeChangeListener(new RangeColorView.OnRangeChangeListener()
            {
                @Override
                public void onRangeChange(RangeColorView view, float minVal, float maxVal, int mode)
                {
                    sd.setRangeColor(minVal, maxVal, mode);
                }
            });
            m_rangeColorView.setRange(0.0f, 1.0f);
            m_rangeColorView.setColorMode(ColorMode.RAINBOW);

            //Add the snap image
            ImageView snapImg = new ImageView(this);
            snapImg.setAdjustViewBounds(true);
            snapImg.setScaleType(ImageView.ScaleType.CENTER_INSIDE);
            snapImg.setImageResource(R.drawable.no_snapshot);
            dataView.addChild(new Tree<View>(snapImg), -1);
        }
    }
}