package com.sereno.vfs;

import android.app.Dialog;
import android.app.DialogFragment;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Spinner;

import com.sereno.gl.VFVSurfaceView;
import com.sereno.vfs.Data.ApplicationModel;
import com.sereno.vfs.Data.DataFile;
import com.sereno.vfs.Data.FluidDataset;
import com.sereno.vfs.Listener.INoticeDialogListener;

/* \brief The MainActivity. First Activity to be launched*/
public class MainActivity extends AppCompatActivity
                          implements ApplicationModel.IDataCallback, INoticeDialogListener
{
    private ApplicationModel m_model;         /*!< The application data model */
    private DrawerLayout     m_drawerLayout;  /*!< The root layout. DrawerLayout permit to have a left menu*/
    private Button           m_deleteDataBtn; /*!< The delete data button*/

    /* \brief OnCreate function. Called when the activity is on creation*/
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

    /* \brief Function called when the options items from the Toolbar are selected
     * \param item the MenuItem selected*/
    public boolean onOptionsItemSelected(MenuItem item)
    {
        switch (item.getItemId())
        {
            case android.R.id.home:
                m_drawerLayout.openDrawer(GravityCompat.START);
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    /* \brief Function called when the model has added a new dataset
     * \param model the model which fired this call
     * \param d the new dataset added*/
    @Override
    public void onAddDataset(ApplicationModel model, FluidDataset d)
    {
        m_deleteDataBtn.setVisibility(View.VISIBLE);
    }

    /* \brief Function called when the model is deleting a new dataset
     * \param model the model which fired this call
     * \param idx the index dataset being destroyed*/
    @Override
    public void onDeleteDataset(ApplicationModel model, int idx)
    {
        if(m_model.getFluidDatasets().size() == 1)
            m_deleteDataBtn.setVisibility(View.INVISIBLE);
    }

    /* \brief Function called when an AlertDialog constructor from a DialogFragment has pressed the positive button
     * \param dialog the fragment dialog which created the AlertDialog calling this function
     * \param v the dialog view calling this method*/
    @Override
    public void onDialogPositiveClick(DialogFragment dialogFragment, View v)
    {
        Spinner dataSpinner = v.findViewById(R.id.openDatasetSpinner);
        DataFile df         = (DataFile)dataSpinner.getSelectedItem();
        FluidDataset fd     = new FluidDataset(df.getFile());

        m_model.addFluidDataset(fd);
    }

    /* \brief Function called when an AlertDialog constructor from a DialogFragment has pressed the negative button
     * \param dialog the fragment dialog which created the AlertDialog calling this function*/
    @Override
    public void onDialogNegativeClick(DialogFragment dialogFragment, View v)
    {
    }

    /* \brief Set up the drawer layout (root layout)*/
    private void setUpDrawerLayout()
    {
        m_drawerLayout = findViewById(R.id.rootLayout);
        m_drawerLayout.addDrawerListener(new DrawerLayout.DrawerListener()
        {
            @Override
            public void onDrawerSlide(View drawerView, float slideOffset){}

            @Override
            public void onDrawerOpened(View drawerView){}

            @Override
            public void onDrawerClosed(View drawerView){}

            @Override
            public void onDrawerStateChanged(int newState){}
        });

        VFVSurfaceView surfaceView = findViewById(R.id.mainView);
        m_model.addCallback(surfaceView);
    }

    /* \brief Setup the toolbar */
    private void setUpToolbar()
    {
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        ActionBar actionbar = getSupportActionBar();
        actionbar.setDisplayHomeAsUpEnabled(true);
    }

    /* \brief Setup the hidden menu (left menu) part of the drawer */
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

    /* \brief Dialog about opening a new dataset */
    private void openNewDataDialog()
    {
        OpenDatasetDialogFragment dialogFragment = new OpenDatasetDialogFragment();
        dialogFragment.setNoticeDialogListener(this);
        dialogFragment.show(getFragmentManager(), "dialog");
    }
}