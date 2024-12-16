//Only use this ctor the first time, otherwise we load from file or server.
class Height(initHeight: Double) {
    var height : Double = 0.0 //In inches

    private companion object Conversions {
        private const val IN_TO_CM: Double = 2.54;                  private const val CM_TO_IN: Double = 1 / IN_TO_CM
        private const val IN_TO_M: Double = IN_TO_CM / 100;         private const val M_TO_IN: Double = 1 / IN_TO_M
        private const val IN_TO_FT: Double = 1.0 / 12.0;            private const val FT_TO_IN: Double = 1 / IN_TO_FT
    }

    //Obviously if it was saved in metric and loaded in Freedom, things would be off.
    fun setHeight(metric : Boolean, newHeight : Double) {
        height = if (metric) newHeight * CM_TO_IN else newHeight
    }

    fun heightInIn() : Double = height

    fun heightInCm() : Double = height * IN_TO_CM

    fun heightInFt() : Double = height * IN_TO_FT

    fun heightInM() : Double = height * IN_TO_M

    fun displayHeight() : String {
        return if (Preferences.metric) heightInCm().toInt().toString()
        else heightInFt().toInt().toString() + '\'' + heightInIn().toInt().toString() + '"'
    }

    init {
        setHeight(Preferences.metric, initHeight)
    }

}