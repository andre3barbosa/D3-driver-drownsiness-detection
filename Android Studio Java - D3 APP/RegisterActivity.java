package com.example.d3final;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.android.volley.AuthFailureError;
import com.android.volley.Request;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

public class RegisterActivity extends AppCompatActivity {
    //Buttons
    Button registerButton;

    EditText editUser,editEmail,editPass;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register);

        registerButton=findViewById(R.id.buttonReg);

        editUser=findViewById(R.id.editTextUser);
        editEmail=findViewById(R.id.editTextEmail);
        editPass=findViewById(R.id.editTextPass);

        //If user press register button
        registerButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //register user to server
                registerUser();
                //Intent intent = new Intent(RegisterActivity.this,LoginActivity.class);
                //startActivity(intent);
            }
        });

    }
    private void registerUser() {
        final String username = editUser.getText().toString().trim();
        final String email = editEmail.getText().toString().trim();
        final String password = editPass.getText().toString().trim();

        //final String gender = ((RadioButton) findViewById(radioGroupGender.getCheckedRadioButtonId())).getText().toString();

        //first we will do the validations

        if (TextUtils.isEmpty(username)) {
            editUser.setError("Please enter username");
            editUser.requestFocus();
            return;
        }

        if (TextUtils.isEmpty(email)) {
            editEmail.setError("Please enter your email");
            editEmail.requestFocus();
            return;
        }

        if (!android.util.Patterns.EMAIL_ADDRESS.matcher(email).matches()) {
            editEmail.setError("Enter a valid email");
            editEmail.requestFocus();
            return;
        }

        if (TextUtils.isEmpty(password)) {
            editPass.setError("Enter a password");
            editPass.requestFocus();
            return;
        }
        //string request
        StringRequest stringRequest = new StringRequest(Request.Method.POST, URLs.URL_REGISTER,
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
                                startActivity(new Intent(getApplicationContext(), LoginActivity.class));
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
                Map<String, String> params = new HashMap<>();
                params.put("username", username);
                params.put("email", email);
                params.put("password", password);
                //params.put("gender", gender);
                return params;
            }
        };

        VolleySingleton.getInstance(this).addToRequestQueue(stringRequest);
    }
}