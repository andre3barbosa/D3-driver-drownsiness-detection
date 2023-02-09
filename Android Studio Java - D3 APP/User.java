package com.example.d3final;

public class User {

    private int id;
    private String username, email;

    public User(int id, String username, String email) {
        this.id = id;
        this.username = username;
        this.email = email;
        //this.gender = gender;
    }

    public int getId() {
        return id;
    }

    public String getUsername(){
        return username;
    }

    public String getEmail(){
        return email;
    }

    /*public String getGender() {
        return gender;
    }*/
}
