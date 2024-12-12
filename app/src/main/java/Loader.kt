import java.io.File


class Loader {

    private var data = String()

    constructor(file: File) {
        data = file.readBytes().toString()
    }

    constructor(str: String) {
        data = str
    }

    fun fetchVariable(name : String): String {
        val tokens = data.split('\n')
        for (token in tokens) {
            val split = token.split('=')
            if (split[0].equals(name, true))
                return split[1]
        }

        return String()
    }

    fun fetchIntegralVariable(name : String) = fetchVariable(name).toInt()
}
