package com.example.myapplication

import InstructorAdapter
import InstructorData
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView

class Instructor : Fragment() {

    private lateinit var recyclerView: RecyclerView
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
        recyclerView = view.findViewById(R.id.recycler_view)
        recyclerView.setHasFixedSize(true)
        recyclerView.layoutManager = LinearLayoutManager(requireContext())

        addDataToList()

        mAdapter = InstructorAdapter(mList)
        recyclerView.adapter = mAdapter

        return view
    }

    private fun addDataToList() {
        //mList.add(InstructorData("Title", "description", "https://www.youtube.com/watch?v=IODxDxX7oi4"))
    }
}