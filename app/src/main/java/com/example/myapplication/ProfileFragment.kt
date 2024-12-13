package com.example.myapplication

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import androidx.navigation.fragment.findNavController
import com.example.myapplication.databinding.FragmentProfileBinding

class ProfileFragment : Fragment() {
    private var _binding: FragmentProfileBinding? = null
    private val binding get() = _binding!!

    private val profileViewModel: ProfileViewModel by activityViewModels()

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentProfileBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        profileViewModel.username.observe(viewLifecycleOwner) {
            binding.username.text = it
        }
        profileViewModel.bio.observe(viewLifecycleOwner) {
            binding.userBio.text = it
        }
        profileViewModel.height.observe(viewLifecycleOwner) {
            binding.heightValue.text = it
        }
        profileViewModel.weight.observe(viewLifecycleOwner) {
            binding.weightValue.text = it
        }
        profileViewModel.sex.observe(viewLifecycleOwner) {
            binding.sexValue.text = it
        }
        profileViewModel.calorieGoal.observe(viewLifecycleOwner) {
            binding.calorieValue.text = it
        }
        binding.btnEditProfile.setOnClickListener {
            findNavController().navigate(R.id.action_profileFragment_to_profileEdit)
        }

    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}