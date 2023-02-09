package com.example.d3final;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;




public class SplashActivity extends AppCompatActivity {

    public static final int REQUEST_CODE_PERMISSIONS = 1;

    String[] permissions = {Manifest.permission.BLUETOOTH_CONNECT, Manifest.permission.BLUETOOTH};

    Button getStart;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);

        getStart = findViewById(R.id.buttonStarted);

        /*if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, permissions, REQUEST_CODE_PERMISSIONS);
            Log.e("Permission", "Permission denied.");
        }*/


        getStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(SplashActivity.this,MainActivity.class);
                startActivity(intent);
                finish();
            }
        });

    }
}