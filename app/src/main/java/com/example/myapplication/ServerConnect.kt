package com.example.myapplication
import android.os.StrictMode
import androidx.collection.emptyLongSet
import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.lang.Byte
import java.net.InetAddress
import java.net.Socket
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.util.*

enum class Command(val int : Int) {
    SocketError(-1),
    Failed(0),
    Login(1),
    NewUser(2),
    GetUsers(3),
    MessageServer(4),
    DiscussionPost(5),
    GetUser(6),
    BanUser(7),
}
enum class MessageResult {
    Failed,
    LoginSuccess,
    Success,
}


class ServerConnect() {


    private val serverAddress = getMyServerAddress()
    //TODO: add some error handling
    private var socket : Socket? = null
    private var inputStream : InputStream? = null
    private var outputStream : OutputStream? = null
    private var token : ByteArray? = null

    private fun getMyServerAddress() : InetAddress {
        val policy = StrictMode.ThreadPolicy.Builder().permitAll().build()
        StrictMode.setThreadPolicy(policy);

        return InetAddress.getByName(SERVER_NAME)     //https://stackoverflow.com/questions/5806220/how-to-connect-to-my-http-localhost-web-server-from-android-emulator
    }

    private fun getMySocket() {
        val policy = StrictMode.ThreadPolicy.Builder().permitAll().build()
        StrictMode.setThreadPolicy(policy)

        try {
            socket = Socket(serverAddress, PORT)
        }
        finally {
            println("Attempted to make socket")
        }

    }

    private fun listenForServer()
    {
        handleToken()
        while (true)
        {
            val command = readHeader()
            when (command)
            {
                Command.SocketError.int -> error("Socket error")
                Command.Failed.int -> error("Server disconnected")
                Command.Login.int -> handleToken()
                else -> error("Received unexpected command")
            }
        }
    }

    private fun readHeader() : Int {
        val buffer = ByteArray(Int.SIZE_BYTES)
        inputStream?.read(buffer)
        return ByteBuffer.wrap(buffer).order(ENDIAN).getInt()
    }

    private fun handleToken() {
        inputStream?.skip(Int.SIZE_BYTES.toLong())
        val bytesToRead = readHeader()

        token = ByteArray(bytesToRead + 1 )
        inputStream?.read(token, 0, bytesToRead)
        println("Your token: $token")

    }

    //TODO: Remove this
    //  NOTE:
    //      When doing the login, fields are \n delimited

    fun sendToServer(command : Int, message : String)
    {
        val tokenSize = if (token != null)
            HASH_SIZE + 1 else 0

        val lengthOfMessage = message.length + LENGTH_OF_COMMAND_AND_MESSAGE_HEADER
        val messageToServer = ByteArray(1024)
        if (outputStream == null)
            outputStream = socket?.getOutputStream()

        var buffer : ByteArray = ByteArray(0)
        //Write command
        buffer += ByteBuffer.allocate(Int.SIZE_BYTES).order(ENDIAN).putInt(command).array()

        if (token != null) {
            buffer += ByteBuffer.allocate(Int.SIZE_BYTES).order(ENDIAN).putInt(tokenSize).array()
            buffer += token!!
        }

            buffer += ByteBuffer.allocate(Int.SIZE_BYTES).order(ENDIAN).putInt(lengthOfMessage).array()
            buffer += message.toByteArray() + 0x00

        outputStream?.write(buffer)
        outputStream?.flush()
    }

    init {
        Thread {
            getMySocket()
            inputStream = socket?.getInputStream()
            outputStream = socket?.getOutputStream()
            sendToServer(
                Command.NewUser.int,
                "bobby\nBobIsAwesome\n123BOB"
            )

            listenForServer()
        }.start()
    }

    companion object {
        private const val SERVER_NAME = "10.0.2.2"
        private const val PORT = 5930
        private const val HASH_SIZE = 32
        private const val NAME_SIZE = 50
        private const val USERNAME_SIZE = 30
        private const val PASSWORD_SIZE = 60
        private const val LENGTH_OF_COMMAND_AND_MESSAGE_HEADER = Int.SIZE_BYTES * 3 + 1
        //server endian
        private val ENDIAN = ByteOrder.LITTLE_ENDIAN
    }
}