package com.sereno.vfv;

import android.support.v4.app.DialogFragment;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Point;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.PopupMenu;
import android.widget.Spinner;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;

import com.sereno.Tree;
import com.sereno.vfv.Data.Annotation.AnnotationLogContainer;
import com.sereno.vfv.Data.Annotation.AnnotationPosition;
import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.CloudPointDataset;
import com.sereno.vfv.Data.DataFile;
import com.sereno.vfv.Data.VectorFieldDataset;
import com.sereno.vfv.Data.CPCPTexture;
import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Data.VTKDataset;
import com.sereno.vfv.Dialog.Listener.INoticeDialogListener;
import com.sereno.vfv.Dialog.OpenAnnotationLogDialogFragment;
import com.sereno.vfv.Dialog.OpenDatasetDialogFragment;
import com.sereno.vfv.Network.HeadsetBindingInfoMessage;
import com.sereno.vfv.Network.HeadsetsStatusMessage;
import com.sereno.view.AnnotationCanvasData;
import com.sereno.view.AnnotationStroke;
import com.sereno.view.AnnotationText;
import com.sereno.view.AnnotationCanvasView;
import com.sereno.view.ColorPickerData;
import com.sereno.view.ColorPickerView;
import com.sereno.view.TreeView;

import org.w3c.dom.Text;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class AnnotationsFragment extends VFVFragment implements ApplicationModel.IDataCallback, AnnotationCanvasData.IAnnotationDataListener, AnnotationStroke.IAnnotationStrokeListener, AnnotationText.IAnnotationTextListener,
                                                                SubDataset.ISubDatasetListener, Dataset.IDatasetListener
{
    private static class AnnotationBitmap
    {
        public Bitmap    bitmap;
        public ImageView imageView;
    }

    /** Interface proposing callback methods regarding the AnnotationFragment*/
    public interface IAnnotationsFragmentListener
    {
        /** Called when the user wants to open an annotation fragment.
         * Note that the log can already be opened: it is the call of the listener to check that information
         * @param frag the fragment calling this function
         * @param path the data sub file
         * @param hasHeader has the data a header?
         * @param timeHeader which column should represent the time values? -1 == no time available*/
        void onOpenAnnotationLog(AnnotationsFragment frag, String path, boolean hasHeader, int timeHeader);

        /** Add a new model annotation position in the model container
         * @param frag the fragment calling this function
         * @param annot the annotation log container where a new position entry should be added*/
        void onAddModelAnnotationPosition(AnnotationsFragment frag, AnnotationLogContainer annot);
    }

    /** The application model in use*/
    private ApplicationModel m_model = null;

    /** The Panel used for annotation Canvas*/
    private View m_annotationCanvasPanel;

    /** The Panel used for annotation log*/
    private View m_annotationLogPanel;

    /** The TreeView layout containing the previews of all the annotations*/
    private TreeView m_previews;

    /** The TreeView layout containing the previews of all opened annotation log data*/
    private TreeView m_logPreview;

    /******************************/
    /********CANVAS WIDGETS********/
    /******************************/

    /** The annotation view*/
    private AnnotationCanvasView m_annotView;

    /** The view to display while waiting for an annotation to be anchored*/
    private View m_pendingView;

    /** The draw buttons (color, text, image) view*/
    private View m_annotDrawButtonsView;

    /** The image view text mode*/
    private ImageView m_textMode;

    /** The color parameters image view*/
    private ImageView m_colorParam;

    /** The import images image view*/
    private ImageView m_imageImport;

    /** The default "background" for unselected views */
    private Drawable m_defaultImageViewBackground;

    /**The stroke parameter layout*/
    private LinearLayout m_strokeParamLayout = null;

    /**The stroke parameter layout*/
    private LinearLayout m_textParamLayout = null;

    /******************************/
    /**********LOG WIDGETS*********/
    /******************************/

    /** The filename text view for log annot*/
    private TextView m_annotLogFileName;

    /** The Row containing the headers data*/
    private View m_annotLogHeadersRow;

    /** The table containing the headers' name*/
    private LinearLayout m_annotLogHeaders;

    /** The table containing the log position information*/
    private TableLayout m_annotLogPositionTable;

    /******************************/
    /**********MODEL DATA**********/
    /******************************/

    /** The bitmap showing the content of the annotations*/
    private HashMap<AnnotationCanvasData, AnnotationBitmap> m_bitmaps = new HashMap<>();

    /** The trees of SubDataset*/
    private HashMap<SubDataset, Tree<View>> m_subDatasetTrees = new HashMap<>();

    /** The trees per Dataset*/
    private HashMap<Dataset, Tree<View>> m_datasetTrees = new HashMap<>();

    /** The trees per Annotation*/
    private HashMap<AnnotationCanvasData, Tree<View>> m_annotationCanvasTrees = new HashMap<>();

    private HashMap<AnnotationLogContainer, Tree<View>> m_annotationLogTrees = new HashMap<>();

    /** The current Drawing mode*/
    private AnnotationCanvasData.AnnotationMode m_mode = AnnotationCanvasData.AnnotationMode.STROKE;

    /** The current stroke color*/
    private int m_currentStrokeColor = 0xff000000;

    /** The current text color*/
    private int m_currentTextColor = 0xff000000;

    /** The current annotation log container*/
    private AnnotationLogContainer m_currentSelectedAnnotLog = null;

    /** The registered listeners*/
    private ArrayList<IAnnotationsFragmentListener> m_afListeners = new ArrayList<>(); /*!< Object that registered to this AnnotationsFragment events*/

    /** The context creating this fragment*/
    private Context m_ctx  = null;

    /** @brief OnCreate function. Called when the activity is on creation*/
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceStates)
    {
        View v = inflater.inflate(R.layout.annotations_fragment, container, false);
        setUpMainLayout(v);
        setMode(AnnotationCanvasData.AnnotationMode.STROKE);
        return v;
    }

    /** Set up the model callback through this fragment
     * @param model the model to link with the internal views*/
    public void setUpModel(ApplicationModel model)
    {
        if(m_model != null)
            m_model.removeListener(this);
        m_model = model;
        if(m_ctx != null)
            m_model.addListener(this);

        if(m_ctx != null)
        {
            //Call the callback functions
            for (VectorFieldDataset d : model.getVectorFieldDatasets())
                onAddVectorFieldDataset(m_model, d);
            for (VTKDataset d : model.getVTKDatasets())
                onAddVTKDataset(m_model, d);
            for (CloudPointDataset d : model.getCloudPointDataset())
                onAddCloudPointDataset(m_model, d);

            for (Dataset d : m_model.getDatasets())
            {
                for(SubDataset sd : d.getSubDatasets())
                {
                    for(AnnotationCanvasData annot : sd.getAnnotations())
                        onAddCanvasAnnotation(sd, annot);
                }
            }

            if(m_model.getPendingSubDatasetForCanvasAnnotation() != null)
                onPendingCanvasAnnotation(m_model, m_model.getPendingSubDatasetForCanvasAnnotation());
        }
    }


    /** Remove an already registered listener for the AnnotationsFragment specification
     * @param clbk the listener to not call anymore*/
    public void removeAFListener(IAnnotationsFragmentListener clbk)
    {
        m_afListeners.remove(clbk);
    }

    /** @brief Add a callback object to call at actions performed by the annotations fragment
     * @param clbk the new callback to take account of*/
    public void addAFListener(IAnnotationsFragmentListener clbk)
    {
        if(!m_afListeners.contains(clbk))
            m_afListeners.add(clbk);
    }


    @Override
    public void onAttach(Context context)
    {
        m_ctx = context;
        super.onAttach(context);

        if(m_model != null)
            setUpModel(m_model);
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
    public void onAddVTKDataset(ApplicationModel model, final VTKDataset d)
    {
        onAddDataset(d);
    }

    @Override
    public void onRemoveSubDataset(Dataset dataset, SubDataset sd) {}

    @Override
    public void onAddSubDataset(Dataset dataset, final SubDataset sd)
    {
        sd.addListener(this);

        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                View sdTitle = getActivity().getLayoutInflater().inflate(R.layout.annotation_key_entry, null);
                sdTitle.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));

                TextView sdTitleText = (TextView) sdTitle.findViewById(R.id.annotation_key_entry_name);
                sdTitleText.setText(sd.getName());

                ImageView addView = (ImageView)sdTitle.findViewById(R.id.annotation_key_entry_add);
                addView.setOnClickListener(new View.OnClickListener(){
                    @Override
                    public void onClick(View view) {
                        PopupMenu popup = new PopupMenu(getContext(), view);
                        popup.getMenuInflater().inflate(R.menu.annotation_type_menu, popup.getMenu());

                        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener()
                        {
                            @Override
                            public boolean onMenuItemClick(MenuItem menuItem)
                            {
                                switch(menuItem.getItemId())
                                {
                                    case R.id.canvasAnnot_item:
                                        m_model.pendingCanvasAnnotation(sd);
                                        break;
                                    case R.id.logAnnot_item:
                                    {

                                    }
                                    break;
                                }
                                return true;
                            }
                        });

                        popup.show();
                    }
                });

                //Add the SubDataset title
                Tree<View> sdTitleTree = new Tree<View>(sdTitle);
                m_subDatasetTrees.put(sd, sdTitleTree);
                m_datasetTrees.get(sd.getParent()).addChild(sdTitleTree, -1);

                //Update some visibilities
                onSetOwner(sd, sd.getOwnerID());
                onSetCanBeModified(sd, sd.getCanBeModified());
            }
        });
    }

    @Override
    public void onLoadDataset(Dataset dataset, boolean success) {}

    @Override
    public void onLoadCPCPTexture(Dataset dataset, CPCPTexture texture) {}

    @Override
    public void onLoad1DHistogram(Dataset dataset, float[] values, int pID) {}

    private void onAddDataset(final Dataset d)
    {
        final Tree<View> t = m_previews.getModel();
        d.addListener(this);

        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                //Add the dataset title
                TextView title = new TextView(getContext());
                title.setText(d.getName());
                title.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));

                Tree<View> titleTree = new Tree<View>(title);

                m_datasetTrees.put(d, titleTree);
                t.addChild(titleTree, -1);
            }
        });
    }

    @Override
    public void onAddCanvasAnnotation(ApplicationModel model, AnnotationCanvasData annot, ApplicationModel.AnnotationMetaData metaData)
    { }

    @Override
    public void onPendingCanvasAnnotation(ApplicationModel model, SubDataset sd)
    {
        resetCentralView();
        m_annotationCanvasPanel.setVisibility(View.VISIBLE);
        m_pendingView.setVisibility(View.VISIBLE);
        m_annotView.setVisibility(View.GONE);
        m_annotDrawButtonsView.setVisibility(View.GONE);
    }

    @Override
    public void onEndPendingCanvasAnnotation(ApplicationModel model, SubDataset sd, boolean cancel)
    {
        resetCentralView();
        m_pendingView.setVisibility(View.GONE);
        m_annotView.setVisibility(View.VISIBLE);
        m_annotDrawButtonsView.setVisibility(View.VISIBLE);
    }

    @Override
    public void onChangeCurrentAction(ApplicationModel model, int action) {

    }

    @Override
    public void onChangeCurrentSubDataset(ApplicationModel model, SubDataset sd) {

    }

    @Override
    public void onUpdateHeadsetsStatus(ApplicationModel model, HeadsetsStatusMessage.HeadsetStatus[] headsetsStatus) {}

    @Override
    public void onUpdateBindingInformation(ApplicationModel model, HeadsetBindingInfoMessage info) {}

    @Override
    public void onRemoveDataset(ApplicationModel model, Dataset dataset)
    {
        if(!m_datasetTrees.containsKey(dataset))
            return;

        //Remove every sub datasets
        for(SubDataset sd : dataset.getSubDatasets())
        {
            if (!m_subDatasetTrees.containsKey(sd))
                continue;

            Tree<View> sdTree = m_subDatasetTrees.get(sd);
            sdTree.setParent(null, 0);
        }

        //Remove the dataset entry
        m_datasetTrees.get(dataset).setParent(null, 0);
        m_datasetTrees.remove(dataset);
    }

    @Override
    public void onUpdatePointingTechnique(ApplicationModel model, int pt) {}

    @Override
    public void onChangeTimeAnimationStatus(ApplicationModel model, boolean isInPlay, int speed, float step)
    {}

    @Override
    public void onSetSelectionMode(ApplicationModel model, int selectMode) {}

    @Override
    public void onAddAnnotationLog(ApplicationModel model, final AnnotationLogContainer container)
    {
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run()
            {
                //Create a title
                final TextView logTitle = new TextView(getContext());
                logTitle.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));
                File f = new File(container.getFilePath());
                logTitle.setText(f.getName());

                logTitle.setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View view)
                    {
                        //Show the correct pannel
                        resetCentralView();
                        m_annotationLogPanel.setVisibility(View.VISIBLE);

                        //Set the background
                        view.setBackgroundResource(R.drawable.round_rectangle_background);
                        m_currentSelectedAnnotLog = container;

                        //Update the view
                        updateAnnotationLogPanel();
                    }
                });

                //Add it to the list of displayed objects
                Tree<View> logTitleTree = new Tree<View>(logTitle);
                m_logPreview.getModel().addChild(logTitleTree, -1);
                m_annotationLogTrees.put(container, logTitleTree);
            }
        });
    }

    @Override
    public void onSetLocation(ApplicationModel model, float[] pos, float[] rot) {}

    @Override
    public void onSetTabletScale(ApplicationModel model, float scale, float width, float height, float posx, float posy) {}

    @Override
    public void onSetLasso(ApplicationModel model, float[] lasso) {}

    @Override
    public void onConfirmSelection(ApplicationModel model) {}

    @Override
    public void onSetCurrentBooleanOperation(ApplicationModel model, int op) {}

    @Override
    public void onSetTangibleMode(ApplicationModel model, int tangibleMode) {}


    /** Set up the main layout
     * @param v the main view containing all the Widgets*/
    private void setUpMainLayout(View v)
    {
        //The mains panels
        m_annotationCanvasPanel = v.findViewById(R.id.annotCanvasView);
        m_annotationLogPanel    = v.findViewById(R.id.annotLogView);

        //The tree views
        m_previews  = (TreeView)v.findViewById(R.id.annotPreviewLayout);
        m_logPreview = (TreeView)v.findViewById(R.id.logPreviewLayout);

        //The annotation log view objects
        m_annotLogFileName   = (TextView)v.findViewById(R.id.annotLogFileName);
        m_annotLogHeaders    = (LinearLayout)v.findViewById(R.id.annotLogTableHeaders);
        m_annotLogHeadersRow = v.findViewById(R.id.annotLogHeaderLayout);
        m_annotLogPositionTable = (TableLayout)v.findViewById(R.id.annotLogCurrentPosition);

        ImageView addPosition = (ImageView)v.findViewById(R.id.annotLogAddPosition);
        addPosition.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                if(m_currentSelectedAnnotLog != null && m_model.getBindingInfo() != null) //Connected and we are in the correct view to add position
                {
                    m_currentSelectedAnnotLog.pushAnnotationPosition(m_currentSelectedAnnotLog.initAnnotationPosition());
                    updateAnnotationLogPanel();
                }
            }
        });


        //The canvas annotation view objects
        m_annotView = (AnnotationCanvasView)v.findViewById(R.id.strokeTextView);
        m_annotView.setModel(null); //For the moment put it at null: we cannot draw anything (because no subdataset yet)
        m_strokeParamLayout = (LinearLayout)v.findViewById(R.id.annotationStrokeParamLayout);
        m_textParamLayout   = (LinearLayout)v.findViewById(R.id.annotationTextParamLayout);
        m_pendingView       = v.findViewById(R.id.annotPendingView);
        m_annotDrawButtonsView = v.findViewById(R.id.annotDrawButtons);
        m_pendingView.setVisibility(View.GONE);
        ColorPickerView strokeColorPicker = (ColorPickerView)v.findViewById(R.id.strokeColorPicker);
        ColorPickerView textColorPicker   = (ColorPickerView)v.findViewById(R.id.textColorPicker);

        m_annotView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                onTouchSwippingEvent(motionEvent);
                return false;
            }
        });

        strokeColorPicker.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                onTouchSwippingEvent(motionEvent);
                return false;
            }
        });

        textColorPicker.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                onTouchSwippingEvent(motionEvent);
                return false;
            }
        });

        strokeColorPicker.getModel().addListener(new ColorPickerData.IColorPickerDataListener() {
            @Override
            public void onSetColor(ColorPickerData data, int color) {
                m_currentStrokeColor = color;
            }
        });

        textColorPicker.getModel().addListener(new ColorPickerData.IColorPickerDataListener() {
            @Override
            public void onSetColor(ColorPickerData data, int color) {
                m_currentTextColor = color;
            }
        });

        m_colorParam  = (ImageView)v.findViewById(R.id.annotationStrokeParam);
        m_textMode    = (ImageView)v.findViewById(R.id.annotationTextMode);
        m_imageImport = (ImageView)v.findViewById(R.id.annotationImportImage);

        m_colorParam.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                if(motionEvent.getAction() == MotionEvent.ACTION_DOWN)
                {
                    AnnotationCanvasData.AnnotationMode mode = m_mode;
                    int visibility = m_strokeParamLayout.getVisibility();
                    setMode(AnnotationCanvasData.AnnotationMode.STROKE);

                    //If reselected, toggle the visibility
                    if(mode == AnnotationCanvasData.AnnotationMode.STROKE)
                        m_strokeParamLayout.setVisibility(visibility == View.INVISIBLE ? View.VISIBLE : View.INVISIBLE);
                    return true;
                }
                return false;
            }
        });

        m_textMode.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                if(motionEvent.getAction() == MotionEvent.ACTION_DOWN)
                {
                    AnnotationCanvasData.AnnotationMode mode = m_mode;
                    int visibility = m_textParamLayout.getVisibility();
                    setMode(AnnotationCanvasData.AnnotationMode.TEXT);

                    //If reselected, toggle the visibility
                    if(mode == AnnotationCanvasData.AnnotationMode.TEXT)
                        m_textParamLayout.setVisibility(visibility == View.INVISIBLE ? View.VISIBLE : View.INVISIBLE);
                    return true;
                }
                return false;
            }
        });

        m_defaultImageViewBackground = m_colorParam.getBackground();
    }

    /** Set the image views button backgrounds to TRANSPARENT and hides parameter layouts*/
    private void setImageViewBackgrounds()
    {
        m_colorParam.setBackground(m_defaultImageViewBackground);
        m_textMode.setBackground(m_defaultImageViewBackground);
        m_imageImport.setBackground(m_defaultImageViewBackground);
        m_strokeParamLayout.setVisibility(View.INVISIBLE);
        m_textParamLayout.setVisibility(View.INVISIBLE);
    }

    /** Set the current mode to apply
     * @param mode the mode to apply*/
    private void setMode(AnnotationCanvasData.AnnotationMode mode)
    {
        if(mode == AnnotationCanvasData.AnnotationMode.STROKE)
        {
            getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    setImageViewBackgrounds();
                    m_colorParam.setBackgroundResource(R.drawable.round_rectangle_background);
                    m_strokeParamLayout.setVisibility(View.VISIBLE);
                }
            });
        }
        else if(mode == AnnotationCanvasData.AnnotationMode.TEXT)
        {
            getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    setImageViewBackgrounds();
                    m_textMode.setBackgroundResource(R.drawable.round_rectangle_background);
                    m_textParamLayout.setVisibility(View.VISIBLE);
                }
            });
        }

        m_mode = mode;
        if(m_annotView.getModel() != null && m_annotView.getModel().getMode() != mode) //Not fire for nothing
            m_annotView.getModel().setMode(mode);
    }

    /**Update the bitmap bound to an annotation data
     * @param data the data being updated*/
    private void updateBitmap(final AnnotationCanvasData data)
    {
        AnnotationCanvasData savedModel = m_annotView.getModel(); //Save the last model
        m_annotView.setModel(data);

        //Draw on the bitmap
        AnnotationBitmap bmp = m_bitmaps.get(data);
        Canvas canvas = new Canvas(bmp.bitmap);
        m_annotView.draw(canvas);
        bmp.imageView.invalidate();

        m_annotView.setModel(savedModel);
    }

    /** Update the bitmap bound to a stroke
     * @param stroke the stroke being updated*/
    private void updateBitmapStroke(AnnotationStroke stroke)
    {
        for(AnnotationCanvasData key : m_bitmaps.keySet())
            for(AnnotationStroke s : key.getStrokes())
            {
                if(s == stroke)
                {
                    updateBitmap(key);
                    return;
                }
            }
    }

    /** Update the bitmap bound to a text
     * @param text the text being updated*/
    private void updateBitmapText(AnnotationText text)
    {
        for(AnnotationCanvasData key : m_bitmaps.keySet())
            for(AnnotationText t : key.getTexts())
            {
                if(t == text)
                {
                    updateBitmap(key);
                    return;
                }
            }
    }

    /** Update the annotation log panel correctely*/
    private void updateAnnotationLogPanel()
    {
        m_annotationLogPanel.setVisibility(View.VISIBLE);

        //Set text
        m_annotLogFileName.setText(m_currentSelectedAnnotLog.getFilePath());

        //Set the available headers
        if(m_currentSelectedAnnotLog.hasHeaders())
        {
            m_annotLogHeadersRow.setVisibility(View.VISIBLE);
            m_annotLogHeaders.removeAllViews();

            for(String h : m_currentSelectedAnnotLog.getHeaders())
            {
                final TextView headerView = new TextView(getContext());
                TableRow.LayoutParams params = new TableRow.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
                params.setMargins(8, 0, 8, 0);
                headerView.setLayoutParams(params);
                headerView.setText(h);

                m_annotLogHeaders.addView(headerView);
            }
        }
        else
            m_annotLogHeadersRow.setVisibility(View.GONE);

        //Redo the position table
        /*for(AnnotationPosition pos : m_currentSelectedAnnotLog.getAnnotationPosition())
        {

        }*/
    }

    /** Reset the central view (framelayout) displaying the current objects to empty*/
    private void resetCentralView()
    {
        //Hide views
        m_annotationCanvasPanel.setVisibility(View.GONE);
        m_annotationLogPanel.setVisibility(View.GONE);

        //Reset models
        if(m_currentSelectedAnnotLog != null)
            m_annotationLogTrees.get(m_currentSelectedAnnotLog).value.setBackground(m_defaultImageViewBackground);
        m_currentSelectedAnnotLog = null;
    }

    private void changeCurrentAnnotation(AnnotationPosition pos)
    {
        //TODO
    }

    private void changeCurrentAnnotation(ImageView snapImg, AnnotationCanvasData annotation)
    {
        //Eveything to default
        for(Map.Entry<AnnotationCanvasData, AnnotationBitmap> bmp : m_bitmaps.entrySet())
            bmp.getValue().imageView.setBackground(m_defaultImageViewBackground);

        //Our particular stylized
        m_bitmaps.get(annotation).imageView.setBackgroundResource(R.drawable.round_rectangle_background);
        annotation.setMode(m_mode);
        m_annotView.setModel(annotation);
    }

    /** Function to enable or disable the swipping based on a motion event
     * @param motionEvent the motion event received*/
    private void onTouchSwippingEvent(MotionEvent motionEvent)
    {
        if(motionEvent.getAction() == MotionEvent.ACTION_DOWN)
        {
            for(IFragmentListener l : m_listeners)
                l.onDisableSwipping(AnnotationsFragment.this);
        }
        else if(motionEvent.getAction() == MotionEvent.ACTION_UP)
        {
            for(IFragmentListener l : m_listeners)
                l.onEnableSwipping(AnnotationsFragment.this);
        }
    }

    @Override
    public void onRotationEvent(SubDataset dataset, float[] quaternion) {}

    @Override
    public void onPositionEvent(SubDataset dataset, float[] position) {}

    @Override
    public void onScaleEvent(SubDataset dataset, float[] scale) {}

    @Override
    public void onSnapshotEvent(SubDataset dataset, Bitmap snapshot) {}

    @Override
    public void onAddCanvasAnnotation(final SubDataset dataset, final AnnotationCanvasData annotation)
    {
        annotation.addListener(this);

        Tree<View> tree = m_subDatasetTrees.get(dataset);

        final AnnotationBitmap bmp = new AnnotationBitmap();
        bmp.bitmap = Bitmap.createBitmap(m_annotView.getWidth(), m_annotView.getHeight(), Bitmap.Config.ARGB_8888);

        final ImageView snapImg = new ImageView(getContext());
        snapImg.setAdjustViewBounds(true);
        snapImg.setScaleType(ImageView.ScaleType.CENTER_INSIDE);
        snapImg.setBackgroundColor(Color.WHITE);
        snapImg.setImageBitmap(bmp.bitmap);
        snapImg.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                changeCurrentAnnotation(snapImg, annotation);
            }
        });
        bmp.imageView = snapImg;
        m_bitmaps.put(annotation, bmp);
        Tree<View> annotTree = new Tree<View>(snapImg);
        tree.addChild(annotTree, -1);
        m_annotationCanvasTrees.put(annotation, annotTree);

        //If no annotation yet added
        if(m_annotView.getModel() == null)
            changeCurrentAnnotation(snapImg, annotation);
    }

    @Override
    public void onRemove(SubDataset dataset)
    {
        if(!m_subDatasetTrees.containsKey(dataset))
            return;

        Tree<View> sdTree = m_subDatasetTrees.get(dataset);
        sdTree.setParent(null, 0);
        m_subDatasetTrees.remove(dataset);
        dataset.removeListener(this);
    }

    @Override
    public void onRemoveCanvasAnnotation(SubDataset dataset, AnnotationCanvasData annot)
    {
        if(annot == m_annotView.getModel())
            m_annotView.setModel(null);

        if(m_bitmaps.containsKey(annot))
            m_bitmaps.remove(annot);

        if(m_annotationCanvasTrees.containsKey(annot))
        {
            m_annotationCanvasTrees.get(annot).setParent(null, -1);
            m_annotationCanvasTrees.remove(annot);
        }
    }

    @Override
    public void onUpdateTF(SubDataset dataset) {}

    @Override
    public void onSetCurrentHeadset(SubDataset dataset, int headsetID) {}

    @Override
    public void onSetOwner(SubDataset dataset, int headsetID)
    {
        View sdTitle = m_subDatasetTrees.get(dataset).value;

        if(m_model.getBindingInfo() == null)
        {
            sdTitle.setVisibility(View.GONE);
            return;
        }
        //Show public / our datasets, hide the others.
        if(headsetID == -1 || headsetID == m_model.getBindingInfo().getHeadsetID())
            sdTitle.setVisibility(View.VISIBLE);
        else
            sdTitle.setVisibility(View.GONE);
    }

    @Override
    public void onSetCanBeModified(SubDataset dataset, boolean status)
    {
        //Change the visibility of the add button
        View sdTitle = m_subDatasetTrees.get(dataset).value;
        ImageView addView = (ImageView)sdTitle.findViewById(R.id.annotation_key_entry_add);
        if(!dataset.getCanBeModified())
            addView.setVisibility(View.GONE);
        else
            addView.setVisibility(View.VISIBLE);
    }

    @Override
    public void onSetMapVisibility(SubDataset dataset, boolean visibility)
    {}

    @Override
    public void onSetVolumetricMask(SubDataset dataset)
    {}

    @Override
    public void onAddStroke(AnnotationCanvasData data, AnnotationStroke stroke)
    {
        stroke.addListener(this);
        stroke.setColor(m_currentStrokeColor);
    }

    @Override
    public void onAddText(AnnotationCanvasData data, AnnotationText text)
    {
        text.setColor(m_currentTextColor);
        text.addListener(this);
    }

    @Override
    public void onAddImage(AnnotationCanvasData data)
    {
        updateBitmap(data);
    }

    @Override
    public void onSetMode(AnnotationCanvasData data, AnnotationCanvasData.AnnotationMode mode)
    {
        updateBitmap(data);
    }

    @Override
    public void onAddPoint(AnnotationStroke stroke, Point p)
    {
        updateBitmapStroke(stroke);
    }

    @Override
    public void onSetColor(AnnotationStroke stroke, int c)
    {
        updateBitmapStroke(stroke);
    }

    @Override
    public void onSetWidth(AnnotationStroke stroke, float w)
    {
        updateBitmapStroke(stroke);
    }


    @Override
    public void onSetText(AnnotationText text, String str)
    {
        updateBitmapText(text);
    }

    @Override
    public void onSetPosition(AnnotationText text, Point pos)
    {
        updateBitmapText(text);
    }

    @Override
    public void onSetColor(AnnotationText text, int color)
    {
        updateBitmapText(text);
    }

}
