import java.util.*

object ActiveUser : User() {
    //TODO: A far from exhaustive list of anatomy!
    enum class Parts {
        Arms, Legs, Torso, Head
    }

    lateinit var name : String

    var caloriesBurned : Double = 0.0
    var calorieGoals : Int = 0
    var timeSpentExercising : Int = 0 // Minutes

    var weight : Double = 0.0    //lbs
    var height : Height //inches

    var partsWorked : BooleanArray = BooleanArray(Parts.entries.size)

    //TODO: Implement these, and remember, we only need to SAVE one user's data.
    fun loadFromFile() {}
    fun saveToFile() {}

    fun weight() : Double = if (Preferences.metric) weightInKgs() else weight
    fun displayWeight() : String = weight().toString() + (if (Preferences.metric) "kg" else "lb")
    fun displayHeight() : String = height.displayHeight()
    fun displayCaloriesBurned() : String =
        String.format(Locale.US, "%d\nKCals!", caloriesBurned.toInt())
    fun displayTimeSpentExercising() : String =
        String.format(Locale.US, "%d hours\n%d minutes!", timeSpentExercising / 60, timeSpentExercising % 60)


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

    init {
        height = Height(0.0)
        caloriesBurned = 314.0
        partsWorked[Parts.Arms.ordinal] = true
        partsWorked[Parts.Legs.ordinal] = true
        partsWorked[Parts.Torso.ordinal] = true
        partsWorked[Parts.Head.ordinal] = false
        timeSpentExercising = 105
    }
}