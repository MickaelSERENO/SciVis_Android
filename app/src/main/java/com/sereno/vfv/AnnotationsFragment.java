package com.sereno.vfv;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Point;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.sereno.Tree;
import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.BinaryDataset;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Data.VTKDataset;
import com.sereno.view.AnnotationData;
import com.sereno.view.AnnotationStroke;
import com.sereno.view.AnnotationText;
import com.sereno.view.AnnotationView;
import com.sereno.view.TreeView;

import java.util.HashMap;
import java.util.Map;

public class AnnotationsFragment extends VFVFragment implements ApplicationModel.IDataCallback, AnnotationData.IAnnotationDataListener, AnnotationStroke.IAnnotationStrokeListener, SubDataset.ISubDatasetListener
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

    /** The image view text mode*/
    private ImageView m_textMode;

    /** The color parameters image view*/
    private ImageView m_colorParam;

    /** The import images image view*/
    private ImageView m_imageImport;

    private Drawable m_defaultImageViewBackground;

    /** The bitmap showing the content of the annotations*/
    private HashMap<AnnotationData, AnnotationBitmap> m_bitmaps = new HashMap<>();

    /** The trees of SubDataset*/
    private HashMap<SubDataset, Tree<View>> m_subDatasetTrees = new HashMap<>();

    /** The current Drawing mode*/
    private AnnotationData.AnnotationMode m_mode = AnnotationData.AnnotationMode.STROKE;

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
        m_model = model;
        m_model.addListener(this);

        //Call the callback functions
        for(BinaryDataset d : model.getBinaryDatasets())
            onAddBinaryDataset(m_model, d);
        for(VTKDataset d : model.getVTKDatasets())
            onAddVTKDataset(m_model, d);
    }

    @Override
    public void onAddBinaryDataset(ApplicationModel model, BinaryDataset d)
    {
    }

    @Override
    public void onAddVTKDataset(ApplicationModel model, final VTKDataset d)
    {
        final Tree<View> t = m_previews.getModel();

        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                //Add the dataset title
                TextView title = new TextView(getContext());
                title.setText(d.getName());
                Tree<View> titleTree = new Tree<View>(title);

                //Add each subdataset
                for(int i = 0; i < d.getNbSubDataset(); i++)
                {
                    final SubDataset sd = d.getSubDataset(i);
                    sd.addListener(AnnotationsFragment.this);
                    View sdTitle = getActivity().getLayoutInflater().inflate(R.layout.annotation_key_entry, null);
                    ((TextView)sdTitle.findViewById(R.id.annotation_key_entry_name)).setText(sd.getName());
                    ((ImageView)sdTitle.findViewById(R.id.annotation_key_entry_add)).setOnTouchListener(new View.OnTouchListener() {
                        @Override
                        public boolean onTouch(View view, MotionEvent motionEvent) {
                            if(motionEvent.getAction() == MotionEvent.ACTION_DOWN)
                            {
                                addNewAnnotation(sd);
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
                t.addChild(titleTree, -1);
            }
        });
    }

    /** Set up the main layout
     * @param v the main view containing all the Widgets*/
    private void setUpMainLayout(View v)
    {
        m_previews  = (TreeView)v.findViewById(R.id.annotPreviewLayout);
        m_annotView = (AnnotationView)v.findViewById(R.id.annotView);
        m_annotView.setModel(null); //For the moment put it at null: we cannot draw anything (because no subdataset yet)

        m_annotView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
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
                return false;
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
                    setMode(AnnotationData.AnnotationMode.STROKE);
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
                    setMode(AnnotationData.AnnotationMode.TEXT);
                    return true;
                }
                return false;
            }
        });

        m_defaultImageViewBackground = m_colorParam.getBackground();
    }

    /** Set the image views button backgrounds to TRANSPARENT*/
    private void setImageViewBackgrounds()
    {
        m_colorParam.setBackground(m_defaultImageViewBackground);
        m_textMode.setBackground(m_defaultImageViewBackground);
        m_imageImport.setBackground(m_defaultImageViewBackground);
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
                }
            });
        }

        m_mode = mode;
        if(m_annotView.getModel() != null && m_annotView.getModel().getMode() != mode) //Not fire for nothing
            m_annotView.getModel().setMode(mode);
    }

    private void addNewAnnotation(SubDataset sd)
    {
        AnnotationData data = new AnnotationData();
        data.setMode(m_mode); //Set to the current mode
        sd.addAnnotation(data);
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
                    float width = stroke.getWidth();
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

    @Override
    public void onRangeColorChange(SubDataset sd, float min, float max, int mode) {}

    @Override
    public void onRotationEvent(SubDataset dataset, float dRoll, float dPitch, float dYaw) {}

    @Override
    public void onRotationEvent(SubDataset dataset, float[] quaternion) {}

    @Override
    public void onSnapshotEvent(SubDataset dataset, Bitmap snapshot) {}

    @Override
    public void onAddAnnotation(final SubDataset dataset, final AnnotationData annotation)
    {
        annotation.addListener(this);

        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
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
                tree.addChild(new Tree<View>(snapImg), -1);

                //If no annotation yet added
                if(m_annotView.getModel() == null)
                    changeCurrentAnnotation(snapImg, annotation);
            }
        });
    }

    @Override
    public void onAddStroke(AnnotationData data, AnnotationStroke stroke)
    {
        stroke.addListener(this);
    }

    @Override
    public void onAddText(AnnotationData data, AnnotationText text)
    {
        updateBitmap(data);
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
}
