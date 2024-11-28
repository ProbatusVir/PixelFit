package com.example.myapplication

import android.os.Bundle
import android.view.MenuInflater
import android.view.View
import android.widget.ImageButton
import android.widget.PopupMenu
import androidx.appcompat.app.AppCompatActivity
import androidx.navigation.findNavController
import androidx.navigation.fragment.NavHostFragment
import androidx.navigation.ui.AppBarConfiguration
import androidx.navigation.ui.navigateUp
import androidx.navigation.ui.setupWithNavController
import com.example.myapplication.databinding.ActivityMainBinding
import com.google.android.material.bottomnavigation.BottomNavigationView

class MainActivity : AppCompatActivity() {

    private lateinit var appBarConfiguration: AppBarConfiguration
    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        val profile = findViewById<ImageButton>(R.id.user_avatar)
        val bottomNav = findViewById<BottomNavigationView>(R.id.bottom_nav)
        val navHostFragment =
            supportFragmentManager.findFragmentById(R.id.nav_host_fragment_activity_main) as NavHostFragment
        val navController = navHostFragment.navController


        val navView: BottomNavigationView = binding.bottomNav
        navView.setupWithNavController(navController)


        appBarConfiguration = AppBarConfiguration(
            setOf(
                R.id.HomeFragment, R.id.FriendsFragment, R.id.GroupFragment, R.id.InstructorFragment
            )
        )


        navController.addOnDestinationChangedListener { _, destination, _ ->
            when (destination.id) {
                R.id.LoginFragment, R.id.signUp -> {
                    bottomNav.visibility = View.GONE
                    profile.visibility = View.GONE
                }
                else -> {
                    bottomNav.visibility = View.VISIBLE
                    profile.visibility = View.VISIBLE
                }
            }
        }

        profile.setOnClickListener { view ->
            showUserAvatarMenu(view, navController)
        }
    }

    private fun showUserAvatarMenu(view: View, navController: androidx.navigation.NavController) {

        val popup = PopupMenu(this, view)
        val inflater: MenuInflater = popup.menuInflater

        inflater.inflate(R.menu.menu_main, popup.menu)

        // Handle menu item clicks
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
                    if(navController.currentDestination?.id == R.id.SettingsFragment) {
                        return@setOnMenuItemClickListener false
                    }
                    else {
                        navController.navigate(R.id.action_HomeFragment_to_SettingsFragment)
                    }
                    true
                }
                else -> false
            }
        }

        popup.show()
    }

    override fun onSupportNavigateUp(): Boolean {
        val navController = findNavController(R.id.nav_host_fragment_activity_main)
        return navController.navigateUp(appBarConfiguration) || super.onSupportNavigateUp()
    }
}