package com.example.myapplication

import InstructorAdapter
import InstructorData
import ServerConnect
import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.view.MenuInflater
import androidx.appcompat.app.AppCompatActivity
import androidx.navigation.findNavController
import androidx.navigation.ui.AppBarConfiguration
import androidx.navigation.ui.navigateUp
import android.view.MenuInflater
import android.view.MenuItem
import android.view.View
import android.widget.ImageButton
import android.widget.PopupMenu
import android.widget.SearchView
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.constraintlayout.widget.ConstraintSet.Constraint
import androidx.navigation.fragment.NavHostFragment
import androidx.navigation.ui.setupWithNavController
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.example.myapplication.databinding.ActivityMainBinding
import com.google.android.material.bottomnavigation.BottomNavigationView
import java.io.FileInputStream

class MainActivity : AppCompatActivity() {

    private lateinit var appBarConfiguration: AppBarConfiguration
    private lateinit var binding: ActivityMainBinding

    private val connection = ServerConnect()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        val profile = findViewById<ImageButton>(R.id.user_avatar)

        val bottomNav = findViewById<BottomNavigationView>(R.id.bottom_nav)

        val navHostFragment = supportFragmentManager.findFragmentById(R.id.nav_host_fragment_activity_main) as NavHostFragment

        val navView : BottomNavigationView = binding.bottomNav

        val navController = navHostFragment.navController
        navView.setupWithNavController(navController)

        val appBarConfiguration = AppBarConfiguration(
            setOf(
                R.id.HomeFragment, R.id.FriendsFragment, R.id.GroupFragment, R.id.InstructorFragment
            )

        )

        // Show/hide bottom navigation and profile based on destination
        navController.addOnDestinationChangedListener { _, destination, _ ->
            when (destination.id) {
                R.id.LoginFragment, R.id.signUp, R.id.SettingsFragment -> {
                    bottomNav.visibility = View.GONE
                    profile.visibility = View.GONE
                }

                R.id.InstructorFragment-> {
                    profile.visibility = View.GONE
                }
                else -> {
                    bottomNav.visibility = View.VISIBLE
                    profile.visibility = View.VISIBLE
                }
            }
        }
        openImageIntent = Intent(Intent.ACTION_OPEN_DOCUMENT).apply {
            type = "image/png"
            addCategory(Intent.CATEGORY_OPENABLE)
            intent.action = Intent.ACTION_GET_CONTENT
            intent.addFlags(Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION)
        }

        // Handle user avatar click (popup menu)
        profile.setOnClickListener {
            showUserAvatarMenu(it, navController)
            //testFeature()
        }
    }

    fun testFeature()
    {
        //This is for requesting an image
       connection?.requestData("workouts", ResourceType.DIR)
    }

    private fun showUserAvatarMenu(view: View, navController: androidx.navigation.NavController) {
        // Create and show the popup menu
        val popup = PopupMenu(this, view)
        val inflater: MenuInflater = popup.menuInflater
        inflater.inflate(R.menu.menu_main, popup.menu)


        popup.setOnMenuItemClickListener { item ->
            when (item.itemId) {
                R.id.edit_profile_menuitem -> {


                    true
                }

                R.id.goals_trophies_menuitem -> {


                    true
                }

                R.id.block_list_menuitem -> {

                    true
                }

                R.id.pending_duels_menuitem -> {


                    true
                }

                R.id.settings_menuitem -> {

                    navController.navigate(R.id.action_HomeFragment_to_SettingsFragment)

                    true
                }

                else -> false
            }
        }

        popup.show()
        //click register for making the popup

    }

    //PSSSSSSSSSSSSSSSSSSSSSSSSST
    //HEY, YOU SHOULD READ ME
    //PLEASE READ BELOW
    //DON'T DELETE
    @Deprecated("Deprecated in Java")
    override fun onActivityResult(requestCode: Int, resultCode: Int, returnIntent: Intent?) {
        super.onActivityResult(requestCode, resultCode, returnIntent)
        if (resultCode != RESULT_OK) {
            return
        } //
        if (requestCode == OPEN_IMAGE) {
            val returnUri = returnIntent?.data ?: return
            val pfd = contentResolver.openFileDescriptor(returnIntent.data!!, "r")
            val fis = FileInputStream(pfd!!.fileDescriptor)
            connection?.sendImageToServer(fis)
        }
    }

    //implement this when the screens (fragments???) exist
    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        val navController = findNavController(R.id.nav_host_fragment_activity_main)
        return when (item.itemId) {
            R.id.settings_menuitem -> {
                // Navigate to SettingsFragment
                navController.navigate(R.id.action_HomeFragment_to_SettingsFragment)
                true
            }

            else -> super.onOptionsItemSelected(item)
        }
    }

    override fun onSupportNavigateUp(): Boolean {
        val navController = findNavController(R.id.nav_host_fragment_activity_main)
        return navController.navigateUp(appBarConfiguration) || super.onSupportNavigateUp()
    }
    companion object {
        var CONTEXT : Context? = null
        const val OPEN_IMAGE = 56
    }
}
