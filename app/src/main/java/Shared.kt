import android.net.Uri
import androidx.core.net.toUri
import java.io.File
import kotlin.collections.ArrayList
import java.util.HashMap

object Shared {
    lateinit var filesDir : File
    var directories = HashMap<String, List<String>>()
    var userQueryResults = ArrayList<String>()

    fun getPfpUri() : Uri? {
        val imageFile = File(filesDir, PROFILE_IMAGE_NAME)

        return if (!imageFile.exists()) null
        else imageFile.toUri()
    }

    const val PROFILE_IMAGE_NAME = "pfp.png"
}