package com.example.d3final;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import androidx.core.app.ActivityCompat;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.Objects;
import java.util.Set;
import java.util.UUID;

public class BluetoothSingleton extends AsyncTask<Void, Void, Void> {
    private static BluetoothSingleton _instance;

    //bluetooth adapter
    private BluetoothAdapter mBluetoothAdapter;

    //get the bluetooth paired devices associated
    private Set<BluetoothDevice> pairedDevices;
    //ListView lv;


    private BluetoothSocket btSocket = null;
    private BluetoothSocket btSocketRcv = null;

    private OutputStream outStream = null;
    private InputStream inpStream = null;

    private String inputString;

    private int rcvFlag;

    private static final String myAddress = "E4:5F:01:4E:04:57";
    //private static final String myAddress = "E4:5F:01:41:AB:69";  //rasp
    private short pairedFlag; //this flag tells the user if the target device is paired

    int begin, bytes;
    private short received;
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    //private static final UUID channel4Uuid = UUID.fromString(String.format("0000%04X-0000-1000-8000-00805f9b34fb", 5));
    //UUID channel4Uuid = UUID.randomUUID();

    private static final UUID MY_INS_UUID = UUID.fromString("00001800-0000-1000-8000-00805f9b34fb");

    private byte[] buffer;
    BluetoothDevice device;

    private BluetoothSingleton() {
        pairedFlag = 0;
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        received = 0;
        begin = 0;
        bytes = 0;
        rcvFlag =  0;
        buffer = new byte[256];
        device = mBluetoothAdapter.getRemoteDevice(myAddress);
        //connectSocket();
        //BluetoothAdapter mBluetoothAdapter; //bluetooth adapter
        //execute();
        execute();
    }

    public static BluetoothSingleton getInstance() {
        if (_instance == null) {
            _instance = new BluetoothSingleton();
        }
        return _instance;
    }

    public String getInputString() {
        return inputString;
    }

    public int getRcvFlag() {
        return rcvFlag;
    }

    public void setRcvFlag(int rcvFlag) {
        this.rcvFlag = rcvFlag;
    }

    public short getRcv() {
        return received;
    }

    public short readFromLocal() {
        short error = 0;
        try {
            //read from input stream and return the number of read bytes
            bytes = inpStream.read(buffer);
            //send the value of buffer to the string readMessage
            String readMessage = new String(buffer, 0, bytes);
            Log.i("Read from local","Read from device: " + readMessage);
            //copy the value of local readMessage to the object inputString
            inputString = readMessage;
            //flag to identify that a message was received
            rcvFlag = 1;
        } catch (IOException e) {
            error = 1;
            Log.e("Error","Read from device: ");
        }
        return error;
    }

    /**
     * Sends a string (through serial communication) to the target device
     * ---> Note that char ':' is the escape I chose to end a
     * command with the target device.
     *
     * @param
     *
     */
    public short writeToLocal(String msg) {
        short error = 0;
        if (btSocket != null) {
            try {

                btSocket.getOutputStream().write(msg.getBytes());

            } catch (IOException e) {
                error = 1;
            }
        }
        return error;
    }

    public boolean bluetoothStatus() {
        boolean status = false;
        if (mBluetoothAdapter.isEnabled()) {
            status = true;
        }
        return status;
    }

    public boolean paired() {

        boolean pairStatus = false;
        pairedDevices = mBluetoothAdapter.getBondedDevices();

        for (BluetoothDevice bt : pairedDevices)    //scroll every mac address device paired
        {
            if (Objects.equals(bt.getAddress(), myAddress))    //if the device is paired with the target device
            {
                pairStatus = true;
            }
        }
        return pairStatus;
    }

    public short acceptSocket() {
        short error = 0;
        BluetoothServerSocket btSocketServer = null;
        // Create a pointer to the device by its MAC address
        //pairedFlag = 1;
        try {
            btSocketServer = mBluetoothAdapter.listenUsingInsecureRfcommWithServiceRecord(device.getName(), MY_UUID);
            //btSocketServer = mBluetoothAdapter.listenUsingRfcommWithServiceRecord(device.getName(),MY_UUID);
            //btSocketServer = mBluetoothAdapter.listenUsingRfcommWithServiceRecord(device.getName(),MY_INS_UUID);
        }catch (IOException e)
        {
            error = 1;
            Log.e("Bluetooth","Server socket failed");
        }

        //while(!bluetoothStatus())
        //{
            try {
                Log.i("Bluetooth","Waiting to connect" + device.getName());
                //assert btSocketServer != null;
                btSocketRcv = btSocketServer.accept();
                Log.i("Bluetooth","Connect error");
            }catch (IOException e){
                error = 1;
                Log.e("Bluetooth","failed to accept");
            }

        //}
        Log.i("Bluetooth","Connected");
        // Create a data stream so we can talk to server.
        try {
            inpStream = btSocketRcv.getInputStream();
            //inpStream = btSocket.getInputStream();
        } catch (IOException e) {
            errorExit("Error", "Get input Stream error: " + e.getMessage() + ".");
            error = 1;
            try {
                btSocketRcv.close();
            } catch (IOException e2) {
                errorExit("Error", "Unable to close connection after connection failure: " + e2.getMessage() + ".");
            }
        }


        return error;
    }

    //Connect to socket
    public short connectSocket()
    {
        short error = 0;

        // Create a pointer to the device by its MAC address
        //BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(myAddress);
        //pairedFlag = 1;

        try {
            //btSocket = device.createInsecureRfcommSocketToServiceRecord(MY_UUID);
            btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
            //btSocketRcv = device.createInsecureRfcommSocketToServiceRecord(MY_UUID);
            //btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
        } catch (IOException e) {
            errorExit("Error", "In onResume() happened the following error: " + e.getMessage() + ".");
            error = 1;
        }
        // Resuming the bluetooth connection
        try {

            btSocket.connect();
            //btSocket.wait(30000);
        } catch (IOException e) {
            try {
                btSocket.close();
            } catch (IOException e2) {
                errorExit("Error", "Unable to close connection after connection failure: " + e2.getMessage() + ".");
                error = 1;
            }
        }

        // Create a data stream so we can talk to server.
        try {
            outStream = btSocket.getOutputStream();
            //inpStream = btSocket.getInputStream();
        } catch (IOException e) {
            errorExit("Error", "Stream creation failed: " + e.getMessage() + ".");
            error = 1;
        }

        return error;
    }


    //Close the socket
    public short closeSocket()
    {
        short error = 0;
        if (outStream != null) {
            try {
                outStream.flush();
            } catch (IOException e) {
                errorExit("Fatal Error", "In onPause() and failed to flush output stream: " + e.getMessage() + ".");
                error = 1;
            }
        }
        try {
            btSocket.close();
        } catch (IOException e2) {
            errorExit("Fatal Error", "In onPause() and failed to close socket." + e2.getMessage() + ".");
            error = 1;
        }
        return error;
    }


    /**
     * Create a {@link Toast} to notify user that something went wrong and close
     * the application.
     *
     * @param title
     *            of the error notify
     * @param message
     *            to display
     */
    private void errorExit(String title, String message) {
        //Toast msg = Toast.makeText(getBaseContext(), title + " - " + message, Toast.LENGTH_SHORT);
        //msg.show();
        //finish();
    }


    @Override
    protected Void doInBackground(Void... params) {
        // Perform background task here
        connectSocket();
        acceptSocket();
        while(true)
        {
            if(readFromLocal() == 1)
            {
                Log.e("Bluetooth","Error reading from local");

                break;
            }
            else{
                received = 1;
                Log.i("Bluetooth", "Byte Received");
            }
        }

        return null;
    }


}

