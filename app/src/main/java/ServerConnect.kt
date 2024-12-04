import android.os.StrictMode
import java.io.File
import java.io.FileInputStream
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
    NewDiscussionPost(5),
    DiscussionPost(6),
    GetUser(7),
    BanUser(8),
    SendImageToServer(9),
}

enum class MessageResult(val int : Int) {

    Failed(0),
    LoginSuccess(1),
    Success(2);

    companion object {
        fun fromInt(int : Int) = entries.first {it.int == int}
    }
}


class ServerConnect private constructor() {

    private val serverAddress = getMyServerAddress()
    //TODO: add some error handling
    private var socket : Socket? = null
    private var inputStream : InputStream? = null
    private var outputStream : OutputStream? = null
    private var token : ByteArray? = null


    /**
     * Initialize the server address.
     */
    private fun getMyServerAddress() : InetAddress {
        val policy = StrictMode.ThreadPolicy.Builder().permitAll().build()
        StrictMode.setThreadPolicy(policy)

        return InetAddress.getByName(SERVER_NAME)     //https://stackoverflow.com/questions/5806220/how-to-connect-to-my-http-localhost-web-server-from-android-emulator
    }

    /**
     * Initialize the socket.
     */
    private fun getMySocket() {
        val policy = StrictMode.ThreadPolicy.Builder().permitAll().build()
        StrictMode.setThreadPolicy(policy)

        try { socket = Socket(serverAddress, PORT) }
        finally { println("Attempted to make socket") }

    }

    /**
     * Listen for server and parse command
     */
    private fun listenForServer()
    {
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
                Command.NewDiscussionPost.int -> {}
                Command.DiscussionPost.int -> {}
                Command.GetUser.int -> {}
                Command.BanUser.int -> {}
                else -> println("Received unexpected command")
            }
        }
    }

    /**
     * Read in an integer from the ByteStream. Used in ListenForServer
     * and also to get the size of chunks for reading.
     */
    private fun readHeader() : Int {
        val buffer = ByteArray(Int.SIZE_BYTES)
        var bytesRead = 0
        while (bytesRead < 1) {
            bytesRead = inputStream!!.read(buffer)
        }
        return ByteBuffer.wrap(buffer).order(ENDIAN).getInt()
    }

    /**
     * Some methods return a state from the server. This reads that.
     */
    private fun messageResult() : Int {
        return readHeader()
    }

    /**
     * Read in the new token. The token should be null before the first time this is called.
     */
    private fun handleToken() {
        //println(messageResult())
        val bytesToRead = readHeader()

        token = ByteArray(bytesToRead)
        inputStream?.read(token, 0, bytesToRead)
        println("Your token: $token")

    }

    /**
     * Sends the command, token, and message to the server.
     */
    private fun sendToServer(command : Int, message : ByteArray)
    {
        val tokenSize = if (token != null)
            HASH_SIZE + 1 else 0

        val lengthOfMessage = message.size + LENGTH_OF_COMMAND_AND_MESSAGE_HEADER

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
        messageToServer += message + 0x00

        outputStream?.write(messageToServer)
        outputStream?.flush()
    }
    private fun sendToServer(command : Int, message : String) = sendToServer(command, message.toByteArray())


    /**
     * Attempts to sign the user up to the server given the name, username, email, and password, in that order.
     */
    fun signUp(name : String, username : String, email : String, password : String) {
        val message = "$name\n$username\n$email\n$password" + 0.toChar()
        sendToServer(Command.NewUser.int, message)
    }

    fun login(username : String, password : String)
    {
        val message = "$username\n$password"
        sendToServer(Command.Login.int, message)
    }

    fun sendImageToServer(file : File) {
        Thread{
        val input = file.readBytes()
        sendToServer(Command.SendImageToServer.int, input.toString())
        }.start()
    }

    fun sendImageToServer(file : FileInputStream) {
        Thread {
            val input = file.readBytes()
            sendToServer(Command.SendImageToServer.int, input)
            file.close()
        }.start()
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
        fun instance() = INSTANCE
        private val INSTANCE = ServerConnect()
        private const val SERVER_NAME = "10.0.2.2"
        private const val PORT = 5930
        private const val HASH_SIZE = 32
        private const val LENGTH_OF_COMMAND_AND_MESSAGE_HEADER = Int.SIZE_BYTES * 3 + 1
        //server endian
        private val ENDIAN = ByteOrder.LITTLE_ENDIAN
    }
}