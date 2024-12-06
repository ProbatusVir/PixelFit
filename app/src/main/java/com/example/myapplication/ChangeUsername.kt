import android.os.Bundle
import android.view.Gravity
import android.view.LayoutInflater
import android.view.View
import android.widget.Button
import android.widget.PopupWindow
import androidx.appcompat.app.AppCompatActivity
import com.example.myapplication.R

class MainActivity : AppCompatActivity() {

    private lateinit var blurOverlay: View // Reference to the blur overlay

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.fragment_settings) // Main layout with the blur overlay

        blurOverlay = findViewById(R.id.blur_overlay) // Initialize the blur overlay

        val changeUsernameButton: Button = findViewById(R.id.button_change_username)
        changeUsernameButton.setOnClickListener {
            showChangeUsernamePopup()
        }
    }

    private fun showChangeUsernamePopup() {
        // Inflate the layout for the popup
        val inflater = LayoutInflater.from(this)
        val popupView = inflater.inflate(R.layout.fragment_change_username, null)

        // Create the PopupWindow
        val popupWindow = PopupWindow(
            popupView,
            800, // Width in pixels
            600, // Height in pixels
            true // Focusable
        )

        // Show the blur overlay
        blurOverlay.visibility = View.VISIBLE

        // Set up dismissal behavior
        popupWindow.setOnDismissListener {
            blurOverlay.visibility = View.GONE // Hide the blur overlay when popup is dismissed
        }

        // Show the popup at the center of the screen
        popupWindow.showAtLocation(window.decorView.rootView, Gravity.CENTER, 0, 0)

        // Set up popup content actions
        val closeButton: Button = popupView.findViewById(R.id.popup_button)
        closeButton.setOnClickListener {
            popupWindow.dismiss() // Close the popup
        }
    }
}
