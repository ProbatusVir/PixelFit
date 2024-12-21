import java.io.File
import java.io.IOException

class Loader {

    private var data = HashMap<String, String>()

    constructor(file: File) {
        initialize(file.readText())
    }

    constructor(str: String) {
        initialize(str)
    }

    private fun initialize(str: String) {
        val tokens = str.split('\n')
        for (token in tokens) {
            val split = token.split('=')
            if (split.size < 2) {
                println("Skipping malformed line: $token") // Log malformed lines
                continue
            }
            data[split[0]] = split[1]
        }
    }

    fun fetchVariable(name: String): String {
        return data[name] ?: throw IOException("Variable '$name' does not exist in the data.")
    }

    fun fetchIntegralVariable(name: String): Int {
        return fetchVariable(name).toIntOrNull()
            ?: throw IOException("Variable '$name' is not a valid integer.")
    }

    fun fetchDoubleVariable(name: String): Double {
        return fetchVariable(name).toDoubleOrNull()
            ?: throw IOException("Variable '$name' is not a valid double.")
    }

    fun fetchBooleanVariable(name: String): Boolean {
        return fetchVariable(name).toBoolean()
    }
}
