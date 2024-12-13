package com.example.myapplication

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import androidx.navigation.fragment.findNavController
import com.example.myapplication.databinding.FragmentProfileEditBinding

class ProfileEdit : Fragment() {
    private var _binding: FragmentProfileEditBinding? = null
    private val binding get() = _binding!!

    private val profileViewModel: ProfileViewModel by activityViewModels()

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentProfileEditBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding.btnSave.setOnClickListener {
            val username = binding.username.text.toString()
            val bio = binding.userBio.text.toString()
            val height = binding.heightSpinner.selectedItem.toString()
            val weight = binding.weightSpinner.selectedItem.toString()
            val sex = binding.sexSpinner.selectedItem.toString()
            val calorieGoal = binding.calorieSpinner.selectedItem.toString()

            profileViewModel.updateProfile(username, bio, height, weight, sex, calorieGoal)
            profileViewModel.saveProfileToJson(requireContext(), "profile.json")

            findNavController().navigate(R.id.action_profileEdit_to_profileFragment)
        }

        binding.backButton.setOnClickListener {
            findNavController().navigate(R.id.profileFragment)
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}
