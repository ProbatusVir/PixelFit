import java.io.File
import java.io.IOException


class Loader {

    private var data = HashMap<String, String>()

    constructor(file: File) {
        initialize(file.readText().toString())
    }

    constructor(str: String) {
        initialize(str)
    }

    private fun initialize(str : String)
    {
        val tokens = str.split('\n')
        for (token in tokens) {
            val split = token.split('=')
            data[split[0]] = split[1]
        }
    }

    fun fetchVariable(name : String): String {
        if (!data.containsKey(name))
            throw(IOException("Make sure the variable you're looking for exists!"))
        else
            return data[name]!!
    }

    fun fetchIntegralVariable(name : String) = fetchVariable(name).toInt()

    fun fetchDoubleVariable(name : String) = fetchVariable(name).toDouble()

    //This will default to false without excepting.
    fun fetchBooleanVariable(name : String) = fetchVariable(name).toBoolean()
}
