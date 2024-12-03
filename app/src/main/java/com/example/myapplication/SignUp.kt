package com.example.myapplication

import android.content.Context
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.navigation.fragment.findNavController
import com.example.myapplication.databinding.FragmentSecondBinding
import com.example.myapplication.databinding.FragmentSignUpBinding


class SignUp : Fragment() {

    private var _binding:FragmentSignUpBinding? = null

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
            val connection = ServerConnect.instance()

            if (name.isNotEmpty()&& email.isNotEmpty() && password.isNotEmpty() && passwordCheck.isNotEmpty() ) {

                if (password == passwordCheck) {

                    if (password.length > 7) {

                        if (password.length < 60) {


                            val sharedPreferences =
                                requireContext().getSharedPreferences("UserPrefs", Context.MODE_PRIVATE)
                            val editor = sharedPreferences.edit()
                            editor.putString("email", email)
                            editor.putString("password", password)
                            editor.putString("name", name)
                            editor.putString("passwordCheck", passwordCheck)
                            editor.apply()

                            //Fixme: This needs its own field.
                            val username = email

                            connection.signUp(name, username, email, password)

                            findNavController().navigate(R.id.action_signUp_to_HomeFragment)
                        }
                        else{
                            Toast.makeText(context, "Passwords must be less than 60 characters", Toast.LENGTH_SHORT).show()
                        }
                    }
                    else{
                        Toast.makeText(context, "Passwords must be at least 8 characters", Toast.LENGTH_SHORT).show()
                    }
                }
                else{
                    Toast.makeText(context, "Passwords do not match", Toast.LENGTH_SHORT).show()
                }
            } else {
                Toast.makeText(context, "Please fill out all fields", Toast.LENGTH_SHORT).show()
            }
        }

        binding.backButton.setOnClickListener {
            findNavController().navigate(R.id.action_signUp_to_LoginFragment2)
        }

    }


}