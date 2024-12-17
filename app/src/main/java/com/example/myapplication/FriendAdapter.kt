import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.cardview.widget.CardView
import androidx.recyclerview.widget.RecyclerView
import com.example.myapplication.R

class FriendAdapter(var mList : List<FriendData>) : RecyclerView.Adapter<FriendAdapter.FriendViewHolder>() {

    inner class FriendViewHolder(v : View) : RecyclerView.ViewHolder(v) {
        val username: TextView = v.findViewById(R.id.usernameVar)

    }

    fun setFilteredList(newList: List<InstructorData>) {

        notifyDataSetChanged() // Notify RecyclerView of data changes
    }
}




