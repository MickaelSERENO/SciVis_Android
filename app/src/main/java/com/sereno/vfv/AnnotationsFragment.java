package com.sereno.vfv;

import android.content.ClipData;
import android.graphics.PorterDuff;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Point;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
import android.view.DragEvent;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.PopupMenu;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;

import com.sereno.Tree;
import com.sereno.color.HSVColor;
import com.sereno.vfv.Data.Annotation.AnnotationLogComponent;
import com.sereno.vfv.Data.Annotation.AnnotationLogContainer;
import com.sereno.vfv.Data.Annotation.AnnotationPosition;
import com.sereno.vfv.Data.Annotation.DrawableAnnotationPosition;
import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.CloudPointDataset;
import com.sereno.vfv.Data.TF.TransferFunction;
import com.sereno.vfv.Data.VectorFieldDataset;
import com.sereno.vfv.Data.CPCPTexture;
import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Data.VTKDataset;
import com.sereno.vfv.Network.HeadsetBindingInfoMessage;
import com.sereno.vfv.Network.HeadsetsStatusMessage;
import com.sereno.view.AnnotationCanvasData;
import com.sereno.view.AnnotationStroke;
import com.sereno.view.AnnotationText;
import com.sereno.view.AnnotationCanvasView;
import com.sereno.view.ColorPickerData;
import com.sereno.view.ColorPickerView;
import com.sereno.view.CustomShadowBuilder;
import com.sereno.view.TreeView;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class AnnotationsFragment extends VFVFragment implements ApplicationModel.IDataCallback, AnnotationCanvasData.IAnnotationDataListener, AnnotationStroke.IAnnotationStrokeListener, AnnotationText.IAnnotationTextListener, SubDataset.ISubDatasetListener, Dataset.IDatasetListener, DrawableAnnotationPosition.IDrawableAnnotationPositionListener
{
    private static class AnnotationBitmap
    {
        public Bitmap bitmap;
        public ImageView imageView;
    }

    private static class HeaderID
    {
        public int id = -1;
        public String header;

        public HeaderID(int _id, String _header)
        {
            id = _id;
            header = _header;
        }

        @Override
        public String toString()
        {
            return header;
        }
    };

    /** Interface proposing callback methods regarding the AnnotationFragment*/
    public interface IAnnotationsFragmentListener
    {
        /** Add a new annotation position in the model container
         * @param frag  the fragment calling this function
         * @param annot the annotation log container where a new position entry should be added*/
        void onAddAnnotationPosition(AnnotationsFragment frag, AnnotationLogContainer annot);

        /** Function called when a request to set the headers of an annotation position is fired
         * @param frag the fragment calling this function
         * @param pos the AnnotationPosition to consider
         * @param indexes the new indexes. Size == 3*/
        void onSetAnnotationPositionIndexes(AnnotationsFragment frag, AnnotationPosition pos, int[] indexes);

        /** Link a SubDataset with an AnnotationPosition object
         * @param frag the fragment calling this function
         * @param sd the SubDataset to which a request is made
         * @param pos the AnnotationPosition to add to sd*/
        void onLinkSubDatasetAnnotationPosition(AnnotationsFragment frag, SubDataset sd, AnnotationPosition pos);

        void onSetDrawableAnnotationPositionColor(AnnotationsFragment frag, DrawableAnnotationPosition pos, com.sereno.color.Color color);

        void onSetDrawableAnnotationPositionMappedDataIndices(AnnotationsFragment frag, DrawableAnnotationPosition pos, int[] idx);
    }

    /** The lab for the drag and drop operation made on subdatasets*/
    private static final String LABEL_SUBDATASET_DRAG_AND_DROP = "SUBDATASET";

    /** The meme type associated to the local state "SubDataset"*/
    private static final String MIMETYPE_SUBDATASET = "data/Subdataset";

    private static final int NO_PANEL                        = -1;
    private static final int ANNOTATION_CANVAS_SD_PANEL      = 0;
    private static final int ANNOTATION_LOG_PANEL            = 1;
    private static final int ANNOTATION_POSITION_SD_PANEL    = 2;
    private static final int PENDING_ANNOTATION_CANVAS_PANEL = 3;

    /** What is the current panel?*/
    private int m_currentPanel = NO_PANEL;

    /** The application model in use*/
    private ApplicationModel m_model = null;

    /** The Panel used for annotation Canvas*/
    private View m_annotationCanvasPanel;

    /** The Panel used for annotation log*/
    private View m_annotationLogPanel;

    /** The Panel used for annotation position (configurable ones)*/
    private View m_drawableAnnotationPositionPanel;

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

    /** The default "background" for unselected views*/
    private Drawable m_defaultImageViewBackground;

    /** The stroke parameter layout*/
    private LinearLayout m_strokeParamLayout = null;

    /** The stroke parameter layout*/
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

    /** The array to update the table concerning log annotation positions*/
    private ArrayList<Runnable> m_updateAnnotLogPositionTable = new ArrayList<>();

    /******************************/
    /**********MODEL DATA**********/
    /******************************/

    /** The bitmap showing the content of the annotations*/
    private HashMap<AnnotationCanvasData, AnnotationBitmap> m_bitmaps = new HashMap<>();

    /** The trees of SubDataset*/
    private HashMap<SubDataset, Tree<View>> m_subDatasetTrees = new HashMap<>();

    /** The trees per Dataset*/
    private HashMap<Dataset, Tree<View>> m_datasetTrees = new HashMap<>();

    /** The trees per Annotation Canvas*/
    private HashMap<AnnotationCanvasData, Tree<View>> m_annotationCanvasTrees = new HashMap<>();

    /** The trees per drawable annotation position*/
    private HashMap<DrawableAnnotationPosition, Tree<View>> m_annotationPositionTrees = new HashMap<>();

    /** The trees per annotation log opened*/
    private HashMap<AnnotationLogContainer, Tree<View>> m_annotationLogTrees = new HashMap<>();

    /** The current Drawing mode*/
    private AnnotationCanvasData.AnnotationMode m_mode = AnnotationCanvasData.AnnotationMode.STROKE;

    /** The current stroke color*/
    private int m_currentStrokeColor = 0xff000000;

    /** The current text color*/
    private int m_currentTextColor = 0xff000000;

    /** The current annotation log container*/
    private AnnotationLogContainer m_currentSelectedAnnotLog = null;

    /** The current drawable annotation position in use*/
    private DrawableAnnotationPosition m_currentDrawableAnnotationPosition = null;

    /** The registered listeners*/
    private ArrayList<IAnnotationsFragmentListener> m_afListeners = new ArrayList<>(); /*!< Object that registered to this AnnotationsFragment events*/

    /** The context creating this fragment*/
    private Context m_ctx = null;

    private boolean m_inUpdate = false;

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
        if(m_model != null) m_model.removeListener(this);
        m_model = model;
        if(m_ctx != null) m_model.addListener(this);

        if(m_ctx != null)
        {
            //Call the callback functions
            for(VectorFieldDataset d : model.getVectorFieldDatasets())
                onAddVectorFieldDataset(m_model, d);
            for(VTKDataset d : model.getVTKDatasets())
                onAddVTKDataset(m_model, d);
            for(CloudPointDataset d : model.getCloudPointDataset())
                onAddCloudPointDataset(m_model, d);

            for(Dataset d : m_model.getDatasets())
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

    /** @param clbk the new callback to take account of
     * @brief Add a callback object to call at actions performed by the annotations fragment*/
    public void addAFListener(IAnnotationsFragmentListener clbk)
    {
        if(!m_afListeners.contains(clbk)) m_afListeners.add(clbk);
    }

    @Override
    public void onAttach(Context context)
    {
        m_ctx = context;
        super.onAttach(context);

        if(m_model != null) setUpModel(m_model);
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
    public void onRemoveSubDataset(Dataset dataset, SubDataset sd)
    {
        if(m_currentDrawableAnnotationPosition != null &&
           m_currentPanel == ANNOTATION_POSITION_SD_PANEL &&
           m_currentDrawableAnnotationPosition.getSubDataset() == sd)
            resetCentralView();
    }

    @Override
    public void onAddSubDataset(Dataset dataset, final SubDataset sd)
    {
        sd.addListener(this);

        getActivity().runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                final View sdTitle = getActivity().getLayoutInflater().inflate(R.layout.annotation_canvas_key_entry, null);
                sdTitle.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));

                TextView sdTitleText = (TextView) sdTitle.findViewById(R.id.annotation_key_entry_name);
                sdTitleText.setText(sd.getName());

                ImageView addView = (ImageView) sdTitle.findViewById(R.id.annotation_key_entry_add);
                addView.setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View view)
                    {
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

                sdTitle.findViewById(R.id.annotation_key_entry_drag_drop).setOnLongClickListener(new View.OnLongClickListener()
                {
                    @Override
                    public boolean onLongClick(View view)
                    {
                        ClipData data = new ClipData(LABEL_SUBDATASET_DRAG_AND_DROP, new String[]{MIMETYPE_SUBDATASET}, new ClipData.Item(sd.getName()));
                        View.DragShadowBuilder shadow = new CustomShadowBuilder(sdTitle);
                        view.startDrag(data, shadow, sd, 0);
                        return true;
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
    public void onLoadDataset(Dataset dataset, boolean success)
    {
    }

    @Override
    public void onLoadCPCPTexture(Dataset dataset, CPCPTexture texture)
    {
    }

    @Override
    public void onLoad1DHistogram(Dataset dataset, float[] values, int pID)
    {
    }

    private void onAddDataset(final Dataset d)
    {
        final Tree<View> t = m_previews.getModel();
        d.addListener(this);

        getActivity().runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
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
    {
    }

    @Override
    public void onPendingCanvasAnnotation(ApplicationModel model, SubDataset sd)
    {
        resetCentralView();
        m_currentPanel = PENDING_ANNOTATION_CANVAS_PANEL;
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
    public void onChangeCurrentAction(ApplicationModel model, int action)
    {

    }

    @Override
    public void onChangeCurrentSubDataset(ApplicationModel model, SubDataset sd)
    {

    }

    @Override
    public void onUpdateHeadsetsStatus(ApplicationModel model, HeadsetsStatusMessage.HeadsetStatus[] headsetsStatus)
    {
    }

    @Override
    public void onUpdateBindingInformation(ApplicationModel model, HeadsetBindingInfoMessage info)
    {
    }

    @Override
    public void onRemoveDataset(ApplicationModel model, Dataset dataset)
    {
        if(!m_datasetTrees.containsKey(dataset)) return;

        //Remove every sub datasets
        for(SubDataset sd : dataset.getSubDatasets())
        {
            if(!m_subDatasetTrees.containsKey(sd)) continue;

            Tree<View> sdTree = m_subDatasetTrees.get(sd);
            sdTree.setParent(null, 0);
        }

        //Remove the dataset entry
        m_datasetTrees.get(dataset).setParent(null, 0);
        m_datasetTrees.remove(dataset);
    }

    @Override
    public void onRemoveAnnotationLog(ApplicationModel model, AnnotationLogContainer annot)
    {
        if(!m_annotationLogTrees.containsKey(annot))
            return;

        if(annot == m_currentSelectedAnnotLog)
        {
            m_currentSelectedAnnotLog = null;
            if(m_currentPanel == ANNOTATION_LOG_PANEL) //This should always be true
                resetCentralView();
        }

        Tree<View> view = (Tree<View>)m_annotationLogTrees.get(annot);
        view.setParent(null, -1);
        m_annotationLogTrees.remove(annot);
    }

    @Override
    public void onUpdatePointingTechnique(ApplicationModel model, int pt)
    {
    }

    @Override
    public void onChangeTimeAnimationStatus(ApplicationModel model, boolean isInPlay, int speed, float step)
    {
    }

    @Override
    public void onSetSelectionMode(ApplicationModel model, int selectMode)
    {
    }

    @Override
    public void onAddAnnotationLog(ApplicationModel model, final AnnotationLogContainer container)
    {
        container.addListener(new AnnotationLogContainer.IAnnotationLogContainerListener()
        {
            @Override
            public void onAddAnnotationLogPosition(final AnnotationLogContainer container, final AnnotationPosition position)
            {
                if(container == m_currentSelectedAnnotLog)
                    updateAnnotationLogPanel();

                position.addListener(new AnnotationLogComponent.AnnotationLogComponentListener()
                {
                    @Override
                    public void onSetHeaders(final AnnotationLogComponent component)
                    {
                        //Update the view
                        getActivity().runOnUiThread(new Runnable()
                        {
                            @Override
                            public void run()
                            {
                                if(m_currentSelectedAnnotLog == container && m_currentPanel == ANNOTATION_LOG_PANEL)
                                    for(Runnable r : m_updateAnnotLogPositionTable)
                                        r.run();

                                if(m_currentDrawableAnnotationPosition != null && m_currentDrawableAnnotationPosition.getData() == position && m_currentPanel == ANNOTATION_POSITION_SD_PANEL)
                                    changeCurrentAnnotation(m_currentDrawableAnnotationPosition);
                            }
                        });
                    }
                });
            }

            @Override
            public void onRemoveAnnotationLogPosition(AnnotationLogContainer container, AnnotationPosition position)
            {
                if(container == m_currentSelectedAnnotLog)
                    updateAnnotationLogPanel();
            }
        });

        getActivity().runOnUiThread(new Runnable()
        {
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
    public void onSetLocation(ApplicationModel model, float[] pos, float[] rot)
    {
    }

    @Override
    public void onSetTabletScale(ApplicationModel model, float scale, float width, float height, float posx, float posy)
    {
    }

    @Override
    public void onSetLasso(ApplicationModel model, float[] lasso)
    {
    }

    @Override
    public void onConfirmSelection(ApplicationModel model)
    {
    }

    @Override
    public void onSetCurrentBooleanOperation(ApplicationModel model, int op)
    {
    }

    @Override
    public void onSetTangibleMode(ApplicationModel model, int tangibleMode)
    {
    }

    private void setUpAnnotationCanvasPanel(View v)
    {
        //The canvas annotation view objects
        m_annotView = (AnnotationCanvasView) v.findViewById(R.id.strokeTextView);
        m_annotView.setModel(null); //For the moment put it at null: we cannot draw anything (because no subdataset yet)
        m_strokeParamLayout = (LinearLayout) v.findViewById(R.id.annotationStrokeParamLayout);
        m_textParamLayout = (LinearLayout) v.findViewById(R.id.annotationTextParamLayout);
        m_pendingView = v.findViewById(R.id.annotPendingView);
        m_annotDrawButtonsView = v.findViewById(R.id.annotDrawButtons);
        m_pendingView.setVisibility(View.GONE);
        ColorPickerView strokeColorPicker = (ColorPickerView) v.findViewById(R.id.strokeColorPicker);
        ColorPickerView textColorPicker = (ColorPickerView) v.findViewById(R.id.textColorPicker);

        m_annotView.setOnTouchListener(new View.OnTouchListener()
        {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent)
            {
                onTouchSwippingEvent(motionEvent);
                return false;
            }
        });

        strokeColorPicker.setOnTouchListener(new View.OnTouchListener()
        {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent)
            {
                onTouchSwippingEvent(motionEvent);
                return false;
            }
        });

        textColorPicker.setOnTouchListener(new View.OnTouchListener()
        {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent)
            {
                onTouchSwippingEvent(motionEvent);
                return false;
            }
        });

        strokeColorPicker.getModel().addListener(new ColorPickerData.IColorPickerDataListener()
        {
            @Override
            public void onSetColor(ColorPickerData data, int color)
            {
                m_currentStrokeColor = color;
            }
        });

        textColorPicker.getModel().addListener(new ColorPickerData.IColorPickerDataListener()
        {
            @Override
            public void onSetColor(ColorPickerData data, int color)
            {
                m_currentTextColor = color;
            }
        });

        m_colorParam = (ImageView) v.findViewById(R.id.annotationStrokeParam);
        m_textMode = (ImageView) v.findViewById(R.id.annotationTextMode);
        m_imageImport = (ImageView) v.findViewById(R.id.annotationImportImage);

        m_colorParam.setOnTouchListener(new View.OnTouchListener()
        {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent)
            {
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

        m_textMode.setOnTouchListener(new View.OnTouchListener()
        {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent)
            {
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

    private void setUpAnnotationLog(View v)
    {
        m_annotLogFileName = (TextView) v.findViewById(R.id.annotLogFileName);
        m_annotLogHeaders = (LinearLayout) v.findViewById(R.id.annotLogTableHeaders);
        m_annotLogHeadersRow = v.findViewById(R.id.annotLogHeaderLayout);
        m_annotLogPositionTable = (TableLayout) v.findViewById(R.id.annotLogCurrentPosition);

        ImageView addPosition = (ImageView) v.findViewById(R.id.annotLogAddPosition);
        addPosition.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                if(m_currentSelectedAnnotLog != null && m_model.getBindingInfo() != null) //Connected and we are in the correct view to add position
                {
                    for(IAnnotationsFragmentListener list : m_afListeners)
                        list.onAddAnnotationPosition(AnnotationsFragment.this, m_currentSelectedAnnotLog);
                }
            }
        });
    }

    private void setUpDrawableAnnotationPosition(View v)
    {
        ColorPickerView annotPosColorView = v.findViewById(R.id.annotPositionColorPicker);
        annotPosColorView.setOnTouchListener(new View.OnTouchListener()
        {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent)
            {
                onTouchSwippingEvent(motionEvent);
                return false;
            }
        });
        annotPosColorView.getModel().addListener(new ColorPickerData.IColorPickerDataListener()
        {
            @Override
            public void onSetColor(ColorPickerData data, int color)
            {
                if(m_inUpdate)
                    return;
                if(m_currentDrawableAnnotationPosition != null && m_currentPanel == ANNOTATION_POSITION_SD_PANEL)
                    for(IAnnotationsFragmentListener l : m_afListeners)
                        l.onSetDrawableAnnotationPositionColor(AnnotationsFragment.this, m_currentDrawableAnnotationPosition, data.getColor().toRGB());
            }
        });

        CheckBox annotPosHasIndicesView = v.findViewById(R.id.annotPositionHasIndices);
        annotPosHasIndicesView.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener()
        {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b)
            {
                if(m_inUpdate)
                    return;
                if(m_currentDrawableAnnotationPosition != null && m_currentPanel == ANNOTATION_POSITION_SD_PANEL)
                {
                    if(!b)
                        for(IAnnotationsFragmentListener l : m_afListeners)
                            l.onSetDrawableAnnotationPositionMappedDataIndices(AnnotationsFragment.this, m_currentDrawableAnnotationPosition, new int[0]);
                    else
                    {
                        SubDataset sd = m_currentDrawableAnnotationPosition.getSubDataset();
                        if(sd == null)
                            return;

                        TransferFunction tf = sd.getTransferFunction();
                        if(tf == null)
                            return;

                        int[] newIdx = new int[tf.getDimension()];
                        for(int i = 0; i < newIdx.length; i++)
                            newIdx[i] = -1;

                        for(IAnnotationsFragmentListener l : m_afListeners)
                            l.onSetDrawableAnnotationPositionMappedDataIndices(AnnotationsFragment.this, m_currentDrawableAnnotationPosition, newIdx);
                    }
                }
            }
        });
    }

    /** Set up the main layout
     * @param v the main view containing all the Widgets*/
    private void setUpMainLayout(View v)
    {
        //The mains panels
        m_annotationCanvasPanel           = v.findViewById(R.id.annotCanvasView);
        m_annotationLogPanel              = v.findViewById(R.id.annotLogView);
        m_drawableAnnotationPositionPanel = v.findViewById(R.id.annotPositionView);

        //The tree views
        m_previews   = (TreeView) v.findViewById(R.id.annotPreviewLayout);
        m_logPreview = (TreeView) v.findViewById(R.id.logPreviewLayout);

        //Set up the main components
        setUpAnnotationCanvasPanel(v);
        setUpAnnotationLog(v);
        setUpDrawableAnnotationPosition(v);
    }

    /**
     * Set the image views button backgrounds to TRANSPARENT and hides parameter layouts
     */
    private void setImageViewBackgrounds()
    {
        m_colorParam.setBackground(m_defaultImageViewBackground);
        m_textMode.setBackground(m_defaultImageViewBackground);
        m_imageImport.setBackground(m_defaultImageViewBackground);
        m_strokeParamLayout.setVisibility(View.INVISIBLE);
        m_textParamLayout.setVisibility(View.INVISIBLE);
    }

    /**
     * Set the current mode to apply
     *
     * @param mode the mode to apply
     */
    private void setMode(AnnotationCanvasData.AnnotationMode mode)
    {
        if(mode == AnnotationCanvasData.AnnotationMode.STROKE)
        {
            getActivity().runOnUiThread(new Runnable()
            {
                @Override
                public void run()
                {
                    setImageViewBackgrounds();
                    m_colorParam.setBackgroundResource(R.drawable.round_rectangle_background);
                    m_strokeParamLayout.setVisibility(View.VISIBLE);
                }
            });
        } else if(mode == AnnotationCanvasData.AnnotationMode.TEXT)
        {
            getActivity().runOnUiThread(new Runnable()
            {
                @Override
                public void run()
                {
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

    /** Update the bitmap bound to an annotation data
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

    /** Update the annotation log panel correctly*/
    private void updateAnnotationLogPanel()
    {
        m_currentPanel = ANNOTATION_LOG_PANEL;
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
        } else m_annotLogHeadersRow.setVisibility(View.GONE);

        //Redo the position table
        m_annotLogPositionTable.removeAllViews();

        m_updateAnnotLogPositionTable = new ArrayList<>();

        for(final AnnotationPosition pos : m_currentSelectedAnnotLog.getAnnotationPositions())
        {
            View positionView = getActivity().getLayoutInflater().inflate(R.layout.annotation_log_position_entry, null);
            positionView.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));

            final Spinner xSpinner = (Spinner) positionView.findViewById(R.id.annotPositionXSpinner);
            final Spinner ySpinner = (Spinner) positionView.findViewById(R.id.annotPositionYSpinner);
            final Spinner zSpinner = (Spinner) positionView.findViewById(R.id.annotPositionZSpinner);

            m_updateAnnotLogPositionTable.add(new Runnable()
            {
                @Override
                public void run()
                {
                    int[] posHeaders = pos.getHeaders();
                    int[] remainingHeaders = m_currentSelectedAnnotLog.getRemainingHeaders();
                    String[] headers;
                    if(m_currentSelectedAnnotLog.hasHeaders())
                    {
                        headers = m_currentSelectedAnnotLog.getHeaders();
                    } else
                    {
                        headers = new String[m_currentSelectedAnnotLog.getNbColumns()];
                        for(int i = 0; i < headers.length; i++)
                            headers[i] = Integer.toString(i);
                    }

                    Spinner[] spinners = new Spinner[]{xSpinner, ySpinner, zSpinner};

                    for(int i = 0; i < 3; i++)
                    {
                        ArrayAdapter<HeaderID> adapter = new ArrayAdapter<>(getActivity(), android.R.layout.simple_spinner_dropdown_item);

                        //Add this header
                        if(posHeaders[i] != -1)
                            adapter.add(new HeaderID(posHeaders[i], headers[posHeaders[i]]));

                        //Followed by -1
                        adapter.add(new HeaderID(-1, "-1"));

                        //Followed by the remaining ones
                        for(int rem : remainingHeaders)
                            adapter.add(new HeaderID(rem, headers[rem]));

                        //Modify the spinners
                        spinners[i].setAdapter(adapter);
                        spinners[i].setSelection(0);
                    }
                }
            });

            xSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()
            {
                @Override
                public void onItemSelected(AdapterView<?> adapterView, View view, int i, long _l)
                {
                    int[] posHeaders = pos.getHeaders();
                    HeaderID hID = (HeaderID) adapterView.getSelectedItem();

                    if(hID.id == posHeaders[0])
                        return;

                    for(IAnnotationsFragmentListener l : m_afListeners)
                        l.onSetAnnotationPositionIndexes(AnnotationsFragment.this, pos, new int[]{hID.id, posHeaders[1], posHeaders[2]});
                }

                @Override
                public void onNothingSelected(AdapterView<?> adapterView)
                {
                }
            });

            ySpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()
            {
                @Override
                public void onItemSelected(AdapterView<?> adapterView, View view, int i, long _l)
                {
                    int[] posHeaders = pos.getHeaders();
                    HeaderID hID = (HeaderID) adapterView.getSelectedItem();

                    if(hID.id == posHeaders[1])
                        return;

                    for(IAnnotationsFragmentListener l : m_afListeners)
                        l.onSetAnnotationPositionIndexes(AnnotationsFragment.this, pos, new int[]{posHeaders[0], hID.id, posHeaders[2]});
                }

                @Override
                public void onNothingSelected(AdapterView<?> adapterView)
                {
                }
            });

            zSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()

            {
                @Override
                public void onItemSelected(AdapterView<?> adapterView, View view, int i, long _l)
                {
                    int[] posHeaders = pos.getHeaders();
                    HeaderID hID = (HeaderID) adapterView.getSelectedItem();

                    if(hID.id == posHeaders[2])
                        return;

                    for(IAnnotationsFragmentListener l : m_afListeners)
                        l.onSetAnnotationPositionIndexes(AnnotationsFragment.this, pos, new int[]{posHeaders[0], posHeaders[1], hID.id});
                }

                @Override
                public void onNothingSelected(AdapterView<?> adapterView)
                {
                }
            });

            //Handle drag event
            positionView.setOnDragListener(new View.OnDragListener()
            {
                @Override
                public boolean onDrag(View v, DragEvent dragEvent)
                {
                    if(dragEvent.getAction() == DragEvent.ACTION_DRAG_STARTED || dragEvent.getAction() == DragEvent.ACTION_DRAG_EXITED)
                    {
                        // Determines if this View can accept the dragged data
                        if(dragEvent.getClipDescription().hasMimeType(MIMETYPE_SUBDATASET))
                        {
                            v.getBackground().setColorFilter(Color.argb(0.25f, 0.0f, 0.0f, 1.0f), PorterDuff.Mode.LIGHTEN);
                            v.invalidate();
                            return true;

                        }
                    }

                    else if(dragEvent.getAction() == DragEvent.ACTION_DRAG_ENTERED)
                    {
                        v.getBackground().setColorFilter(Color.argb(0.25f, 0.0f, 1.0f, 0.0f), PorterDuff.Mode.LIGHTEN);
                        v.invalidate();

                        return true;
                    }

                    else if(dragEvent.getAction() == DragEvent.ACTION_DROP)
                    {
                        for(IAnnotationsFragmentListener list : m_afListeners)
                            list.onLinkSubDatasetAnnotationPosition(AnnotationsFragment.this, ((SubDataset) dragEvent.getLocalState()), pos);
                        return true;
                    }

                    else if(dragEvent.getAction() == DragEvent.ACTION_DRAG_ENDED)
                    {
                        v.getBackground().clearColorFilter();
                        v.invalidate();
                        return true;
                    }
                    return false;
                }
            });

            m_annotLogPositionTable.addView(positionView);
        }

        for(Runnable r : m_updateAnnotLogPositionTable)
            r.run();
    }

    /** Reset the central view (framelayout) displaying the current objects to empty*/
    private void resetCentralView()
    {
        m_currentPanel = NO_PANEL;

        //Hide views
        m_annotationCanvasPanel.setVisibility(View.GONE);
        m_annotationLogPanel.setVisibility(View.GONE);
        m_drawableAnnotationPositionPanel.setVisibility(View.GONE);

        //Reset models
        m_annotView.setModel(null);
        m_updateAnnotLogPositionTable.clear();

        //Hide backgrounds
        if(m_currentSelectedAnnotLog != null)
            m_annotationLogTrees.get(m_currentSelectedAnnotLog).value.setBackground(m_defaultImageViewBackground);
        m_currentSelectedAnnotLog = null;
        if(m_currentDrawableAnnotationPosition != null)
            m_annotationPositionTrees.get(m_currentDrawableAnnotationPosition).value.setBackground(m_defaultImageViewBackground);
        m_currentDrawableAnnotationPosition = null;
        for(Map.Entry<AnnotationCanvasData, AnnotationBitmap> bmp : m_bitmaps.entrySet())
            bmp.getValue().imageView.setBackground(m_defaultImageViewBackground);
    }

    private void updateCurrentDrawableAnnotationPanel()
    {
        boolean oldInUpdate = m_inUpdate;
        m_inUpdate = true;
        AnnotationLogContainer container = m_currentDrawableAnnotationPosition.getData().getAnnotationLog();
        int[]    headers    = m_currentDrawableAnnotationPosition.getData().getHeaders();
        String[] headersStr = new String[headers.length];

        //Dimensions
        TextView dimensionView = m_drawableAnnotationPositionPanel.findViewById(R.id.annotPositionDimension);
        String dimensionText = "";

        if(m_currentDrawableAnnotationPosition.getData().getAnnotationLog().hasHeaders())
            for(int i = 0; i < headersStr.length; i++)
            {
                if(headers[i] != -1)
                    headersStr[i] = m_currentDrawableAnnotationPosition.getData().getAnnotationLog().getHeaders()[headers[i]];
                else
                    headersStr[i] = "-1";
            }
        else
            for(int i = 0; i < headersStr.length; i++)
                headersStr[i] = Integer.toString(headers[i]);

        for(int i = 0; i < headersStr.length-1; i++)
            dimensionText += headersStr[i] + " | ";
        if(headersStr.length > 0)
            dimensionText += headersStr[headersStr.length-1];
        dimensionView.setText(dimensionText);

        //Color
        ColorPickerView colorView = m_drawableAnnotationPositionPanel.findViewById(R.id.annotPositionColorPicker);
        HSVColor col = new HSVColor(m_currentDrawableAnnotationPosition.getColor());
        if(col.s < 0.001f) //No need to change the hue...
            col.h = colorView.getModel().getColor().h;
        colorView.getModel().setColor(col);

        //Indices
        CheckBox hasIndicesView = m_drawableAnnotationPositionPanel.findViewById(R.id.annotPositionHasIndices);
        int[] idx = m_currentDrawableAnnotationPosition.getMappedDataIndices().clone();
        if(hasIndicesView.isChecked() != (idx.length != 0))
            hasIndicesView.setChecked(idx.length != 0);

        LinearLayout idxListView = m_drawableAnnotationPositionPanel.findViewById(R.id.annotPositionMapDataEntries);
        idxListView.removeAllViews();

        //Create the spinners for the indices
        for(int i = 0; i < idx.length; i++)
        {
            int       indice = idx[i];
            final int idxPos = i;
            Spinner s = new Spinner(getContext());
            s.setLayoutParams(new LinearLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));

            ArrayAdapter<HeaderID> adapter = new ArrayAdapter<>(getActivity(), android.R.layout.simple_spinner_item);
            adapter.add(new HeaderID(-1, "-1"));

            if(container.hasHeaders())
            {
                headersStr = container.getHeaders();
                for(int j = 0; j < headersStr.length; j++)
                    adapter.add(new HeaderID(j, headersStr[j]));
            }
            else
                for(int j = 0; j < container.getNbColumns(); j++)
                    adapter.add(new HeaderID(j, Integer.toString(j)));

            s.setAdapter(adapter);
            if(indice < headersStr.length)
                s.setSelection(indice+1);
            else
                s.setSelection(0);
            adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

            s.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()
            {
                @Override
                public void onItemSelected(AdapterView<?> adapterView, View view, int _i, long _l)
                {
                    if(m_inUpdate)
                        return;
                    if(m_currentDrawableAnnotationPosition == null)
                        return;
                    int[] posHeaders = m_currentDrawableAnnotationPosition.getMappedDataIndices();
                    HeaderID hID = (HeaderID)adapterView.getSelectedItem();
                    if(posHeaders[idxPos] != hID.id)
                    {
                        posHeaders[idxPos] = hID.id;
                        for(IAnnotationsFragmentListener l : m_afListeners)
                            l.onSetDrawableAnnotationPositionMappedDataIndices(AnnotationsFragment.this, m_currentDrawableAnnotationPosition, posHeaders);
                    }
                }

                @Override
                public void onNothingSelected(AdapterView<?> adapterView)
                {}
            });

            idxListView.addView(s);
        }
        m_inUpdate = oldInUpdate;
    }

    private void changeCurrentAnnotation(DrawableAnnotationPosition pos)
    {
        m_currentPanel = ANNOTATION_POSITION_SD_PANEL;

        Tree<View> tree = m_annotationPositionTrees.get(pos);
        if(tree == null)
            return;
        tree.value.setBackgroundResource(R.drawable.round_rectangle_background);
        m_currentDrawableAnnotationPosition = pos;
        m_drawableAnnotationPositionPanel.setVisibility(View.VISIBLE);

        updateCurrentDrawableAnnotationPanel();
    }

    private void changeCurrentAnnotation(AnnotationCanvasData annotation)
    {
        m_currentPanel = ANNOTATION_CANVAS_SD_PANEL;
        m_annotView.setVisibility(View.VISIBLE);

        //Our particular stylized
        m_bitmaps.get(annotation).imageView.setBackgroundResource(R.drawable.round_rectangle_background);
        annotation.setMode(m_mode);
        m_annotView.setModel(annotation);
    }

    /**
     * Function to enable or disable the swipping based on a motion event
     *
     * @param motionEvent the motion event received
     */
    private void onTouchSwippingEvent(MotionEvent motionEvent)
    {
        if(motionEvent.getAction() == MotionEvent.ACTION_DOWN)
        {
            for(IFragmentListener l : m_listeners)
                l.onDisableSwipping(AnnotationsFragment.this);
        } else if(motionEvent.getAction() == MotionEvent.ACTION_UP)
        {
            for(IFragmentListener l : m_listeners)
                l.onEnableSwipping(AnnotationsFragment.this);
        }
    }

    @Override
    public void onRotationEvent(SubDataset dataset, float[] quaternion)
    {
    }

    @Override
    public void onPositionEvent(SubDataset dataset, float[] position)
    {
    }

    @Override
    public void onScaleEvent(SubDataset dataset, float[] scale)
    {
    }

    @Override
    public void onSnapshotEvent(SubDataset dataset, Bitmap snapshot)
    {
    }

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
        snapImg.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                resetCentralView();
                changeCurrentAnnotation(annotation);
            }
        });
        bmp.imageView = snapImg;
        m_bitmaps.put(annotation, bmp);
        Tree<View> annotTree = new Tree<View>(snapImg);
        tree.addChild(annotTree, -1);
        m_annotationCanvasTrees.put(annotation, annotTree);

        //If no annotation yet added
        if(m_annotView.getModel() == null && m_currentPanel == NO_PANEL)
            changeCurrentAnnotation(annotation);
    }

    @Override
    public void onRemove(SubDataset dataset)
    {
        if(!m_subDatasetTrees.containsKey(dataset)) return;

        Tree<View> sdTree = m_subDatasetTrees.get(dataset);
        sdTree.setParent(null, 0);
        m_subDatasetTrees.remove(dataset);
        dataset.removeListener(this);
    }

    @Override
    public void onRemoveCanvasAnnotation(SubDataset dataset, AnnotationCanvasData annot)
    {
        if(annot == m_annotView.getModel())
        {
            m_annotView.setModel(null);
            if(m_currentPanel == ANNOTATION_CANVAS_SD_PANEL) //This should always be true
                resetCentralView();
        }

        if(m_bitmaps.containsKey(annot)) m_bitmaps.remove(annot);

        if(m_annotationCanvasTrees.containsKey(annot))
        {
            m_annotationCanvasTrees.get(annot).setParent(null, -1);
            m_annotationCanvasTrees.remove(annot);
        }
    }

    @Override
    public void onUpdateTF(SubDataset dataset)
    {
    }

    @Override
    public void onSetCurrentHeadset(SubDataset dataset, int headsetID)
    {
    }

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
        else sdTitle.setVisibility(View.GONE);
    }

    @Override
    public void onSetCanBeModified(SubDataset dataset, boolean status)
    {
        //Change the visibility of the add button
        View sdTitle = m_subDatasetTrees.get(dataset).value;
        ImageView addView = (ImageView) sdTitle.findViewById(R.id.annotation_key_entry_add);
        if(!dataset.getCanBeModified()) addView.setVisibility(View.GONE);
        else addView.setVisibility(View.VISIBLE);
    }

    @Override
    public void onSetMapVisibility(SubDataset dataset, boolean visibility)
    {
    }

    @Override
    public void onSetVolumetricMask(SubDataset dataset)
    {
    }

    @Override
    public void onAddDrawableAnnotationPosition(SubDataset dataset, final DrawableAnnotationPosition pos)
    {
        View view = getActivity().getLayoutInflater().inflate(R.layout.drawable_annotation_position_entry, null);

        TextView textID = view.findViewById(R.id.drawableAnnotationPositionID);
        textID.setText(Integer.toString(pos.getID()));

        view.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                resetCentralView();
                changeCurrentAnnotation(pos);
            }
        });

        Tree<View> tree   = new Tree<View>(view);
        Tree<View> sdTree = m_subDatasetTrees.get(dataset);
        sdTree.addChild(tree, -1);
        m_annotationPositionTrees.put(pos, tree);

        pos.addListener(this);

        if(m_currentPanel == NO_PANEL)
            changeCurrentAnnotation(pos);
    }

    @Override
    public void onSetDepthClipping(SubDataset dataset, float depthClipping){}

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


    @Override
    public void onSetColor(DrawableAnnotationPosition l, com.sereno.color.Color c)
    {
        m_inUpdate = true;
        if(m_currentPanel == ANNOTATION_POSITION_SD_PANEL && m_currentDrawableAnnotationPosition == l)
            updateCurrentDrawableAnnotationPanel();
        m_inUpdate = false;
    }

    @Override
    public void onSetMappedDataIndices(DrawableAnnotationPosition l, int[] idx)
    {
        m_inUpdate = true;
        if(m_currentPanel == ANNOTATION_POSITION_SD_PANEL && m_currentDrawableAnnotationPosition == l)
            updateCurrentDrawableAnnotationPanel();
        m_inUpdate = false;
    }
}