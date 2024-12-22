import java.io.File
import java.util.*

object ActiveUser : User() {
    //TODO: A far from exhaustive list of anatomy!
    enum class Parts {
        Arms, Legs, Torso, Head
    }

    var name : String = "User-san"

    var sex : Boolean = false //True is male, will consult with Devin
    var caloriesBurned : Double = 0.0
    var calorieGoals : Int = 0
    var timeSpentExercising : Int = 0 // Minutes

    var weight : Double = 0.0    //lbs
    var height : Height = Height(0.0)//inches

    var partsWorked : BooleanArray = BooleanArray(Parts.entries.size)

    fun loadFromFile() : Boolean {
        val file = File(Shared.filesDir, FILE_NAME)

        if (!file.exists()) return false

        val variables = Loader(file)
        name = variables.fetchVariable("name")
        sex = variables.fetchBooleanVariable("sex")
        caloriesBurned = variables.fetchDoubleVariable("caloriesBurned")
        calorieGoals = variables.fetchIntegralVariable("calorieGoals")
        timeSpentExercising = variables.fetchIntegralVariable("timeSpentExercising")
        weight = variables.fetchDoubleVariable("weight")
        height = Height(variables.fetchDoubleVariable("height"))
        for (part in Parts.entries) {
            partsWorked[part.ordinal] = variables.fetchBooleanVariable(part.name)
        }

        if (isOlderThanADay(file))
        {
            caloriesBurned = 0.0
            timeSpentExercising = 0
            for (i in partsWorked.indices)
                partsWorked[i] = false
            saveToFile()
        }
        return true
    }
    fun saveToFile() {
        //Things are always saved in freedom units: lbs, in, KCal, minutes, no need for flagging or conversion here.
        val file = File(Shared.filesDir, FILE_NAME)

        file.writeText("name=$name\n")
        file.appendText(String.format(Locale.US,"sex=%1s\n", sex))
        file.appendText(String.format(Locale.US,"caloriesBurned=%.2f\n", caloriesBurned))
        file.appendText(String.format(Locale.US,"calorieGoals=%d\n", calorieGoals))
        file.appendText(String.format(Locale.US,"timeSpentExercising=%d\n", timeSpentExercising))
        file.appendText(String.format(Locale.US,"weight=%.2f\n", weight))
        file.appendText(String.format(Locale.US, "height=%.2f\n", height.heightInIn()))
        var partsString = ""
        for (part in Parts.entries) {
            partsString += String.format(Locale.US, "%1s=%2b\n", part.name, partsWorked[part.ordinal])
        }
        partsString.trimEnd() //The last \n was causing problems
        file.appendText(partsString)
    }

    fun weight() : Double = if (Preferences.metric) weightInKgs() else weight
    fun displayWeight() : String = weight().toString() + (if (Preferences.metric) "kg" else "lb")
    fun displayHeight() : String = height.displayHeight()
    fun displayCaloriesBurned() : String =
        String.format(Locale.US, "%d\nKCals!", caloriesBurned.toInt())
    fun displayTimeSpentExercising() : String =
        String.format(Locale.US, "%d hours\n%d minutes!", timeSpentExercising / 60, timeSpentExercising % 60)


    fun goalPercent() : Double {

        return if (calorieGoals == 0) 0.0
        else caloriesBurned / calorieGoals
    }
    fun displayPartsWorked() : String {
        var str = String()

        for (i in partsWorked.indices)
            if (partsWorked[i]) str += Parts.entries[i].name + '\n'

        return str.trimEnd()
    }
    //maybe we can show users the effectiveness of their workouts in Kcal/min so they can maximize gains

    private fun weightInLbs() : Double = weight

    private fun weightInKgs() : Double = weight * 0.453592

    fun bmi() : Double = weightInKgs() / (height.heightInM() * height.heightInM())

    //TODO: Implement... maybe...
    private fun isOlderThanADay(file : File) : Boolean
    {
        file.isFile //this is dumb IDE appeasement
        return false
    }

    init {
        loadFromFile()

    }

    const val FILE_NAME = "ActiveUser.usr"
}