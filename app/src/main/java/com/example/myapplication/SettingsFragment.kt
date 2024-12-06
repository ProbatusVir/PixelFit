package com.example.myapplication

import android.os.Bundle
import android.view.Gravity
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.PopupWindow
import androidx.fragment.app.Fragment
import androidx.core.content.ContextCompat
import com.example.myapplication.databinding.FragmentSettingsBinding
import ServerConnect
import androidx.navigation.fragment.findNavController

class SettingsFragment : Fragment() {

    private var _binding: FragmentSettingsBinding? = null
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {

        _binding = FragmentSettingsBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        val blurOverlay = view.findViewById<View>(R.id.blur_overlay)

        binding.buttonLogOut.setOnClickListener {
            findNavController().navigate(R.id.action_SettingsFragment_to_LoginFragment)
            ServerConnect.instance()?.logOut()
        }

        binding.backButton.setOnClickListener {
            findNavController().navigate(R.id.action_SettingsFragment_to_HomeFragment)
        }

        binding.buttonChangeUsername.setOnClickListener {
            showPopupWindow(blurOverlay)
        }
    }


    private fun showPopupWindow(blurOverlay: View) {
        // Inflate the popup layout
        val inflater = LayoutInflater.from(requireContext())
        val popupView = inflater.inflate(R.layout.fragment_change_username, null)

        // Create the PopupWindow
        val popupWindow = PopupWindow(
            popupView,
            ViewGroup.LayoutParams.WRAP_CONTENT,
            ViewGroup.LayoutParams.WRAP_CONTENT,
            true // Focusable
        )

        // Set a background drawable to allow outside click dismissal
        popupWindow.setBackgroundDrawable(
            ContextCompat.getDrawable(requireContext(), android.R.color.transparent)
        )

        // Show the blur overlay
        toggleView(blurOverlay)

        // Handle popup dismissal
        popupWindow.setOnDismissListener {
            toggleView(blurOverlay) // Hide the blur overlay when popup is dismissed
        }

        // Show the popup in the center of the screen
        popupWindow.showAtLocation(binding.root, Gravity.CENTER, 0, 0)

        // Set up the close button logic
        val closeButton: Button = popupView.findViewById(R.id.popup_button)
        closeButton.setOnClickListener {
            popupWindow.dismiss() // Close the popup
        }
    }


    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null // Avoid memory leaks
    }

    fun toggleView(view: View) {
        view.visibility = if (view.visibility == View.VISIBLE) View.GONE else View.VISIBLE
    }

}

