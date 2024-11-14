object ActiveUser : User() {
    //TODO: A far from exhaustive list of anatomy!
    enum class Parts {
        Arms, Legs, Torso
    }

    lateinit var name : String

    var caloriesBurned : Double = 0.0
    var calorieGoals : Int = 0

    var weight : Double = 0.0 //lbs
    var height : Height = Height() //inches

    var partsWorked : BooleanArray = BooleanArray(Parts.entries.size)

    //TODO: Implement these, and remember, we only need to SAVE one user's data.
    fun loadFromFile() {}
    fun saveToFile() {}

    public fun weightInLbs() : Double = weight

    public fun weightInKgs() : Double = weight * 0.453592

    public fun bmi() : Double = weightInKgs() / (height.heightInM() * height.heightInM())
        //return weightInKgs() / (height.heightInM() * height.heightInM())
    //}

    class Height
    {
        private final val inToCm : Double = 2.54;       private final val cmToIn : Double = 1 / inToCm
        private final val inToM : Double = .0254;       private final val mToIn : Double = 1 / inToM
        private final val inToFt : Double = 1.0 / 12.0; private final val ftToIn : Double = 1 / inToFt


        var height : Double = 0.0

        //Obviously if it was saved in metric and loaded in Freedom, things would be off.
        public fun setHeight(metric : Boolean, newHeight : Double) {
            height = if (metric) newHeight * cmToIn else newHeight;
        }

        fun heightInIn() : Double = height

        fun heightInCm() : Double = height * inToCm

        fun heightInFt() : Double = height * inToFt

        public fun heightInM() : Double = height * inToM

        public fun displayHeight() : String {
            return if (Preferences.metric) heightInCm().toInt().toString()
                else heightInFt().toInt().toString() + '\'' + heightInIn().toInt().toString() + '"'
        }

    }
}