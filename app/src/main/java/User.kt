import android.content.Context
import android.graphics.BitmapFactory
import android.graphics.drawable.Drawable
import androidx.core.graphics.drawable.toDrawable
import kotlin.random.Random

//TODO: This is a far from complete class, will have to work on server integration.
open class User {
    lateinit var username : String
    lateinit var displayName : String
    private lateinit var profileImagePath : String


    fun loadFromServer() {}
    fun sendToServer() {}

    public fun requestUser() {
        //TODO: Get info from server and initialize our information
        profileImagePath = "/tmp/" + randomPath()
        cacheProfileImage()
    }

    public fun getProfileImage(context : Context) : Drawable {
        return BitmapFactory.decodeFile(profileImagePath).toDrawable(context.resources)
    }

    private fun cacheProfileImage()
    {

    }

    // TODO: Possible variables???
    //  physique
    //  achievements
    //  namesOnOtherPlatforms

    private fun randomPath() : String {
        val pathSize = 8
        var str = String()

        //There is a randomSequence method for strings, but it's not guaranteed to be filesystem safe.

        for (i in 0 until pathSize)
        {
            val x = Random.nextInt(1, 62 + 1)
            str += (x + 47 + 7 * (if (x > 10)  1 else 0) + 6 * (if (x > 36) 1 else 0)) to Char
        }
        return str
    }
}