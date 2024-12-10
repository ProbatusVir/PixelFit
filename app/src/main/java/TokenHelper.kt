class TokenHelper {
        var tokenList = ArrayList<String>()

        constructor(data : String, delim : Char = '\n') {
            var fields = 1
            for (i in data.indices)
                fields += if (data[i] == delim) 1 else 0

            initialize(fields, data, delim)
        }

        constructor(fields : Int, data : String, delim : Char = '\n') {
            initialize(fields, data, delim)
        }

        private fun initialize(fields : Int, data : String, delim : Char = '\n') {
            var prev = 0
            for (i in 0 until fields - 1) {
                val offset = data.indexOf(delim, prev)

                //var b = data.substring(prev, offset)
                tokenList.add(data.substring(prev, offset))

                prev = offset + 1
            }

            tokenList.add(data.substring(prev))
        }
}