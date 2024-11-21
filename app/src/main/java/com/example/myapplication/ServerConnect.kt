package com.example.myapplication
import android.content.Context
import android.net.wifi.WifiManager
import android.os.StrictMode
import android.text.format.Formatter.formatIpAddress
import java.net.InetAddress
import java.net.Socket
import java.util.*


class ServerConnect() {


    val serverAddress = getMyServerAddress()
    val serverPort = 5930
    //TODO: add some error handling
    val socket = getMySocket()

    private fun getMyServerAddress() : InetAddress {
        val policy = StrictMode.ThreadPolicy.Builder().permitAll().build()
        StrictMode.setThreadPolicy(policy);

        //NOTE: When testing on the local NETWORK, you must rebuild the project with
        // the server's address hardcoded here.
        return InetAddress.getByName("10.0.2.2")     //https://stackoverflow.com/questions/5806220/how-to-connect-to-my-http-localhost-web-server-from-android-emulator
    }

    private fun getMySocket() : Socket {
        val policy = StrictMode.ThreadPolicy.Builder().permitAll().build()
        StrictMode.setThreadPolicy(policy)

        return Socket(serverAddress, serverPort)
    }
}