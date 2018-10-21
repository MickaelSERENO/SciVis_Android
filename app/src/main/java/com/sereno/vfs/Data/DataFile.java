package com.sereno.vfs.Data;

import android.content.Context;
import android.service.autofill.Dataset;
import android.util.Log;

import java.io.File;
import java.util.ArrayList;

/* \brief class representing a data file. Contain the path and the file name.
 * Can be used with ArrayAdapter. see toString() method
 */
public class DataFile
{
    /* \brief Return in an array all the available datasets
     * \return an array of DataFile containing description of datasets. They are not yet opened.
     * Use parseData on each to parse the data
     * \param ctx the context permitting to read internal storage*/
    public static DataFile[] getAvailableDatasets(Context ctx)
    {
        //Read the data files in externalDir/fluidData. All the files are for this moment valid
        //We may further put some filter.
        File   external      = new File(ctx.getExternalFilesDir(null), "Datas");
        if(external == null || !external.isDirectory())
            return null;

        File[] externalFiles = external.listFiles();
        if(externalFiles == null)
            return null;

        ArrayList<DataFile> files = new ArrayList<>();
        for(File f : externalFiles)
            if(f.isFile())
                files.add(new DataFile(f));

        return (DataFile[])files.toArray(new DataFile[files.size()]);
    }

    private File m_file; /*! The data file*/

    /* \brief Constructor
     * \param path the dataset path file*/
    public DataFile(String path)
    {
        m_file = new File(path);
    }

    /* \brief Constructor
     * \param f the file containing the interesting data*/
    public DataFile(File f)
    {
        m_file = f;
    }

    /* \brief Get the file associated with this data
     * \return the file associated with this data*/
    public File getFile()
    {
        return m_file;
    }

    /* \brief Return the name of the dataseti
     * \return the name of the dataset*/
    @Override
    public String toString()
    {
        if(m_file != null)
            return m_file.getName();
        else
            return "";
    }

    /* \brief Parse the data and save it to a Dataset object
     * \return the parsed dataset*/
    public Dataset parseData()
    {
        //TODO parse the data
        return null;
    }
}
