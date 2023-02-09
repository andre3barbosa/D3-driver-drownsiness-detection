package com.example.d3final;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.provider.DocumentsContract;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.Toast;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Objects;
import java.util.Set;
import java.util.UUID;

@RequiresApi(api = Build.VERSION_CODES.S)
public class SyncActivity extends AppCompatActivity {
    Button returnButton;

    /*+++++++++++++++++++++++++++++++++++++++++++++++++*/
    //Get the instance of my bluetooth object
    BluetoothSingleton myBt = BluetoothSingleton.getInstance();
    String rcvString; //string received by bluetooth

    ActivityResultLauncher<Intent> createInvoiceActivityResultLauncher; //to store the file

    public static final String DEFAULT_NAME = "File_Trip";
    private int saveFile = 0;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sync);

        createInvoiceActivityResultLauncher = registerForActivityResult(
                new ActivityResultContracts.StartActivityForResult(),
                result -> {
                    if (result.getResultCode() == Activity.RESULT_OK) {
                        // There are no request codes
                        Uri uri = null;
                        if (result.getData() != null) {
                            uri = result.getData().getData();
                            createInvoice(uri);
                            finish();
                            Intent intent2 = new Intent(SyncActivity.this,MainActivity.class);
                            startActivity(intent2);
                            // Perform operations on the document using its URI.
                        }
                    }
                });








        returnButton = findViewById(R.id.buttonReturn);


        returnButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(myBt.getRcvFlag()==1)
                {
                    rcvString = myBt.getInputString();
                    createFile(DEFAULT_NAME);
                    myBt.writeToLocal("R:Return to main");
                    myBt.setRcvFlag(0);
                }
                else
                {
                    Toast msg = Toast.makeText(getBaseContext(), "Wait, data is being transfered", Toast.LENGTH_SHORT);
                    msg.show();
                }
            }
        });

    }

    private void createInvoice(Uri uri) {
        try {
            ParcelFileDescriptor pfd = getContentResolver().openFileDescriptor(uri, "w");
            if (pfd != null) {
                FileOutputStream fileOutputStream = new FileOutputStream(pfd.getFileDescriptor());
                fileOutputStream.write(rcvString.getBytes());
                fileOutputStream.close();
                pfd.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    private void createFile(String title) {
        Intent intent = new Intent(Intent.ACTION_CREATE_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("text/csv");
        intent.putExtra(Intent.EXTRA_TITLE, title);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            intent.putExtra(DocumentsContract.EXTRA_INITIAL_URI, Uri.parse("/Documents"));
        }
        createInvoiceActivityResultLauncher.launch(intent);
    }


    @Override
    public void onResume(){
        super.onResume();
        //Store the value of the string received by bluetooth

    }
}





