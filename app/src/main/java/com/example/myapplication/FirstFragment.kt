package com.example.myapplication

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import com.example.myapplication.databinding.FragmentFirstBinding

/**
 * A simple [Fragment] subclass as the default destination in the navigation.
 */
class FirstFragment : Fragment() {

    private var _binding: FragmentFirstBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {

        _binding = FragmentFirstBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        initFitnessHub(view)
    }

    private fun initFitnessHub(view : View)
    {
        updateText(view)
        updateModel()
    }

    private fun updateText(view : View)
    {
        val calories  : TextView = view.findViewById(R.id.textView_calories_burned_info)
        val workedOn  : TextView = view.findViewById(R.id.textView_worked_on_info)
        val timeSpent : TextView = view.findViewById(R.id.textView_time_spent_info)

        calories.text = ActiveUser.displayCaloriesBurned()
        workedOn.text = ActiveUser.displayPartsWorked()
        timeSpent.text = ActiveUser.displayTimeSpentExercising()
    }

    private fun updateModel()
    {}

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }


}