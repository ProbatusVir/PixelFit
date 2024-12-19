package com.example.myapplication
import FriendData
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView

class FriendAdapter(var mList : List<FriendData>) : RecyclerView.Adapter<FriendAdapter.FriendViewHolder>() {

    inner class FriendViewHolder(v : View) : RecyclerView.ViewHolder(v) {
        val username: TextView = v.findViewById(R.id.usernameVar)
    }

    fun setFilteredList(friendList: List<FriendData>) {
        this.mList = friendList
        notifyDataSetChanged()
    }

    override fun getItemCount(): Int {
        return mList.size
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): FriendViewHolder {
        val view = LayoutInflater.from(parent.context).inflate(R.layout.friendprofile, parent, false)
        return FriendViewHolder(view)
    }

    override fun onBindViewHolder(holder : FriendViewHolder, position: Int) {

        holder.username.text = mList[position].userName
    }
}




