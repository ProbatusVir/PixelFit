package com.example.myapplication

import InstructorAdapter
import InstructorData
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.SearchView
import android.widget.Toast
import androidx.cardview.widget.CardView
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import java.util.*
import kotlin.collections.ArrayList

class Instructor : Fragment() {

    private lateinit var recyclerView: RecyclerView
    private lateinit var searchView: SearchView
    private lateinit var cardView: CardView
    private var mList = ArrayList<InstructorData>()
    private lateinit var mAdapter: InstructorAdapter

//    override fun onCreate(savedInstanceState: Bundle?) {
//        super.onCreate(savedInstanceState)
//
//    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        val view = inflater.inflate(R.layout.fragment_instructor, container, false)
        searchView = view.findViewById(R.id.search_view)
        recyclerView = view.findViewById(R.id.recycler_view)
        recyclerView.setHasFixedSize(true)
        recyclerView.layoutManager = LinearLayoutManager(requireContext())

        addDataToList()

        mAdapter = InstructorAdapter(mList)
        recyclerView.adapter = mAdapter

        searchView.setOnQueryTextListener(object : SearchView.OnQueryTextListener {
            override fun onQueryTextSubmit(query: String?): Boolean {
                return false
            }

            override fun onQueryTextChange(newText: String?): Boolean {
                filterList(newText?.toLowerCase(Locale.getDefault()))
                return true
            }
        })

        return view
    }

    private fun filterList(query : String?) {
        if(query != null) {
            val filterdList = ArrayList<InstructorData>()
            for(i in mList){
                if(i.title.lowercase(Locale.ROOT).contains(query)){
                    filterdList.add(i)
                }
            }
            if(filterdList.isEmpty()) {
                Toast.makeText(context, "No Instructors found", Toast.LENGTH_SHORT).show()
            }else{
                mAdapter.setFilteredList(filterdList)
            }
        }
    }

    private fun addDataToList() {
        mList.add(InstructorData("Push Up",
            "a bodyweight exercise that targets the chest, shoulders, triceps, and core muscles.",
            "https://www.youtube.com/watch?v=IODxDxX7oi4"))
        mList.add(InstructorData("Sit ups", "description", ""))
        mList.add(InstructorData("Squats", "description", ""))
        mList.add(InstructorData("Plank", "description", ""))
        mList.add(InstructorData("Lunges", "description", ""))
    }
}