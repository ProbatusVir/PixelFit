import android.content.Context
import java.util.HashMap

object Shared {
    lateinit var context : Context
    var directories = HashMap<String, List<String>>()
}