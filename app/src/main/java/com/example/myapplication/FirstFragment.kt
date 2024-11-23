package com.example.myapplication

import ActiveUser
import ActiveUser.partsWorked
import android.graphics.Color
import android.graphics.drawable.Drawable
import android.graphics.drawable.LayerDrawable
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
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
        updateModel(view)
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

    private fun updateModel(view : View)
    {
        val context = activity!!.applicationContext

        if (context != null) {

            val guy = view.findViewById<ImageView>(R.id.fitness_model).drawable as LayerDrawable
            val arms = guy.findDrawableByLayerId(R.id.fitness_arms)
            val legs = guy.findDrawableByLayerId(R.id.fitness_legs)
            val torso = guy.findDrawableByLayerId(R.id.fitness_torso)
            val head = guy.findDrawableByLayerId(R.id.fitness_head)

            setBodyPartColor(arms, ActiveUser.Parts.Arms)
            setBodyPartColor(legs, ActiveUser.Parts.Legs)
            setBodyPartColor(torso, ActiveUser.Parts.Torso)
            setBodyPartColor(head, ActiveUser.Parts.Head)
        }
    }

    private fun setBodyPartColor(layer : Drawable, part : ActiveUser.Parts)
    {
        if (partsWorked[part.ordinal])
            layer.setTint(Color.RED)
        else
            layer.setTint(Color.WHITE)
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }


}