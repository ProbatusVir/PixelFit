package com.example.myapplication

import ServerConnect
import android.content.Intent
import android.os.Bundle
import android.view.MenuInflater
import androidx.appcompat.app.AppCompatActivity
import androidx.navigation.findNavController
import androidx.navigation.ui.AppBarConfiguration
import androidx.navigation.ui.navigateUp
import android.view.MenuItem
import android.view.View
import android.widget.ImageButton
import android.widget.PopupMenu
import androidx.navigation.fragment.NavHostFragment
import androidx.navigation.ui.setupWithNavController
import com.example.myapplication.databinding.ActivityMainBinding
import com.google.android.material.bottomnavigation.BottomNavigationView
import java.io.FileInputStream

class MainActivity : AppCompatActivity() {

    private lateinit var appBarConfiguration: AppBarConfiguration
    private lateinit var binding: ActivityMainBinding

    private lateinit var openImageIntent : Intent

    val connection = ServerConnect.instance()

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
                R.id.signUp-> {
                    bottomNav.visibility = View.GONE
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

        //click register for making the popup
       findViewById<ImageButton>(R.id.user_avatar).setOnClickListener {
           val popup = PopupMenu(this, it)
           val inflater: MenuInflater = popup.menuInflater
           inflater.inflate(R.menu.menu_main, popup.menu)
           popup.show()
            //TODO: find a better place for this
           //startActivityForResult(openImageIntent, OPEN_IMAGE)
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
    //This is deprecated, but the docs don't offer a better solution
    @Deprecated("Deprecated in Java")
    override fun onActivityResult(requestCode: Int, resultCode: Int, returnIntent: Intent?) {
        super.onActivityResult(requestCode, resultCode, returnIntent)
        if (resultCode != RESULT_OK) {
            return
        }

        if (requestCode == OPEN_IMAGE) {
            val returnUri = returnIntent?.data ?: return
            val pfd = contentResolver.openFileDescriptor(returnIntent!!.data!!, "r")
            val fis = FileInputStream(pfd!!.fileDescriptor)
            connection.sendImageToServer(fis)
        }

    }

    override fun onSupportNavigateUp(): Boolean {
        val navController = findNavController(R.id.nav_host_fragment_activity_main)
        return navController.navigateUp(appBarConfiguration)
                || super.onSupportNavigateUp()
    }

    companion object {
        private const val OPEN_IMAGE = 56
    }
}