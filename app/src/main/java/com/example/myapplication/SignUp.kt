package com.example.myapplication

import ServerConnect
import android.content.Context
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.navigation.fragment.findNavController
import com.example.myapplication.databinding.FragmentSignUpBinding

class SignUp : Fragment() {

    private var _binding: FragmentSignUpBinding? = null
    private val binding get() = _binding!!

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentSignUpBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding.signupButton.setOnClickListener {
            val email = binding.emailInput.text.toString()
            val password = binding.passwordInput.text.toString()
            val name = binding.nameInput.text.toString()
            val passwordCheck = binding.reenterPasswordInput.text.toString()
            val connection = ServerConnect

            if (name.isNotEmpty() && email.isNotEmpty() && password.isNotEmpty() && passwordCheck.isNotEmpty()) {
                if (password == passwordCheck) {
                    if (password.length in 8..59) {
                        try {
                            val sharedPreferences =
                                requireContext().getSharedPreferences("UserPrefs", Context.MODE_PRIVATE)
                            val editor = sharedPreferences.edit()
                            editor.putString("email", email)
                            editor.putString("password", password)
                            editor.putString("name", name)
                            editor.putString("passwordCheck", passwordCheck)
                            editor.apply()

                            ActiveUser.name = name
                            ActiveUser.username = email

                            connection.signUp(name, email, email, password)

                            findNavController().navigate(R.id.profileEdit)
                        } catch (e: Exception) {
                            e.printStackTrace()
                            Toast.makeText(context, "An error occurred: ${e.message}", Toast.LENGTH_SHORT).show()
                        }
                    } else {
                        Toast.makeText(context, "Passwords must be between 8 and 60 characters.", Toast.LENGTH_SHORT).show()
                    }
                } else {
                    Toast.makeText(context, "Passwords do not match.", Toast.LENGTH_SHORT).show()
                }
            } else {
                Toast.makeText(context, "Please fill out all fields.", Toast.LENGTH_SHORT).show()
            }
        }

        binding.backButton.setOnClickListener {
            findNavController().navigate(R.id.LoginFragment)
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}
