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
            add(ChallengeData("Fortnite Challenge", "Survive the storm, conquer the challenge! Are you fit enough to dominate the battlefield and save Tilted Towers?", 2342, "Be in the top 49 next match", ResourceType.RCHL, "5 minutes of push ups", 5, arrayOf(true, false, false, false).toBooleanArray(), 34))
            add(ChallengeData("Call of Duty", "Duty calls, soldier, will you answer?", 6513, "Get at least 15 kills in your next match.", ResourceType.RCHL, "100 choice-weight sit-ups",  5, arrayOf(false, false, true, false).toBooleanArray(), 25))
            add(ChallengeData("CS2", "Your team’s counting on you! Train hard, aim sharp, and dominate the challenge — this is your moment!", 1314, "Mountain Climbers", ResourceType.TCHL, "1 bicycle kicks", 1, arrayOf(false, true, true, false).toBooleanArray(),  4))
            add(ChallengeData("Valorant", "Ready to take control of the map and your fitness? Step up, agent—this challenge is yours to win!", 1223, "Calf Raises", ResourceType.RCHL, "50 calf-raises", 2, arrayOf(false, true, true, false).toBooleanArray(), 40))
            add(ChallengeData("Rocket League", "Drift. Dodge. Dominate. Pull yourself up to victory if you have to.", 8845, "Pull ups", ResourceType.RCHL, "10 pull ups", 3, arrayOf(true, false, true, false).toBooleanArray(), 11))
            add(ChallengeData("NBA 2K", "Train like a legend. Play like a champ.", 7946, "Weighted Curls", ResourceType.RCHL, "7-7-7-7 bicep curls", 3,  arrayOf(true, false, false, false).toBooleanArray(), 35))
            add(ChallengeData("MLB The Show", "Make it a home run or Coach will be angry!", 7745, "Plank", ResourceType.TCHL, "1 minute planking", 1,  arrayOf(false, false, true, false).toBooleanArray(), 5))
            add(ChallengeData("Overwatch", "Show no Mercy >:)", 1469, "Flutter Kicks", ResourceType.TCHL, "30 flutter kicks", 1, arrayOf(false, true, true, false).toBooleanArray(), 15))
            add(ChallengeData("Skyrim", "You thought the 7000 steps to High Hrothgar were bad?", 5445, "Bicycle Kicks", ResourceType.TCHL, "3 minutes of mountain-climbers",  3, arrayOf(false, true, true, false).toBooleanArray(), 30))
        }
        Log.d("FriendFinder", "Data added to list: $mList")
    }

    companion object {
        var mList = ArrayList<ChallengeData>()
    }

}
