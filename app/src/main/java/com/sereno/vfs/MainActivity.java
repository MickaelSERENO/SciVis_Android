package com.sereno.vfs;

import android.app.DialogFragment;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.Spinner;

import com.sereno.gl.VFVSurfaceView;
import com.sereno.vfs.Data.ApplicationModel;
import com.sereno.vfs.Data.DataFile;
import com.sereno.vfs.Data.FluidDataset;
import com.sereno.vfs.Listener.INoticeDialogListener;
import com.sereno.view.RangeColorView;

/* \brief The MainActivity. First Activity to be launched*/
public class MainActivity extends AppCompatActivity
                          implements ApplicationModel.IDataCallback
{
    public static final String TAG="VFV";

    private ApplicationModel m_model;          /*!< The application data model */
    private DrawerLayout     m_drawerLayout;   /*!< The root layout. DrawerLayout permit to have a left menu*/
    private Button           m_deleteDataBtn;  /*!< The delete data button*/
    private RangeColorView   m_rangeColorView; /*!< The range color view*/
    private VFVSurfaceView   m_surfaceView;    /*!< The surface view displaying the vector field*/

    /** \brief OnCreate function. Called when the activity is on creation*/
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        m_model = new ApplicationModel();
        setContentView(R.layout.main_activity);

        //Set up all internal components
        setUpDrawerLayout();
        setUpToolbar();
        setUpHiddenMenu();
    }

    /** \brief Function called when the options items from the Toolbar are selected
     * \param item the MenuItem selected*/
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

                    }

                    @Override
                    public void onDialogNegativeClick(DialogFragment dialogFrag, View view)
                    {

                    }
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

    /** \brief Function called when the model has added a new dataset
     * \param model the model which fired this call
     * \param d the new dataset added*/
    @Override
    public void onAddDataset(ApplicationModel model, FluidDataset d)
    {
        m_deleteDataBtn.setVisibility(View.VISIBLE);
    }

    /** \brief Function called when the model is deleting a new dataset
     * \param model the model which fired this call
     * \param idx the index dataset being destroyed*/
    @Override
    public void onDeleteDataset(ApplicationModel model, int idx)
    {
        if(m_model.getFluidDatasets().size() == 1)
            m_deleteDataBtn.setVisibility(View.INVISIBLE);
    }

    /** \brief Set up the drawer layout (root layout)*/
    private void setUpDrawerLayout()
    {
        m_drawerLayout = findViewById(R.id.rootLayout);
        m_surfaceView  = findViewById(R.id.mainView);
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

        m_rangeColorView.addOnRangeChangeListener(new RangeColorView.OnRangeChangeListener()
        {
            @Override
            public void onRangeChange(RangeColorView view, float minVal, float maxVal, int mode)
            {
                m_surfaceView.setCurrentRangeColor(minVal, maxVal, mode);
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
                DataFile df         = (DataFile)dataSpinner.getSelectedItem();
                FluidDataset fd     = new FluidDataset(df.getFile());

                m_model.addFluidDataset(fd);
            }

            @Override
            public void onDialogNegativeClick(DialogFragment dialogFrag, View view)
            {

            }
        });
        dialogFragment.show(getFragmentManager(), "dialog");
    }
}