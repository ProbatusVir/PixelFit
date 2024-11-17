import android.graphics.Color
import androidx.core.graphics.blue
import androidx.core.graphics.green
import androidx.core.graphics.red
import java.io.File
import java.util.*

class GuyFactory {
    companion object Guy {
        const val FILE_NAME  = "displayGuy.svg"

        const val LEFT_LEG_SHAPE    = "m80.97,146.71c0.07,-4.63 23.33,-4.25 23.33,0.09l-0.52,101.15c0,7.5 -5.23,11.86 -11.76,11.81 -5.85,-0.05 -11.48,-4.15 -11.08,-11.49z"
        const val RIGHT_LEG_SHAPE   = "m104.55,147.19c0.07,-4.63 23.33,-4.25 23.33,0.09l-0.52,101.15c0,7.5 -5.23,11.86 -11.76,11.81 -5.85,-0.05 -11.48,-4.15 -11.08,-11.49z"
        const val LEFT_ARM_SHAPE    = "M72.11,72.87 L64.08,142.68c-0.57,4.87 2.27,9.15 6.01,9.55 3.92,0.42 7.12,-2.71 8.26,-8.45l8.28,-69.49c1.29,-4.73 -3.63,-4.97 -6.37,-5.31 -2.99,-0.38 -7.44,-0.69 -8.16,3.89z"
        const val RIGHT_ARM_SHAPE   = "m135.91,73.19 l8.03,69.81c0.57,4.87 -2.27,9.15 -6.01,9.55 -3.92,0.42 -7.12,-2.71 -8.26,-8.45l-8.28,-69.49c-1.29,-4.73 3.63,-4.97 6.37,-5.31 2.99,-0.38 7.44,-0.69 8.16,3.89z"
        const val TORSO_SHAPE       = "m80.55,137.08c-0.3,21.84 23.34,20.74 23.34,20.74 0,0 22.34,-1.3 23.04,-20.74l0.3,-62.11c0,-10.52 -13.21,-18.24 -23.04,-18.43 -10.12,-0.2 -24.54,7.21 -24.44,18.03z"
        const val HEAD_SHAPE        = "M104.39,36.93m-22.44,0a22.44,22.44 0,1 1,44.88 0a22.44,22.44 0,1 1,-44.88 0"

        const val NAMESPACE = "android:"
        const val STROKE = .18492
        const val UNIT = "dp"
        const val WIDTH = 210;  const val HEIGHT = 297
        const val OUTLINE_COLOR = Color.BLACK

        const val WORKED_COLOR = Color.RED
        const val UNWORKED_COLOR = Color.WHITE

        const val HEADER = ( "<vector xmlns:android=\"http://schemas.android.com/apk/res/android\"\n"
                + "\t$NAMESPACE" + "width=\"$WIDTH$UNIT\"\n"
                + "\t$NAMESPACE" + "height=\"$HEIGHT$UNIT\"\n"
                + "\t$NAMESPACE" + "viewportWidth=\"$WIDTH\"\n"
                + "\t$NAMESPACE" + "viewportHeight=\"$HEIGHT\"" + ">\n"
                )
    }

    fun saveToFile(file : File) {
        if (!file.exists())
            file.createNewFile()

        file.writeText(
           HEADER
           + Path(LEFT_LEG_SHAPE, ActiveUser.Parts.Legs).toString()
            + Path(RIGHT_LEG_SHAPE, ActiveUser.Parts.Legs).toString()
            + Path(LEFT_ARM_SHAPE, ActiveUser.Parts.Arms).toString()
            + Path(RIGHT_ARM_SHAPE, ActiveUser.Parts.Arms).toString()
            + Path(TORSO_SHAPE, ActiveUser.Parts.Torso).toString()
            + Path(HEAD_SHAPE, ActiveUser.Parts.Head).toString()
            + "</vector>")
    }


    class Path(private val shape : String, private val part : ActiveUser.Parts) {
        private fun getColor() : Int = if (ActiveUser.partsWorked[part.ordinal])  WORKED_COLOR else UNWORKED_COLOR

        @OptIn(ExperimentalStdlibApi::class)
        override fun toString() : String
        {
            val color = getColor()
            val fillHex = String.format(Locale.US, "#%02x%02x%02x", color.red, color.green, color.blue)
            val outlineHex = String.format(Locale.US, "#%02x%02x%02x", OUTLINE_COLOR.red, OUTLINE_COLOR.green, OUTLINE_COLOR.blue)
            return ("\t<path\n"
                            + "\t\t$NAMESPACE" + "pathData=\"$shape\"\n"
                            + "\t\t$NAMESPACE" + "strokeWidth=\"$STROKE\"\n"
                            + "\t\t$NAMESPACE" + "fillColor=\"$fillHex\"\n"
                            + "\t\t$NAMESPACE" + "strokeColor=\"$outlineHex\"" + "/>\n"
                    )
        }
    }
}