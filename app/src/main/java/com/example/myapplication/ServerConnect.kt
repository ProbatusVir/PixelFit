package com.example.myapplication
import android.os.StrictMode
import java.io.IOException
import java.net.InetAddress
import java.net.Socket

class ServerConnect() {


    private val serverName = "10.0.2.2"
    private val serverPort = 5930

    private val serverAddress = getMyServerAddress()
    //TODO: add some error handling
    val socket : Socket? = getMySocket()

    private fun getMyServerAddress() : InetAddress {
        val policy = StrictMode.ThreadPolicy.Builder().permitAll().build()
        StrictMode.setThreadPolicy(policy);

        return InetAddress.getByName(serverName)     //https://stackoverflow.com/questions/5806220/how-to-connect-to-my-http-localhost-web-server-from-android-emulator
    }

    private fun getMySocket() : Socket? {
        val policy = StrictMode.ThreadPolicy.Builder().permitAll().build()
        StrictMode.setThreadPolicy(policy)
        var sock : Socket? = null

        Thread {
            try {
                sock = Socket(serverAddress, serverPort)
            }
            finally {
                println("Error Creating Socket")
            }
        }.start()

        return sock
    }
}