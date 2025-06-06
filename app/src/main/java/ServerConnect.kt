import android.os.Handler
import android.os.Looper
import android.os.StrictMode
import android.widget.Toast
import com.example.myapplication.Instructor
import com.example.myapplication.MainActivity
import java.io.*
import java.net.InetAddress
import java.net.Socket
import java.nio.ByteBuffer
import java.nio.ByteOrder

/**
 * Some things to work on, Request data probably shouldn't be sent back, so that
 * WHen the server needs to ask someone for something, they can deliver it (if ever)
 */


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
    SendPfpToServer(9),
    LogOut(10),
    RequestData(11),
    GetAllUsers(12),
    GetUsersContaining(13),
    GetActiveUsers(14),
    BlockUser(15),
    FriendUser(16),
    CreateChallenge(17),
}

enum class ResourceType(val int:  Int)
{
    PNG(0x504E4700), //this is first four bytes of the PNG header.
    DIR(0x44495200),
    WORK(0x574F524B),
    CHAL(0x4348414C),
    RCHL(0x5243484C),
    TCHL(0x5743484C)

}



object ServerConnect {
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

        var addr : InetAddress? = null
        while (addr == null) {

            addr = try {
                InetAddress.getByName(SERVER_NAME)
            } catch (e: Exception) {
                InetAddress.getByName(LOCALHOST) //This alternative keeps the client from crashing when a server is not specified or not active
            }
            if (addr == null)
                makeToast("Failed to find server address.")
        }

        makeToast("Connected to server!")
        return addr

        //https://stackoverflow.com/questions/5806220/how-to-connect-to-my-http-localhost-web-server-from-android-emulator
    }

    /**
     * Initialize the socket.
     */
    private fun getMySocket() {
        val policy = StrictMode.ThreadPolicy.Builder().permitAll().build()
        StrictMode.setThreadPolicy(policy)
        while (socket == null) {
            try {
                socket = Socket(serverAddress, PORT)
            } catch(e : Exception) { makeToast("Failed to connect to server :(")}
            finally {
                println("Attempted to make socket")
            }
        }

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
                Command.SocketError.int -> makeToast("Disconnected from server!")
                Command.Failed.int -> makeToast("Failed to sign in!")
                Command.Login.int -> handleToken()
                Command.GetUsers.int -> {}
                Command.MessageServer.int -> {}
                Command.NewDiscussionPost.int -> {}
                Command.DiscussionPost.int -> {}
                Command.GetUser.int -> {}
                Command.BanUser.int -> {}
                Command.RequestData.int -> receiveData()
                Command.GetActiveUsers.int -> receiveUsers()
                Command.GetAllUsers.int -> receiveUsers()
                Command.GetUsersContaining.int -> receiveUsers()
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
        makeToast(ActiveUser.username + " logged in!")
    }

    /**
     * Sends the command, token, and message to the server.
     */
    fun sendToServer(command : Int, message : ByteArray)
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
        else
            messageToServer += ByteBuffer.allocate(Int.SIZE_BYTES).order(ENDIAN).putInt(0).array()

        messageToServer += ByteBuffer.allocate(Int.SIZE_BYTES).order(ENDIAN).putInt(lengthOfMessage).array()
        messageToServer += message + 0x00

        outputStream?.write(messageToServer)
        outputStream?.flush()
    }
    public fun sendToServer(command : Int, message : String) = sendToServer(command, message.toByteArray())


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

    fun sendPfpToServer(file : File) {
        Thread{
        val input = file.readBytes()
        sendToServer(Command.SendPfpToServer.int, input)
        }.start()
    }

    fun sendPfpToServer(file : FileInputStream) {
        Thread {
            val input = file.readBytes()
            sendToServer(Command.SendPfpToServer.int, input)
            file.close()
        }.start()
    }

    fun logOut()
    {
        //The error checking is two-fold, for the token does not exist without a socket.
        //otherwise, we have a REAL problem.
        token?.let { sendToServer(Command.LogOut.int, "") }
        token = null
        makeToast(ActiveUser.username + " logged out.")
    }

    fun requestData(fileName : String, type : ResourceType) {
        //val buffer = (ByteBuffer.allocate(Int.SIZE_BYTES).order(ENDIAN).putInt(type.int).array()
        //        + fileName.toByteArray())

        val fileNameAsBA = fileName.toByteArray() //.order(ENDIAN)
        val properType = ByteBuffer.allocate(Int.SIZE_BYTES).putInt(type.int).array()
        val buffer = properType + fileNameAsBA
        sendToServer(Command.RequestData.int, buffer)

    }

    private fun receiveData() {
        val tokenSize = readHeader()
        val token = ByteArray(tokenSize)
        inputStream!!.read(token, 0, tokenSize)
        //TODO: Add some verification
        val bufferSize = readHeader()
        val buffer = ByteArray(bufferSize)
        var bytesRead = 0
        while (bytesRead < bufferSize) {
            bytesRead += inputStream!!.read(buffer, bytesRead, bufferSize - bytesRead)
        }

        val type = ByteBuffer.wrap(buffer).getInt()
        when (type)
        {
            ResourceType.PNG.int -> receiveImage(buffer)
            ResourceType.DIR.int -> receiveDir(buffer)
            ResourceType.WORK.int -> receiveWork(buffer)
        }
    }

    private fun findFirstOf(buffer : ByteArray, delim : Char = '\n') : Int {
        var endOfName = 0
        for (i in 0 until buffer.size)
        {
            if (buffer[i] == delim.code.toByte())
                break
            endOfName++
        }
        return endOfName
    }

    private fun simpleReceiveFile(buffer : ByteArray, extension : String)
    {
        val endOfName = findFirstOf(buffer)
        val fileName = String(buffer, Int.SIZE_BYTES, endOfName - Int.SIZE_BYTES)
        val file = File(Shared.filesDir, fileName + extension)
        val out = FileOutputStream(file)

        out.write(buffer, endOfName + 1, buffer.size - (endOfName + 1))
    }

    private fun receiveImage(buffer : ByteArray) {
        simpleReceiveFile(buffer, ".png")

    }

    private fun receiveDir(buffer : ByteArray) {
        val data = String(buffer).split(Char(0))
        val split = data[1].split('\n')
        val values = split.slice(1 until split.lastIndex)

        Shared.directories[split[0]] = values

    }

    private fun receiveWork(buffer : ByteArray) {
        val endOfName = findFirstOf(buffer)
        val strep = String(buffer, endOfName + 1, buffer.size - (endOfName + 1))
        val data = Loader(strep)
        Instructor.mList.add(
            InstructorData(
            data.fetchVariable("title"),
            data.fetchVariable("description"),
            data.fetchVariable("src")
            )
        )

        makeToast("Your workouts have been delivered.")
    }

    //This needs serious refactoring.
    private fun receiveUsers() {
        Shared.userQueryResults = ArrayList()

        val tokenSize = readHeader()
        val token = ByteArray(tokenSize)
        inputStream!!.read(token, 0, tokenSize)
        val bufferSize = readHeader()
        val buffer = ByteArray(bufferSize)
        var bytesRead = 0
        while (bytesRead < bufferSize) {
            bytesRead += inputStream!!.read(buffer, bytesRead, bufferSize - bytesRead)
        }

        val data = String(buffer).split('\n')
        Shared.userQueryResults = ArrayList(data)

        makeToast("Users have been delivered.")
    }

    fun getAllUsers() = sendToServer(Command.GetAllUsers.int, ByteArray(0))

    fun getUsersContaining(string: String) = sendToServer(Command.GetUsersContaining.int, string)

    fun getActiveUsers() = sendToServer(Command.GetActiveUsers.int, ByteArray(0))

    fun blockUser(username : String) = sendToServer(Command.BlockUser.int, username)

    fun toggleFriendUser(username : String) = sendToServer(Command.FriendUser.int, username)


    fun connected() : Boolean {
        return socket != null
    }

    fun authenticated() : Boolean
    {
        return token != null
    }

    //Safely close the connection
    private fun disconnect() {
        try {
            socket?.let {
                if (it.isConnected)
                    it.close()
            }
        } finally {}
        makeToast("Disconnected from server.")
    }

    private fun makeToast(str : String) {
        val context = MainActivity.getApplicationContext()
        Handler(Looper.getMainLooper()).post {
            Toast.makeText(context, str, Toast.LENGTH_SHORT).show()
        }
    }

    init {
        Thread {
            getMySocket()
            inputStream = socket?.getInputStream()
            outputStream = socket?.getOutputStream()
            listenForServer()
        }.start()
    }

        private const val SERVER_NAME = "10.0.2.2"
        private const val LOCALHOST = "10.0.2.2"
        private const val PORT = 5930
        private const val HASH_SIZE = 32
        private const val LENGTH_OF_COMMAND_AND_MESSAGE_HEADER = Int.SIZE_BYTES * 3 + 1 //This is good for an authenticated read, might have to cut it out later.
        //server endian
        private val ENDIAN = ByteOrder.LITTLE_ENDIAN
}