package com.example.myapplication

import Shared
import ServerConnect
import android.content.Context
import android.os.Bundle
import android.view.MenuInflater
import androidx.appcompat.app.AppCompatActivity
import androidx.navigation.findNavController
import androidx.navigation.ui.AppBarConfiguration
import androidx.navigation.ui.navigateUp
import android.view.MenuItem
import android.view.View
import android.widget.PopupMenu
import androidx.navigation.fragment.NavHostFragment
import androidx.navigation.ui.setupWithNavController
import com.example.myapplication.databinding.ActivityMainBinding
import com.google.android.material.bottomnavigation.BottomNavigationView

class MainActivity : AppCompatActivity() {

    private lateinit var appBarConfiguration: AppBarConfiguration
    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        Shared.filesDir = applicationContext.filesDir

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        val profile = findViewById<com.google.android.material.imageview.ShapeableImageView>(R.id.user_avatar)

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
                R.id.LoginFragment, R.id.signUp, R.id.SettingsFragment, R.id.profileFragment, R.id.profileEdit -> {
                    bottomNav.visibility = View.GONE
                    profile.visibility = View.GONE
                }

                R.id.InstructorFragment-> {
                    profile.visibility = View.GONE
                }
                R.id.FriendsFragment-> {
                    profile.visibility = View.GONE
                }
                R.id.GroupFragment-> {
                    profile.visibility = View.GONE

                }

                else -> {
                    bottomNav.visibility = View.VISIBLE
                    profile.visibility = View.VISIBLE
                }
            }
        }

        loadPfp()

        // Handle user avatar click (popup menu)
        profile.setOnClickListener {
            showUserAvatarMenu(it, navController)
            //testFeature()
        }

        val connect = ServerConnect
        connect.connected() //It's chilling here
    }

    fun testFeature()
    {   //This is for requesting an image
        //connection?.requestData("coolfile", ResourceType.PNG)
    }

    private fun showUserAvatarMenu(view: View, navController: androidx.navigation.NavController) {
        // Create and show the popup menu
        val popup = PopupMenu(this, view)
        val inflater: MenuInflater = popup.menuInflater
        inflater.inflate(R.menu.menu_main, popup.menu)


        popup.setOnMenuItemClickListener { item ->
            when (item.itemId) {
                R.id.profile_menuitem -> {

                    navController.navigate(R.id.profileFragment)
                    true
                }

                R.id.settings_menuitem -> {

                    navController.navigate(R.id.SettingsFragment)

                    true
                }

                else -> false
            }
        }

        popup.show()
        //click register for making the popup

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

    fun loadPfp() {
        val avatar = binding.userAvatar
        val uri = Shared.getPfpUri() ?: return

        //flush and refresh
        avatar.setImageURI(null)
        avatar.setImageURI(uri)

    }

    init {
        instance = this
    }
    fun getContext() : Context = applicationContext

    override fun onSupportNavigateUp(): Boolean {
        val navController = findNavController(R.id.nav_host_fragment_activity_main)
        return navController.navigateUp(appBarConfiguration) || super.onSupportNavigateUp()
    }

    override fun onDestroy() {
        super.onDestroy()
        ActiveUser.saveToFile()
    }

    companion object {
        private var instance : MainActivity? = null
        fun getApplicationContext() : Context = instance!!.applicationContext
    }
}
