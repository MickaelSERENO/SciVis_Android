package com.sereno.vfv.Data;

import java.util.ArrayList;

public class DatasetMetadata
{
    public static class PerTimestepMetadata
    {
        public String date;
        public PerTimestepMetadata() {}

        public String toString()
        {
            return date;
        }
    }

    public String                coastline;
    public PerTimestepMetadata[] perTimesteps = new PerTimestepMetadata[0];

    public DatasetMetadata(){}
}
