package com.example.myapplication

import ChallengeData
import android.os.Bundle
import android.util.Log
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import java.util.*
import kotlin.collections.ArrayList
import ServerConnect

class ChallengeFinder : Fragment() {

    private lateinit var recyclerView: RecyclerView
    private lateinit var searchView: androidx.appcompat.widget.SearchView
    private lateinit var mAdapter: ChallengeAdapter


    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        val view = inflater.inflate(R.layout.challenge_finder, container, false)

        // Use the correct type for SearchView
        searchView = view.findViewById<androidx.appcompat.widget.SearchView>(R.id.searchView)
        recyclerView = view.findViewById(R.id.challengesRecyclerView)

        recyclerView.setHasFixedSize(true)
        recyclerView.layoutManager = LinearLayoutManager(requireContext())

        // Initialize and populate the list before setting the adapter
        addDataToList()

        // Initialize the adapter and attach it to RecyclerView
        mAdapter = ChallengeAdapter(mList)
        recyclerView.adapter = mAdapter

        searchView.setOnQueryTextListener(object : androidx.appcompat.widget.SearchView.OnQueryTextListener {
            override fun onQueryTextSubmit(query: String?): Boolean {
                return false
            }

            override fun onQueryTextChange(newText: String?): Boolean {
                filterList(newText?.lowercase(Locale.getDefault()))
                return true
            }
        })

        return view
    }


    private fun filterList(query: String?) {

    }

    private fun addDataToList() {
        mList.apply {
            add(ChallengeData("Fortnite Challenge", "Random description", 2342, "Push-ups", ResourceType.RCHL ))
            add(ChallengeData("Call of Duty", "Random description", 6513, "Sit-ups", ResourceType.RCHL ))
            add(ChallengeData("CS2", "Random description", 1314, "Mountain Climbers", ResourceType.TCHL ))
            add(ChallengeData("Valorant", "Random description", 1223, "Calf Raises", ResourceType.RCHL ))
            add(ChallengeData("Rocket League", "Random description jalskjbfaslkfjbaslkfbsalkfbaslfkba asfsfasfasf aesfasfasfs lshbflashkfbasf", 8845, "Pull ups", ResourceType.RCHL ))
            add(ChallengeData("NBA 2K", "Random description", 7946, "Weighted Curls", ResourceType.RCHL ))
            add(ChallengeData("MLB The Show", "Random description", 7745, "Plank", ResourceType.TCHL ))
            add(ChallengeData("Overwatch", "Random description", 1469, "Flutter Kicks", ResourceType.TCHL ))
            add(ChallengeData("Skyrim", "Random description", 5445, "Bicycle Kicks", ResourceType.TCHL ))
        }
        Log.d("FriendFinder", "Data added to list: $mList")
    }

    companion object {
        var mList = ArrayList<ChallengeData>()
    }

}
