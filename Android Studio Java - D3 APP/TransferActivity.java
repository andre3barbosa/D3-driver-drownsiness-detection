package com.example.d3final;

import androidx.activity.result.ActivityResult;
import androidx.activity.result.ActivityResultCallback;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.android.volley.AuthFailureError;
import com.android.volley.Request;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.Charset;
import java.util.HashMap;
import java.util.Map;

public class TransferActivity extends AppCompatActivity {
    TextView logOutText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_transfer);


        logOutText = findViewById(R.id.textView3);  //initialize

        User tempUser = SharedPrefmanager.getInstance(getApplicationContext()).getUser();
        Toast.makeText(getApplicationContext(), "User name" + tempUser.getUsername(), Toast.LENGTH_SHORT).show();

        //if logout text i pressed by the user
        logOutText.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //start log in activity
                Intent intent = new Intent(TransferActivity.this,LoginActivity.class);
                startActivity(intent);
            }
        });

    }





    ActivityResultLauncher<Intent> sActivityResultLauncher = registerForActivityResult(
            new ActivityResultContracts.StartActivityForResult(),
            new ActivityResultCallback<ActivityResult>() {
                @Override
                public void onActivityResult(ActivityResult result) {
                    if(result.getResultCode() == Activity.RESULT_OK){
                        Intent data = result.getData();
                        Uri uri = data.getData();
                        TripData trip_data = getBytesFromUri(getApplicationContext(),uri);
                        Log.i("Trip data","Time: "+ trip_data.getTrip_time()+ " and Number of alerts: "+trip_data.getNumber_alert());
                        sendToDatabase(trip_data);
                    }
                }
            }
    );
    //send the values from excel to database
    private void sendToDatabase(TripData tripAux) {

//string request
        StringRequest stringRequest = new StringRequest(Request.Method.POST, URLs.URL_UP,
                new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {
                        //progressBar.setVisibility(View.GONE);
                        //get the response - the JsonObject
                        try {
                            //converting response to json object
                            JSONObject obj = new JSONObject(response);
                            //if no error in response
                            if (!obj.getBoolean("error")) {
                                Toast.makeText(getApplicationContext(), obj.getString("message"), Toast.LENGTH_SHORT).show();

                                //getting the user from the response
                                /*JSONObject userJson = obj.getJSONObject("user");

                                //creating a new user object
                                User user = new User(
                                        userJson.getInt("id"),
                                        userJson.getString("username"),
                                        userJson.getString("email")
                                        //userJson.getString("gender")
                                );*/

                                //storing the user in shared preferences
                                //SharedPrefmanager.getInstance(getApplicationContext()).userLogin(user);

                                //starting the profile activity
                                //
                                //Toast.makeText(getApplicationContext(), "Register successful", Toast.LENGTH_SHORT).show();
                                finish();
                                startActivity(new Intent(getApplicationContext(), MainActivity.class));
                            } else {
                                Toast.makeText(getApplicationContext(), obj.getString("message"), Toast.LENGTH_SHORT).show();
                            }
                        } catch (JSONException e) {
                            e.printStackTrace();
                        }
                    }
                },
                new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        Toast.makeText(getApplicationContext(), error.getMessage(), Toast.LENGTH_SHORT).show();
                    }
                }) {
            @Override
            protected Map<String, String> getParams() throws AuthFailureError {
                User tempUser = SharedPrefmanager.getInstance(getApplicationContext()).getUser();
                Map<String, String> params = new HashMap<>();
                params.put("username", tempUser.getUsername());
                params.put("email", tempUser.getEmail());
                params.put("duration", Integer.toString(tripAux.getTrip_time()));
                params.put("numAlerts", Integer.toString(tripAux.getNumber_alert()));
                params.put("userId", Integer.toString(tempUser.getId()));
                return params;
            }
        };
        VolleySingleton.getInstance(this).addToRequestQueue(stringRequest);
    }

    //if user press the select file button
    public void openFileDialog(View view) {
        //(Intent.ACTION_CREATE_DOCUMENT)
        Intent data = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        data.addCategory(Intent.CATEGORY_OPENABLE);
        data.setType("text/*");
        data = Intent.createChooser(data,"Choose a file");
        sActivityResultLauncher.launch(data);   //start activity to select the file
    }

    TripData getBytesFromUri (Context context, Uri uri)
    {
        InputStream iStream = null;
        TripData tempData = null;
        try{
            iStream = context.getContentResolver().openInputStream(uri);
            ByteArrayOutputStream byteBuffer = new ByteArrayOutputStream();
            BufferedReader reader = new BufferedReader(
                    new InputStreamReader(iStream, Charset.forName("UTF-8"))
            );
            String line ;
            try {
                while((line = reader.readLine()) != null){
                    //Split by '-'
                    Log.i("Transfer Activity", "Line:"+line);
                    String[] tokens = line.split(",");

                    //Read the data
                    tempData = new TripData();

                    tempData.setTrip_time(Integer.parseInt(tokens[0]));
                    tempData.setNumber_alert(Integer.parseInt(tokens[1]));
                }
            } catch (IOException e){
                Log.wtf("Transfer Activity", "Error in reading file",e);
                e.printStackTrace();
            }



        } catch(Exception e2) {
            Toast.makeText(getBaseContext(), "Can't read from file", Toast.LENGTH_SHORT).show();

            e2.printStackTrace();
        }
        return tempData;
    }
}