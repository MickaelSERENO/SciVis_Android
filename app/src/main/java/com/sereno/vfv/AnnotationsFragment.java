package com.sereno.vfv;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Point;
import android.graphics.drawable.Drawable;
import android.opengl.Visibility;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.sereno.Tree;
import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.BinaryDataset;
import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Data.SubDatasetMetaData;
import com.sereno.vfv.Data.VTKDataset;
import com.sereno.vfv.Network.HeadsetBindingInfoMessage;
import com.sereno.vfv.Network.HeadsetsStatusMessage;
import com.sereno.view.AnnotationData;
import com.sereno.view.AnnotationStroke;
import com.sereno.view.AnnotationText;
import com.sereno.view.AnnotationView;
import com.sereno.view.ColorPickerData;
import com.sereno.view.ColorPickerView;
import com.sereno.view.TreeView;

import java.util.HashMap;
import java.util.Map;

public class AnnotationsFragment extends VFVFragment implements ApplicationModel.IDataCallback, AnnotationData.IAnnotationDataListener, AnnotationStroke.IAnnotationStrokeListener, AnnotationText.IAnnotationTextListener,
                                                                SubDataset.ISubDatasetListener, SubDatasetMetaData.ISubDatasetMetaDataListener
{
    private static class AnnotationBitmap
    {
        public Bitmap    bitmap;
        public ImageView imageView;
    }

    /** The application model in use*/
    private ApplicationModel m_model = null;

    /** The TreeView layout containing the previews of all the annotations*/
    private TreeView m_previews;

    /** The annotation view*/
    private AnnotationView m_annotView;

    private View m_pendingView;

    private View m_annotDrawButtonsView;

    /** The image view text mode*/
    private ImageView m_textMode;

    /** The color parameters image view*/
    private ImageView m_colorParam;

    /** The import images image view*/
    private ImageView m_imageImport;

    private Drawable m_defaultImageViewBackground;

    /**The stroke parameter layout*/
    private LinearLayout m_strokeParamLayout = null;

    /**The stroke parameter layout*/
    private LinearLayout m_textParamLayout = null;

    /** The bitmap showing the content of the annotations*/
    private HashMap<AnnotationData, AnnotationBitmap> m_bitmaps = new HashMap<>();

    /** The trees of SubDataset*/
    private HashMap<SubDataset, Tree<View>> m_subDatasetTrees = new HashMap<>();

    /** The trees per Dataset*/
    private HashMap<Dataset, Tree<View>> m_datasetTrees = new HashMap<>();

    /** The trees per Annotation*/
    private HashMap<AnnotationData, Tree<View>> m_annotationTrees = new HashMap<>();

    /** The current Drawing mode*/
    private AnnotationData.AnnotationMode m_mode = AnnotationData.AnnotationMode.STROKE;

    /** The current stroke color*/
    private int m_currentStrokeColor = 0xff000000;

    /** The current text color*/
    private int m_currentTextColor = 0xff000000;

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
        setMode(AnnotationData.AnnotationMode.STROKE);
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
            for (BinaryDataset d : model.getBinaryDatasets())
                onAddBinaryDataset(m_model, d);
            for (VTKDataset d : model.getVTKDatasets())
                onAddVTKDataset(m_model, d);

            for (Dataset d : m_model.getDatasets())
            {
                for(SubDataset sd : d.getSubDatasets())
                {
                    for(AnnotationData annot : sd.getAnnotations())
                        onAddAnnotation(sd, annot);
                }
            }

            if(m_model.getPendingSubDatasetForAnnotation() != null)
                onPendingAnnotation(m_model, m_model.getPendingSubDatasetForAnnotation());
        }
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
    public void onAddBinaryDataset(ApplicationModel model, BinaryDataset d)
    {
        onAddDataset(d);
    }

    @Override
    public void onAddVTKDataset(ApplicationModel model, final VTKDataset d)
    {
        onAddDataset(d);
    }

    private void onAddDataset(final Dataset d)
    {
        for(SubDataset sd : d.getSubDatasets())
            m_model.getSubDatasetMetaData(sd).addListener(this);
        final Tree<View> t = m_previews.getModel();

        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                //Add the dataset title
                TextView title = new TextView(getContext());
                title.setText(d.getName());
                title.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));

                Tree<View> titleTree = new Tree<View>(title);

                //Add each subdataset
                for(int i = 0; i < d.getNbSubDataset(); i++)
                {
                    SubDatasetMetaData metaData = m_model.getSubDatasetMetaData(d.getSubDataset(i));
                    metaData.addListener(AnnotationsFragment.this);
                    SubDataset sds[] = new SubDataset[2];

                    sds[0] = metaData.getPublicState();
                    sds[1] = metaData.getPrivateState();

                    for(final SubDataset sd : sds)
                    {
                        if(sd != null)
                        {
                            sd.addListener(AnnotationsFragment.this);
                            View sdTitle = getActivity().getLayoutInflater().inflate(R.layout.annotation_key_entry, null);
                            sdTitle.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));

                            TextView sdTitleText = (TextView) sdTitle.findViewById(R.id.annotation_key_entry_name);
                            sdTitleText.setText(sd.getName());

                            ((ImageView) sdTitle.findViewById(R.id.annotation_key_entry_add)).setOnTouchListener(new View.OnTouchListener() {
                                @Override
                                public boolean onTouch(View view, MotionEvent motionEvent) {
                                    if (motionEvent.getAction() == MotionEvent.ACTION_DOWN) {
                                        m_model.pendingAnnotation(sd);
                                        return true;
                                    }
                                    return false;
                                }
                            });

                            //Add the SubDataset title
                            Tree<View> sdTitleTree = new Tree<View>(sdTitle);
                            m_subDatasetTrees.put(sd, sdTitleTree);
                            titleTree.addChild(sdTitleTree, -1);
                        }
                    }

                    onSetVisibility(metaData, metaData.getVisibility());
                }
                m_datasetTrees.put(d, titleTree);
                t.addChild(titleTree, -1);
            }
        });
    }

    @Override
    public void onAddAnnotation(ApplicationModel model, AnnotationData annot, ApplicationModel.AnnotationMetaData metaData)
    { }

    @Override
    public void onPendingAnnotation(ApplicationModel model, SubDataset sd)
    {
        m_pendingView.setVisibility(View.VISIBLE);
        m_annotView.setVisibility(View.GONE);
        m_annotDrawButtonsView.setVisibility(View.GONE);
    }

    @Override
    public void onEndPendingAnnotation(ApplicationModel model, SubDataset sd, boolean cancel)
    {
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

    /** Set up the main layout
     * @param v the main view containing all the Widgets*/
    private void setUpMainLayout(View v)
    {
        m_previews  = (TreeView)v.findViewById(R.id.annotPreviewLayout);
        m_annotView = (AnnotationView)v.findViewById(R.id.annotView);
        m_annotView.setModel(null); //For the moment put it at null: we cannot draw anything (because no subdataset yet)
        m_strokeParamLayout = (LinearLayout)v.findViewById(R.id.annotationStrokeParamLayout);
        m_textParamLayout   = (LinearLayout)v.findViewById(R.id.annotationTextParamLayout);
        m_pendingView = v.findViewById(R.id.annotPendingView);
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
                    AnnotationData.AnnotationMode mode = m_mode;
                    int visibility = m_strokeParamLayout.getVisibility();
                    setMode(AnnotationData.AnnotationMode.STROKE);

                    //If reselected, toggle the visibility
                    if(mode == AnnotationData.AnnotationMode.STROKE)
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
                    AnnotationData.AnnotationMode mode = m_mode;
                    int visibility = m_textParamLayout.getVisibility();
                    setMode(AnnotationData.AnnotationMode.TEXT);

                    //If reselected, toggle the visibility
                    if(mode == AnnotationData.AnnotationMode.TEXT)
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
    private void setMode(AnnotationData.AnnotationMode mode)
    {
        if(mode == AnnotationData.AnnotationMode.STROKE)
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
        else if(mode == AnnotationData.AnnotationMode.TEXT)
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
    private void updateBitmap(final AnnotationData data)
    {
        AnnotationData savedModel = m_annotView.getModel(); //Save the last model
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
        for(AnnotationData key : m_bitmaps.keySet())
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
        for(AnnotationData key : m_bitmaps.keySet())
            for(AnnotationText t : key.getTexts())
            {
                if(t == text)
                {
                    updateBitmap(key);
                    return;
                }
            }
    }

    private void changeCurrentAnnotation(ImageView snapImg, AnnotationData annotation)
    {
        //Eveything to default
        for(Map.Entry<AnnotationData, AnnotationBitmap> bmp : m_bitmaps.entrySet())
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
    public void onClampingChange(SubDataset sd, float min, float max) {}

    @Override
    public void onRotationEvent(SubDataset dataset, float[] quaternion) {}

    @Override
    public void onPositionEvent(SubDataset dataset, float[] position) {}

    @Override
    public void onScaleEvent(SubDataset dataset, float[] scale) {}

    @Override
    public void onSnapshotEvent(SubDataset dataset, Bitmap snapshot) {}

    @Override
    public void onAddAnnotation(final SubDataset dataset, final AnnotationData annotation)
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
        m_annotationTrees.put(annotation, annotTree);

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
    }

    @Override
    public void onRemoveAnnotation(SubDataset dataset, AnnotationData annot)
    {
        if(annot == m_annotView.getModel())
            m_annotView.setModel(null);

        if(m_bitmaps.containsKey(annot))
            m_bitmaps.remove(annot);

        if(m_annotationTrees.containsKey(annot))
        {
            m_annotationTrees.get(annot).setParent(null, -1);
            m_annotationTrees.remove(annot);
        }
    }

    @Override
    public void onSetVisibility(SubDatasetMetaData dataset, int visibility)
    {
        SubDataset sdRemove = null;
        SubDataset sd       = null;
        if(visibility == SubDataset.VISIBILITY_PUBLIC)
        {
            sd = dataset.getPublicState();
            sdRemove = dataset.getPrivateState();
        }
        else
        {
            sd = dataset.getPrivateState();
            sdRemove = dataset.getPublicState();
        }

        //Switch the view
        if(m_subDatasetTrees.containsKey(sdRemove))
            m_subDatasetTrees.get(sdRemove).value.setVisibility(View.GONE);
        if(m_subDatasetTrees.containsKey(sd))
            m_subDatasetTrees.get(sd).value.setVisibility(View.VISIBLE);
    }

    @Override
    public void onAddStroke(AnnotationData data, AnnotationStroke stroke)
    {
        stroke.addListener(this);
        stroke.setColor(m_currentStrokeColor);
    }

    @Override
    public void onAddText(AnnotationData data, AnnotationText text)
    {
        text.setColor(m_currentTextColor);
        text.addListener(this);
    }

    @Override
    public void onAddImage(AnnotationData data)
    {
        updateBitmap(data);
    }

    @Override
    public void onSetMode(AnnotationData data, AnnotationData.AnnotationMode mode)
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
