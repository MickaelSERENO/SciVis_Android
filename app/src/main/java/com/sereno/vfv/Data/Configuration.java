package com.sereno.vfv.Data;

import android.util.Log;

import com.sereno.vfv.MainActivity;

import java.io.File;
import java.io.FileInputStream;
import java.nio.charset.StandardCharsets;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;


public class Configuration
{
    private String m_serverIP;
    private int    m_serverPort;

    public Configuration()
    {
        m_serverIP   = "127.0.0.1";
        m_serverPort = 8000;
    }

    public Configuration(File f)
    {
        this();

        try
        {
            //Read the file
            FileInputStream fis = new FileInputStream(f);
            byte[] jsonData = new byte[(int) f.length()];
            fis.read(jsonData);
            fis.close();

            //Parse it
            JSONObject reader = new JSONObject(new String(jsonData, StandardCharsets.UTF_8));

            //Parse Network
            try
            {
                JSONObject ntwk = reader.getJSONObject("network");
                m_serverIP = ntwk.getString("ip");
                m_serverPort = ntwk.getInt("port");
            }
            catch (final JSONException e)
            {
                Log.e(MainActivity.TAG, "Error at parsing network configuration");
            }
        }
        catch(Exception e)
        {
            return;
        }
    }

    public String getServerIP()
    {
        return m_serverIP;
    }

    public int getServerPort()
    {
        return m_serverPort;
    }
}
