import java.io.File
import java.util.HashMap

object Shared {
    lateinit var filesDir : File
    var directories = HashMap<String, List<String>>()
}