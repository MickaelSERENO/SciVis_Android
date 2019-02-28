package com.sereno.vfv.Network;

/** Basic class representing a ServerMessage (message received from the Server)*/
abstract class ServerMessage
{
    /** The type of this message*/
    int type = -1;

    /** The current cursor*/
    int cursor = 0;

    /** The current type to read.
     * 'i' == short
     * 'I' == int
     * 'f' == float
     * 's' == string
     * @return the type to read
     */
    abstract byte getCurrentType();

    /** Push a short value
     * @param value the value to push*/
    void pushValue(short value) {}

    /** Push a int value
     * @param value the value to push*/
    void pushValue(int value) {}

    /** Push a float value
     * @param value the value to push*/
    void pushValue(float value) {}

    /** Push a string value
     * @param value the value to push*/
    void pushValue(String value) {}

    /** What is the maximum cursor of this message?
     * @return the maximum cursor (included) this message can handle. -1 == no data to push*/
    int getMaxCursor() {return -1;}

    /** Get the type of this Message
     * @return the type of the Message. See MessageBuffer*/
    public int getType() {return type;}
}