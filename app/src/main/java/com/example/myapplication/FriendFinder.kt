package com.example.myapplication

import FriendData
import android.os.Bundle
import android.util.Log
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.SearchView
import android.widget.Toast
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.example.myapplication.databinding.FragmentSecondBinding
import java.util.*
import kotlin.collections.ArrayList

class FriendFinder : Fragment() {

    private var _binding: FragmentSecondBinding? = null
    private val binding get() = _binding!!

    private lateinit var recyclerView: RecyclerView
    private lateinit var searchView: androidx.appcompat.widget.SearchView
    private lateinit var mAdapter: FriendAdapter

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        val view = inflater.inflate(R.layout.fragment_add_friends, container, false)

        // Use the correct type for SearchView
        searchView = view.findViewById<androidx.appcompat.widget.SearchView>(R.id.searchView)
        recyclerView = view.findViewById(R.id.recyclerView)

        recyclerView.setHasFixedSize(true)
        recyclerView.layoutManager = LinearLayoutManager(requireContext())

        // Initialize and populate the list before setting the adapter
        addDataToList()

        // Initialize the adapter and attach it to RecyclerView
        mAdapter = FriendAdapter(mList)
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
        if (query != null) {
            val filteredList = ArrayList<FriendData>()
            for (friend in mList) {
                if (friend.userName.lowercase(Locale.getDefault()).contains(query)) {
                    filteredList.add(friend)
                }
            }
            if (filteredList.isEmpty()) {
                Toast.makeText(context, "No Friends Match", Toast.LENGTH_SHORT).show()
            } else {
                mAdapter.setFilteredList(filteredList)
            }
        }
    }

    private fun addDataToList() {
        mList.apply {
            add(FriendData("Avery"))
            add(FriendData("Alex"))
            add(FriendData("Corbin"))
            add(FriendData("Dominick"))
            add(FriendData("Emily"))
            add(FriendData("Jackson"))
            add(FriendData("James"))
            add(FriendData("Kailah"))
            add(FriendData("Rodger"))
            add(FriendData("Samantha"))
        }
        Log.d("FriendFinder", "Data added to list: $mList")
    }

    companion object {
        var mList = ArrayList<FriendData>()
    }

}
