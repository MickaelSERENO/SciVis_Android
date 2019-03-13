package com.sereno.vfv.Network;

/** Empty message received*/
public class EmptyMessage extends ServerMessage
{
    @Override
    public byte getCurrentType() {return 0;}

    public int getMaxCursor() {return -1;}
}
