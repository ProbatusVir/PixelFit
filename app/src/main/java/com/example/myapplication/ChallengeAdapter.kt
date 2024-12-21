package com.example.myapplication

import ChallengeData
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.navigation.findNavController
import androidx.recyclerview.widget.RecyclerView

class ChallengeAdapter(private var mList: List<ChallengeData>) : RecyclerView.Adapter<ChallengeAdapter.ChallengeViewHolder>() {

    inner class ChallengeViewHolder(view: View) : RecyclerView.ViewHolder(view) {
        val gameTitle: TextView = view.findViewById(R.id.gameTitle)
        val challengeDescription: TextView = view.findViewById(R.id.challengeDescription)
        val roomId: TextView = view.findViewById(R.id.challengeID)

    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ChallengeViewHolder {
        val view = LayoutInflater.from(parent.context).inflate(R.layout.challenge_finder_card, parent, false)
        return ChallengeViewHolder(view)
    }

    override fun onBindViewHolder(holder: ChallengeViewHolder, position: Int) {
        val challenge = mList[position]

        // Bind data to views
        holder.gameTitle.text = challenge.game
        holder.challengeDescription.text = challenge.description
        holder.roomId.text = "Room ID: ${challenge.roomID}"

        // Handle button click (optional)
        holder.itemView.findViewById<Button>(R.id.acceptButton).setOnClickListener {
            holder.itemView.findNavController().navigate(R.id.challengeCardView)
            Toast.makeText(holder.itemView.context, "View Challenge: ${challenge.game}", Toast.LENGTH_SHORT).show()
        }
    }


    override fun getItemCount(): Int = mList.size

    fun setFilteredList(challengeList: List<ChallengeData>) {
        this.mList = challengeList
        notifyDataSetChanged()
    }
}
