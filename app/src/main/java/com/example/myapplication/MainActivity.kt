package com.example.myapplication

import ActiveUser
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.navigation.findNavController
import androidx.navigation.ui.AppBarConfiguration
import androidx.navigation.ui.navigateUp
import android.view.MenuInflater
import android.view.MenuItem
import android.widget.ImageButton
import android.widget.PopupMenu
import android.widget.TextView
import androidx.fragment.app.Fragment
import com.example.myapplication.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var appBarConfiguration: AppBarConfiguration
    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        //Setting the starting screen
        bottomNavigation(SecondFragment())
        //Changing the fragment to selected icon
        binding.bottomNav.setOnItemSelectedListener {
            when(it.itemId){
                R.id.Home -> bottomNavigation(FirstFragment())

                else ->{

                }
            }
            true
        }
        //click register for making the popup
        findViewById<ImageButton>(R.id.user_avatar).setOnClickListener {
            val popup = PopupMenu(this, it)
            val inflater: MenuInflater = popup.menuInflater
            inflater.inflate(R.menu.menu_main, popup.menu)
            popup.show()
        }
    }

    // function for moving across the bottom nav
    private fun bottomNavigation(fragment : Fragment){
        val fragmentManager = supportFragmentManager
        val fragmentTransaction = fragmentManager.beginTransaction()
        fragmentTransaction.replace(R.id.home_screen, fragment)
        fragmentTransaction.commit()
    }

    override fun onPostCreate(savedInstanceState: Bundle?) {
        super.onPostCreate(savedInstanceState)
        updateText()
        updateModel()
    }

    private fun updateText()
    {
        val calories  : TextView = findViewById(R.id.textView_calories_burned_info)
        val workedOn  : TextView = findViewById(R.id.textView_worked_on_info)
        val timeSpent : TextView = findViewById(R.id.textView_time_spent_info)

        calories.text = ActiveUser.displayCaloriesBurned()
        workedOn.text = ActiveUser.displayPartsWorked()
        timeSpent.text = ActiveUser.displayTimeSpentExercising()
    }

    private fun updateModel()
    {}

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
        val navController = findNavController(R.id.nav_host_fragment_content_main)
        return navController.navigateUp(appBarConfiguration)
                || super.onSupportNavigateUp()
    }
}