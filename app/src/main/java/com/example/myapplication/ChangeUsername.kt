import android.os.Bundle
import android.view.Gravity
import android.view.LayoutInflater
import android.view.View
import android.widget.Button
import android.widget.EditText
import android.widget.PopupWindow
import androidx.appcompat.app.AppCompatActivity
import com.example.myapplication.R

class MainActivity : AppCompatActivity() {

    private lateinit var blurOverlay: View // Reference to the blur overlay

    private var newUsername: String = "" //new username variable for server connect

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

        blurOverlay.visibility = View.VISIBLE

        popupWindow.setOnDismissListener {
            blurOverlay.visibility = View.GONE // Hide the blur overlay when popup is dismissed
        }

        popupWindow.showAtLocation(window.decorView.rootView, Gravity.CENTER, 0, 0)

        // Reference the EditText and Button in the popup
        val usernameInput: EditText = popupView.findViewById(R.id.newUsername_input)
        val saveButton: Button = popupView.findViewById(R.id.popup_button)

        saveButton.setOnClickListener {
            //FIXME: this needs server error checking so usernames are unique and of the right length
            if (newUsername.length < 3)
            {
              popupWindow.dismiss()
            }
            else
            {
                newUsername = usernameInput.text.toString()
                popupWindow.dismiss()
            }

        }
    }

}

