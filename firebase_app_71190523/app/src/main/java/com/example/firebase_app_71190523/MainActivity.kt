package com.example.firebase_app_71190523

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.FirebaseDatabase
import com.google.firebase.database.ValueEventListener
import com.google.firebase.ktx.Firebase
import java.lang.StringBuilder

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val database = FirebaseDatabase.getInstance("https://fir-71190523-default-rtdb.asia-southeast1.firebasedatabase.app/").reference
        var txtTouch = findViewById<TextView>(R.id.txtTouch)
        var txtAngle = findViewById<TextView>(R.id.txtAngle)
        var btnTouch = findViewById<Button>(R.id.btnTouch)

        btnTouch.setOnClickListener {
            if(txtTouch.text.equals("0")){
                database.child("Node1/touch").setValue(1)
            }else{
                database.child("Node1/touch").setValue(0)
            }
        }
        var getData = object : ValueEventListener{
            override fun onDataChange(snapshot: DataSnapshot) {
                var touch = snapshot.child("Node1/touch").getValue().toString()
                if(touch.equals("0")){
                    txtTouch.setText("0")
                }else{
                    txtTouch.setText("1")
                }
                var angle = snapshot.child("Node1/angle").getValue().toString()
                txtAngle.setText(angle)
            }

            override fun onCancelled(error: DatabaseError) {
                TODO("Not yet implemented")
            }
        }
        database.addValueEventListener(getData)
        database.addListenerForSingleValueEvent(getData)
    }
}
