import android.os.StrictMode
import java.io.InputStream
import java.io.OutputStream
import java.net.InetAddress
import java.net.Socket
import java.nio.ByteBuffer
import java.nio.ByteOrder

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

enum class MessageResult(val int : Int) {

    Failed(0),
    LoginSuccess(1),
    Success(2);

    companion object {
        fun fromInt(int : Int) = entries.first {it.int == int}
    }
}


class ServerConnect {


    private val serverAddress = getMyServerAddress()
    //TODO: add some error handling
    private var socket : Socket? = null
    private var inputStream : InputStream? = null
    private var outputStream : OutputStream? = null
    private var token : ByteArray? = null

    private fun getMyServerAddress() : InetAddress {
        val policy = StrictMode.ThreadPolicy.Builder().permitAll().build()
        StrictMode.setThreadPolicy(policy)

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
                Command.SocketError.int -> println("Socket error")
                Command.Failed.int -> println("Server disconnected")
                Command.Login.int -> handleToken()
                Command.GetUsers.int -> {}
                Command.MessageServer.int -> {}
                Command.DiscussionPost.int -> {}
                Command.GetUser.int -> {}
                Command.BanUser.int -> {} //This should never be implemented
                else -> println("Received unexpected command")
            }
        }
    }

    private fun readHeader() : Int {
        val buffer = ByteArray(Int.SIZE_BYTES)
        inputStream?.read(buffer)
        return ByteBuffer.wrap(buffer).order(ENDIAN).getInt()
    }

    private fun messageResult() : MessageResult {
        return MessageResult.fromInt(readHeader())
    }

    private fun handleToken() {
        println(messageResult())
        val bytesToRead = readHeader()

        token = ByteArray(bytesToRead + 1 )
        inputStream?.read(token, 0, bytesToRead)
        println("Your token: $token")

    }

    // I have decided to make this a private function in order to
    // more specific functions for the fragments
    private fun sendToServer(command : Int, message : String)
    {
        val tokenSize = if (token != null)
            HASH_SIZE + 1 else 0

        val lengthOfMessage = message.length + LENGTH_OF_COMMAND_AND_MESSAGE_HEADER

        if (outputStream == null)
            outputStream = socket?.getOutputStream()

        var messageToServer = ByteArray(0)
        //Write command
        messageToServer += ByteBuffer.allocate(Int.SIZE_BYTES).order(ENDIAN).putInt(command).array()

        if (token != null) {
            messageToServer += ByteBuffer.allocate(Int.SIZE_BYTES).order(ENDIAN).putInt(tokenSize).array()
            messageToServer += token!!
        }

            messageToServer += ByteBuffer.allocate(Int.SIZE_BYTES).order(ENDIAN).putInt(lengthOfMessage).array()
            messageToServer += message.toByteArray() + 0x00

        outputStream?.write(messageToServer)
        outputStream?.flush()
    }

    fun signUp(name : String, username : String, email : String, password : String) {
        val message = "$name\n$username\n$email\n$password" + 0.toChar()
        sendToServer(Command.NewUser.int, message)
    }

    init {
        Thread {
            getMySocket()
            inputStream = socket?.getInputStream()
            outputStream = socket?.getOutputStream()
            listenForServer()
        }.start()
    }

    companion object {
        private const val SERVER_NAME = "10.0.2.2"
        private const val PORT = 5930
        private const val HASH_SIZE = 32
        private const val LENGTH_OF_COMMAND_AND_MESSAGE_HEADER = Int.SIZE_BYTES * 3 + 1
        //server endian
        private val ENDIAN = ByteOrder.LITTLE_ENDIAN
    }
}