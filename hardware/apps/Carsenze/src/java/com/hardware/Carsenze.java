package com.hardware;

import android.app.Activity;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Handler;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import vendor.hardware.carsenze.ICarsenze;

public class Carsenze extends Activity {
    private static final String TAG = "Carsenze_System_APP";
    private static final String ICARSENZE_AIDL_INTERFACE = "vendor.hardware.carsenze.ICarsenze/default";
    private static ICarsenze carsenzeAJ; // AIDL Java
    
    private Handler handler = new Handler();
    private Runnable updateRunnable = new Runnable() {
        @Override
        public void run() {

            Log.d(TAG,"**-Requesting for Network Information-**");
            try{
                if(carsenzeAJ!=null){
            String networkStats = carsenzeAJ.getNetworkStats();
            TextView networkTextView = findViewById(R.id.network);
            networkTextView.setText("Network: " + networkStats);
                }
            } catch (RemoteException e) {
            Log.e(TAG, "Error retrieving stats from ICarsenze AIDL service", e);
            }
            
            handler.postDelayed(this, 5000); // Schedule next update in 5 seconds
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btn = findViewById(R.id.button);
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                updateStats();
            }
        });

        // Attempt to bind to the AIDL service
        IBinder binder = ServiceManager.getService(ICARSENZE_AIDL_INTERFACE);
        if (binder == null) {
            Log.e(TAG, "Getting " + ICARSENZE_AIDL_INTERFACE + " service daemon binder failed!");
        } else {
            carsenzeAJ = ICarsenze.Stub.asInterface(binder);
            if (carsenzeAJ == null) {
                Log.e(TAG, "Getting ICarsenze AIDL daemon interface failed!");
            } else {
                Log.d(TAG, "ICarsenze AIDL daemon interface is binded!");
            }
        }
        if(carsenzeAJ!=null){
        // Update stats initially
        updateStats();
        }
        // Start periodic updates
        handler.post(updateRunnable);
    }

    private void updateStats() {
        try {
            // Retrieve stats from AIDL service
            Log.d(TAG,"**-Requesting for CPU Information-**");
            String cpuStats = carsenzeAJ.getCpuStats();
            Log.d(TAG,"**-Requesting for Memory Information-**");
            String memoryStats = carsenzeAJ.getMemoryStats();
            Log.d(TAG,"**-Requesting for Network Information-**");
            String networkStats = carsenzeAJ.getNetworkStats();

            // Update TextViews with retrieved stats
            TextView cpuTextView = findViewById(R.id.cpu);
            cpuTextView.setText(cpuStats);

            TextView memoryTextView = findViewById(R.id.memory);
            memoryTextView.setText("Memory: \n" + memoryStats);

            TextView networkTextView = findViewById(R.id.network);
            networkTextView.setText("Network: " + networkStats);

        } catch (RemoteException e) {
            Log.e(TAG, "Error retrieving stats from ICarsenze AIDL service", e);
        }
    }
}
