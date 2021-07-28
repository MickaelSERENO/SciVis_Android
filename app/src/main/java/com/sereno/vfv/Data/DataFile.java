package com.sereno.vfv.Data;

import android.content.Context;
import android.util.Log;

import com.sereno.vfv.MainActivity;

import java.io.File;
import java.util.ArrayList;

/** Class representing a data file. Contain the path and the file name.
 * Can be used with ArrayAdapter. see toString() method*/
public class DataFile
{
    /** Return in an array all the available datasets (Datas folder)
     * @param ctx the context permitting to read internal storage
     * @return an array of DataFile containing description of datasets. They are not yet opened*/
    public static DataFile[] getAvailableDatasets(Context ctx)
    {
        ArrayList<DataFile> res = getAvailableFileAt(ctx,"Datas");

        //Remove time component (files finishing with digits)
        int i = 0;
        while(i < res.size())
        {
            int j = res.get(i).toString().lastIndexOf('.');
            if(j > 0)
            {
                //Check if composed of digits
                String s = res.get(i).toString().substring(j+1);
                boolean noDigit = false;
                for(int k = 0; k < s.length(); k++)
                {
                    if(!Character.isDigit(s.charAt(k)))
                    {
                        noDigit = true;
                        break;
                    }
                }
                if(noDigit)
                {
                    i++;
                    continue;
                }

                //If yes, remove value i
                res.remove(i);
            }
            else
                i++;
        }
        return(DataFile[])(res.toArray(new DataFile[res.size()]));
    }

    /** Return in an array all the available log information concerning annotations (Logs folder)
     * @param ctx the context permitting to read internal storage
     * @return an array of DataFile containing description of annotations. They are not yet opened*/
    public static DataFile[] getAvailableAnnotationLogs(Context ctx)
    {
        ArrayList<DataFile> res = getAvailableFileAt(ctx,"Logs");
        return(DataFile[])(res.toArray(new DataFile[res.size()]));
    }

    /** Return in an array all the available files in a specific subdirectory
     * @param ctx the context permitting to read internal storage
     * @param subDir the sub directory to look at
     * @return an array of DataFile containing description of the files.*/
    public static ArrayList<DataFile> getAvailableFileAt(Context ctx, String subDir)
    {
        //Read the data files in externalDir/fluidData. All the files are for this moment valid
        //We may further put some filter.
        Log.i(MainActivity.TAG, "External files dir : " + ctx.getExternalFilesDir(null));

        File external      = new File(ctx.getExternalFilesDir(null), subDir);
        if(external == null || !external.isDirectory())
            return null;

        File[] externalFiles = external.listFiles();
        if(externalFiles == null)
            return null;

        ArrayList<DataFile> files = new ArrayList<>();
        for(File f : externalFiles)
            if(f.isFile())
                files.add(new DataFile(f));

        return files;
    }

    private File m_file; /**! The data file*/

    /** Constructor
     * @param path the dataset path file*/
    public DataFile(String path)
    {
        m_file = new File(path);
    }

    /** Constructor
     * @param f the file containing the interesting data*/
    public DataFile(File f)
    {
        m_file = f;
    }

    /** Get the file associated with this data
     * @return the file associated with this data*/
    public File getFile()
    {
        return m_file;
    }

    /** Return the name of the dataseti
     * @return the name of the dataset*/
    @Override
    public String toString()
    {
        if(m_file != null)
            return m_file.getName();
        else
            return "";
    }
}
