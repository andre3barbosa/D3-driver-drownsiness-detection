package com.example.d3final;

import static androidx.constraintlayout.helper.widget.MotionEffect.TAG;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.ParcelUuid;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Objects;
import java.util.Set;
import java.util.UUID;

@RequiresApi(api = Build.VERSION_CODES.S)
public class TripActivity extends AppCompatActivity {

    Button buttonStop;
    Button buttonCancel;

    BluetoothAdapter mBluetoothAdapter;

    private static String myAddress = "E4:5F:01:41:AB:69";  //rasp

    public static final int REQUEST_CODE_PERMISSIONS = 1;

    String[] permissions = {Manifest.permission.BLUETOOTH_CONNECT, Manifest.permission.BLUETOOTH};

    /*+++++++++++++++++++++++++++++++++++++++++++++++++*/
    //Get the instance of my bluetooth object
    BluetoothSingleton myBt = BluetoothSingleton.getInstance();
    short error = 0;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_trip);


        //Log.e("Bluetooth", "Bluetooth status:"+ myBt.getE() + " ");
        buttonStop = findViewById(R.id.buttonstop);     //initialization of button stop
        buttonCancel = findViewById(R.id.buttoncancel); //initialization of button cancel

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        buttonStop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                error = myBt.writeToLocal("Terminate Trip");
                //Log.e("Bluetooth", "Bluetooth write:"+ error + " ");
                Intent intent = new Intent(TripActivity.this, SyncActivity.class);
                startActivity(intent);
            }
        });

        buttonCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                error = myBt.writeToLocal("Cancel trip");
                //Log.e("Bluetooth", "Bluetooth write:"+ error + " ");
                Intent intent = new Intent(TripActivity.this, MainActivity.class);
                startActivity(intent);
            }
        });
    }

    private void errorExit(String title, String message) {
        Toast msg = Toast.makeText(getBaseContext(), title + " - " + message, Toast.LENGTH_SHORT);
        msg.show();
        finish();
    }

    private void sendDataToPairedDevice(String message, BluetoothDevice device) {
        byte[] toSend = message.getBytes();
        try {
            UUID applicationUUID = UUID.fromString("8ce255c0-200a-11e0-ac64-0800200c9a66");
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                // TODO: Consider calling
                //    ActivityCompat#requestPermissions
                // here to request the missing permissions, and then overriding
                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                //                                          int[] grantResults)
                // to handle the case where the user grants the permission. See the documentation
                // for ActivityCompat#requestPermissions for more details.
                ActivityCompat.requestPermissions(this, permissions, REQUEST_CODE_PERMISSIONS);
                Log.e("Permission", "Permission denied.");
                //return;
            }
            BluetoothSocket socket = device.createInsecureRfcommSocketToServiceRecord(applicationUUID);
            OutputStream mmOutStream = socket.getOutputStream();
            mmOutStream.write(message.getBytes());
            // Your Data is sent to  BT connected paired device ENJOY.
        } catch (IOException e) {
            Log.e(TAG, "Exception during write", e);
        }
    }

    private OutputStream outputStream;
    private InputStream inStream;

    Set<BluetoothDevice> pairedDevices;


    private void init() throws IOException {
        BluetoothAdapter blueAdapter = BluetoothAdapter.getDefaultAdapter();
        if (blueAdapter != null) {
            if (blueAdapter.isEnabled()) {

                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                    // TODO: Consider calling
                    //    ActivityCompat#requestPermissions
                    // here to request the missing permissions, and then overriding
                    //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                    //                                          int[] grantResults)
                    // to handle the case where the user grants the permission. See the documentation
                    // for ActivityCompat#requestPermissions for more details.
                    ActivityCompat.requestPermissions(this, permissions, REQUEST_CODE_PERMISSIONS);
                    Log.e("Permission", "Permission denied.");
                    //return;
                }
                Set<BluetoothDevice> bondedDevices = blueAdapter.getBondedDevices();

                if (bondedDevices.size() > 0) {


                    //ArrayList list = new ArrayList();
                    //BluetoothDevice bt;

                    for (BluetoothDevice bt : bondedDevices)    //scroll every mac address device paired
                    {
                        //Log.d("Adresses", bt.getAddress());
                        if (Objects.equals(bt.getAddress(), myAddress))    //if the device is paired with the target device
                        {
                            Toast.makeText(getApplicationContext(), "Device paired", Toast.LENGTH_SHORT).show();


                            /*ParcelUuid[] uuids = bt.getUuids();
                            BluetoothSocket socket = bt.createRfcommSocketToServiceRecord(uuids[0].getUuid());
                            socket.connect();
                            outputStream = socket.getOutputStream();
                            inStream = socket.getInputStream();*/
                            //onResume();
                        }
                    }

                    Log.e("error", "No appropriate paired devices.");
                } else {
                    Log.e("error", "Bluetooth is disabled.");
                }
            }
        }
    }

    public void run() {
        final int BUFFER_SIZE = 1024;
        byte[] buffer = new byte[BUFFER_SIZE];
        int bytes = 0;
        int b = BUFFER_SIZE;

        while (true) {
            try {
                bytes = inStream.read(buffer, bytes, BUFFER_SIZE - bytes);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }


}