package com.example.myapplication

import FriendData
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageButton
import android.widget.TextView
import android.widget.Toast
import androidx.recyclerview.widget.RecyclerView

class FriendAdapter(private var mList: List<FriendData>) : RecyclerView.Adapter<FriendAdapter.FriendViewHolder>() {

    inner class FriendViewHolder(view: View) : RecyclerView.ViewHolder(view) {
        val username: TextView = view.findViewById(R.id.usernameVar)
        val addFriendButton: ImageButton = view.findViewById(R.id.addFriendButton) // Reference the button
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): FriendViewHolder {
        val view = LayoutInflater.from(parent.context).inflate(R.layout.friendprofile, parent, false)
        return FriendViewHolder(view)
    }

    override fun onBindViewHolder(holder: FriendViewHolder, position: Int) {
        val friend = mList[position]
        holder.username.text = friend.userName

        // Set a click listener for the button
        holder.addFriendButton.setOnClickListener {
            Toast.makeText(
                holder.itemView.context,
                "Friend request sent to ${friend.userName}",
                Toast.LENGTH_SHORT
            ).show()

            // Add any additional logic here (e.g., send a network request)
        }
    }

    override fun getItemCount(): Int = mList.size

    fun setFilteredList(friendList: List<FriendData>) {
        this.mList = friendList
        notifyDataSetChanged()
    }
}




