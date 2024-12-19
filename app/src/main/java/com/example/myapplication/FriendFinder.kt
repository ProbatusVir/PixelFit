package com.example.myapplication

import FriendData
import InstructorData
import ServerConnect
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

class FriendFinder : Fragment() {

    private lateinit var recyclerView: RecyclerView
    private lateinit var searchView: SearchView
    private lateinit var cardView: CardView
    private lateinit var mAdapter: FriendAdapter

//    override fun onCreate(savedInstanceState: Bundle?) {
//        super.onCreate(savedInstanceState)
//
//    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        val view = inflater.inflate(R.layout.fragment_add_friends, container, false)
        searchView = view.findViewById(R.id.searchView)
        recyclerView = view.findViewById(R.id.recyclerView)
        recyclerView.setHasFixedSize(true)
        recyclerView.layoutManager = LinearLayoutManager(requireContext())

        if (!Shared.directories.contains(DIRECTORY))
            loadDataFromServer()
        addDataToList()

        mAdapter = FriendAdapter(mList)
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
            val filterdList = ArrayList<FriendData>()
            for(i in mList){
                if(i.userName.lowercase(Locale.ROOT).contains(query)){
                    filterdList.add(i)
                }
            }
            if(filterdList.isEmpty()) {
                Toast.makeText(context, "No Friends Match", Toast.LENGTH_SHORT).show()
            }else{
                mAdapter.setFilteredList(filterdList)
            }
        }
    }

    private fun addDataToList() {
        mList.add(FriendData("Avery"))
        mList.add(FriendData("Alex"))
        mList.add(FriendData("Corbin"))
        mList.add(FriendData("James"))
        mList.add(FriendData("Rodger"))
        mAdapter.notifyDataSetChanged()
    }

    fun loadDataFromServer() {
        if (!(ServerConnect.connected() && ServerConnect.authenticated())) return
        if (Shared.directories.containsKey(DIRECTORY)) return

        val connection = ServerConnect

        connection.requestData(DIRECTORY, ResourceType.DIR)
        while (!Shared.directories.containsKey(DIRECTORY)) {/* no-op */} //just to halt the thread

        val directoryList = Shared.directories[DIRECTORY]!!
        for (dir in directoryList) {
            connection.requestData("$DIRECTORY/$dir", ResourceType.WORK)
        }

        val requests = directoryList.size
        while (mList.size != requests) { /* no-op */}
    }

    companion object {
        const val DIRECTORY = "workouts"
        var mList = ArrayList<FriendData>()
    }
}