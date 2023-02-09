package com.example.d3final;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;

public class MainActivity extends AppCompatActivity {

    public static final String FILE_NAME = "d3File.jpg";
    private static final int REQUEST_PERMISSION_WRITE = 5;
    private static final int REQUEST_PERMISSION_WRITE_EXTERNAL_STORAGE = 10;
    private static final int MY_PERMISSIONS_REQUEST_WRITE_EXTERNAL_STORAGE = 2;
    private static final int REQUEST_PERMISSION_MANAGE = 3;

    //declare start button
    Button button1;
    Button button2;

    /*+++++++++++++++++++++++++++++++++++++++++++++++++*/

    AlertDialog.Builder builder;    //to alert the user that bluetooth is turned off
    AlertDialog dialog;
    /*+++++++++++++++++++++++++++++++++++++++++++++++++*/

    //BluetoothAdapter mBluetoothAdapter; //bluetooth adapter

    /*+++++++++++++++++++++++++++++++++++++++++++++++++*/
    //Get the instance of my bluetooth object
    BluetoothSingleton myBt = BluetoothSingleton.getInstance();

    //File management
    String inputText = "Hello,Carlos,1";

    File myExternalFile;
    String myData = "";
    private boolean permissionGranted;
    //private PermissionCallback permissionCallback;

    public static final int REQUEST_CODE_PERMISSIONS = 1;

    String[] permissions = {Manifest.permission.BLUETOOTH_CONNECT, Manifest.permission.BLUETOOTH};


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        /*if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, permissions, REQUEST_CODE_PERMISSIONS);
            Log.e("Permission", "Permission denied.");
        }*/

        //myBt.execute();



        //Get instantiation of bluetooth object

        button1=findViewById(R.id.buttonstart);  //initialization of button
        button2=findViewById(R.id.buttontransfer);  //initialization of button
        /*+++++++++++++++++++++++++++++++++++++++++++++++++*/

        //Display a warning to turn on the bluetooth

        builder = new AlertDialog.Builder(this);
        builder.setMessage("Please power on the bluetooth")
                .setTitle("Bluetooth is powered off")
                .setIcon(R.drawable.iconalert);
        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                // User clicked OK button
                dialog.dismiss();

            }
        });
        dialog = builder.create();

        /*+++++++++++++++++++++++++++++++++++++++++++++++++*/


        //When user press Start button
        button1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if (!myBt.bluetoothStatus()) {   //if bluetooth is turn off
                    dialog.show();  //show the alert msg
                }
                else    //if is turned on
                {

                    if(myBt.paired())
                    {
                        if(myBt.writeToLocal("Start trip")==1)
                        {
                            Toast msg = Toast.makeText(getBaseContext(), "Error send char", Toast.LENGTH_SHORT);
                            msg.show();
                        }
                        else {
                            Intent intent = new Intent(MainActivity.this,TripActivity.class);
                            startActivity(intent);
                        }

                    }
                    else
                    {
                        Toast msg = Toast.makeText(getBaseContext(), "Device is not paired", Toast.LENGTH_SHORT);
                        msg.show();
                    }

                }

            }
        });

        //If transfer button is pressed
        button2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //if user was already loggedIn
                if (SharedPrefmanager.getInstance(getApplicationContext()).isLoggedIn()) {
                    finish();
                    startActivity(new Intent(getApplicationContext(), TransferActivity.class));
                    return;
                }
                else {
                    //start the login activity
                    Intent intent = new Intent(MainActivity.this,LoginActivity.class);
                    startActivity(intent);
                }
            }
        });

    }



    /*+++++++++++++++++++++++++++++++++++++*/
    /*+++++++++++++++++++++++++++++++++++++*/

    @Override
    public void onResume(){
        super.onResume();


    }
    @Override
    public void onPause() {
        super.onPause();
        //myBt.closeSocket();

    }



}