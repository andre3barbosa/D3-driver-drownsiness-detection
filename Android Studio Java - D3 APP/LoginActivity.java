package com.example.d3final;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.RadioButton;
import android.widget.TextView;
import android.widget.Toast;

import com.android.volley.AuthFailureError;
import com.android.volley.Request;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class LoginActivity extends AppCompatActivity {
    ImageButton backButton;

    Button loginButton;
    EditText emailText,passwordText;
    TextView signUpText;


    int i=0;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        /*Find the id of components*/
        backButton=findViewById(R.id.imageButton2);  //initialization of button
        loginButton=findViewById(R.id.buttonLogin);  //initialization of button
        emailText=findViewById(R.id.editTextTextEmailAddress);
        passwordText=findViewById(R.id.editTextTextPassword);
        signUpText=findViewById(R.id.textView2);

        //if the user is already logged in we will directly start the profile activity



        backButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(LoginActivity.this,MainActivity.class);
                startActivity(intent);
            }
        });

        loginButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //attempt to login
                    userLogin();


            }
        });

        //If user press the sign up textview
        signUpText.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //start register activity
                Intent intent = new Intent(LoginActivity.this,RegisterActivity.class);
                startActivity(intent);
            }
        });

    }
    private void userLogin() {
        //first getting the values
        //final String username = .getText().toString();
        //final String password = editTextPassword.getText().toString();
        final String email = emailText.getText().toString();
        final String password = passwordText.getText().toString();
        //validating inputs
        if (TextUtils.isEmpty(email)) {
            emailText.setError("Please enter your username");
            emailText.requestFocus();
            return;
        }

        if (TextUtils.isEmpty(password)) {
            passwordText.setError("Please enter your password");
            passwordText.requestFocus();
            return;
        }

        //if everything is fine
        StringRequest stringRequest = new StringRequest(Request.Method.POST, URLs.URL_LOGIN, new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {
                        //progressBar.setVisibility(View.GONE);

                        try {
                            //converting response to json object
                            JSONObject obj = new JSONObject(response);

                            //if no error in response
                            if (!obj.getBoolean("error")) {
                                //Toast.makeText(getApplicationContext(), obj.getString("message"), Toast.LENGTH_SHORT).show();

                                //getting the user from the response
                                //JSONObject userJson = obj.getJSONObject("message");

                                //creating a new user object
                                User user = new User(
                                        obj.getInt("id"),
                                        obj.getString("username"),
                                        obj.getString("email")
                                );

                                //storing the user in shared preferences
                                SharedPrefmanager.getInstance(getApplicationContext()).userLogin(user);

                                //starting the profile activity
                                finish();
                                startActivity(new Intent(getApplicationContext(), TransferActivity.class));
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
                params.put("email", email);
                params.put("password", password);
                return params;
            }
        };

        VolleySingleton.getInstance(this).addToRequestQueue(stringRequest);
    }
}


