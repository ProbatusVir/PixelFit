package com.example.myapplication

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.navigation.findNavController
import androidx.navigation.ui.AppBarConfiguration
import androidx.navigation.ui.navigateUp
import android.view.MenuInflater
import android.view.MenuItem
import android.view.View
import android.widget.ImageButton
import android.widget.PopupMenu
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.NavHostFragment
import androidx.navigation.ui.setupWithNavController
import androidx.navigation.ui.setupActionBarWithNavController
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

        val navHostFragment = supportFragmentManager.findFragmentById(R.id.nav_host_fragment_activity_main) as NavHostFragment

        val navView : BottomNavigationView = binding.bottomNav

        val navController = navHostFragment.navController
        navView.setupWithNavController(navController)

        val appBarConfiguration = AppBarConfiguration(
            setOf(
                R.id.HomeFragment, R.id.FriendsFragment, R.id.GroupFragment, R.id.InstructorFragment
            )
        )

        navController.addOnDestinationChangedListener { _, destination, _ ->
            when (destination.id) {
                R.id.LoginFragment -> {
                    bottomNav.visibility = View.GONE
                    profile.visibility = View.GONE
                }
                else -> {
                    bottomNav.visibility = View.VISIBLE
                    profile.visibility = View.VISIBLE
                }
            }
        }

        //click register for making the popup
       findViewById<ImageButton>(R.id.user_avatar).setOnClickListener {
           val popup = PopupMenu(this, it)
           val inflater: MenuInflater = popup.menuInflater
           inflater.inflate(R.menu.menu_main, popup.menu)
           popup.show()
        }
    }

    //implement this when the screens (fragments???) exist
    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        return when (item.itemId)
        {
            R.id.edit_profile_menuitem ->
            {
                return true
            }
            R.id.goals_trophies_menuitem ->
            {
                return true
            }
            R.id.block_list_menuitem ->
            {
                return true
            }
            R.id.pending_duels_menuitem ->
            {
                return true
            }
            R.id.settings_menuitem ->
            {
                return true
            }
            else -> super.onOptionsItemSelected(item)

        }
    }

    override fun onSupportNavigateUp(): Boolean {
        val navController = findNavController(R.id.nav_host_fragment_activity_main)
        return navController.navigateUp(appBarConfiguration)
                || super.onSupportNavigateUp()
    }
}