package com.example.myapplication;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresPermission;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    private static final int REQUEST_ENABLE_BT = 1;
    private static final int REQUEST_BLUETOOTH_PERMISSIONS = 2;

    private BluetoothAdapter bluetoothAdapter;
    private BluetoothSocket bluetoothSocket;
    private BluetoothDevice arduinoDevice;
    private OutputStream outputStream;

    // Change this to match your Arduino module's UUID (HC-05 typically uses this)
    private static final UUID ARDUINO_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private static final String ARDUINO_NAME = "HC-05"; // or HC-06 or your custom name

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btnConnect = findViewById(R.id.main);
        Button btnSend = findViewById(R.id.main);

        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth not supported", Toast.LENGTH_LONG).show();
            finish();
            return;
        }

        btnConnect.setOnClickListener(view -> {
            if (!bluetoothAdapter.isEnabled()) {
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                    // TODO: Consider calling
                    //    ActivityCompat#requestPermissions
                    // here to request the missing permissions, and then overriding
                    //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                    //                                          int[] grantResults)
                    // to handle the case where the user grants the permission. See the documentation
                    // for ActivityCompat#requestPermissions for more details.
                    return;
                }
                startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            } else {
                checkPermissionsAndConnect();
            }
        });

        btnSend.setOnClickListener(view -> sendData("Hello Arduino!"));
    }

    private void checkPermissionsAndConnect() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, new String[]{
                        Manifest.permission.BLUETOOTH_CONNECT,
                        Manifest.permission.BLUETOOTH_SCAN
                }, REQUEST_BLUETOOTH_PERMISSIONS);
                return;
            }
        }
        connectToArduino();
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    private void connectToArduino() {
        Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();
        for (BluetoothDevice device : pairedDevices) {
            if (device.getName().equals(ARDUINO_NAME)) {
                arduinoDevice = device;
                break;
            }
        }

        if (arduinoDevice == null) {
            Toast.makeText(this, "Arduino device not found", Toast.LENGTH_SHORT).show();
            return;
        }

        new Thread(() -> {
            try {
                bluetoothSocket = arduinoDevice.createRfcommSocketToServiceRecord(ARDUINO_UUID);
                bluetoothSocket.connect();
                outputStream = bluetoothSocket.getOutputStream();
                runOnUiThread(() -> Toast.makeText(this, "Connected to Arduino", Toast.LENGTH_SHORT).show());
            } catch (IOException e) {
                runOnUiThread(() -> Toast.makeText(this, "Connection failed", Toast.LENGTH_SHORT).show());
            }
        }).start();
    }

    private void sendData(String message) {
        if (outputStream != null) {
            try {
                outputStream.write(message.getBytes());
                Toast.makeText(this, "Data sent", Toast.LENGTH_SHORT).show();
            } catch (IOException e) {
                Toast.makeText(this, "Failed to send data", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Not connected", Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        try {
            if (bluetoothSocket != null) bluetoothSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Permission result handler
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_BLUETOOTH_PERMISSIONS) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                    // TODO: Consider calling
                    //    ActivityCompat#requestPermissions
                    // here to request the missing permissions, and then overriding
                    //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                    //                                          int[] grantResults)
                    // to handle the case where the user grants the permission. See the documentation
                    // for ActivityCompat#requestPermissions for more details.
                    return;
                }
                connectToArduino();
            } else {
                Toast.makeText(this, "Bluetooth permission denied", Toast.LENGTH_SHORT).show();
            }
        }
    }
}
