package com.example.myapplication


import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.navigation.fragment.findNavController
import com.example.myapplication.databinding.FragmentChallengeCardViewBinding




class ChallengeCardView : Fragment() {

    private var _binding: FragmentChallengeCardViewBinding? = null
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentChallengeCardViewBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding.challengeAcceptButton.setOnClickListener {
            findNavController().navigate(R.id.GroupFragment)
        }

        binding.challengeDeclineButton.setOnClickListener {
            findNavController().navigate(R.id.GroupFragment)
        }

        val challenge = Shared.currentChallenge
        binding.challengeTitle.text = challenge?.game
        binding.challengeDescription.text = challenge?.description
        binding.conditionText.text = challenge?.condition
        binding.punishmentText.text = "Whoops, we forgot to add a punishment field to the ChallengeData object"
        //binding.

    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}
