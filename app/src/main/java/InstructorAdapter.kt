import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import android.widget.VideoView
import androidx.recyclerview.widget.RecyclerView
import com.example.myapplication.R

class InstructorAdapter(var mList : List<InstructorData>) : RecyclerView.Adapter<InstructorAdapter.InstructorViewHolder>() {

    inner class InstructorViewHolder(v : View) : RecyclerView.ViewHolder(v) {
        val video : VideoView = v.findViewById(R.id.video1)
        val titleTv : TextView = v.findViewById(R.id.title)
        val description : TextView = v.findViewById(R.id.description)
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): InstructorViewHolder {
        val view = LayoutInflater.from(parent.context).inflate(R.layout.fragment_instructor, parent, false)
        return InstructorViewHolder(view)
    }

    override fun getItemCount(): Int {
        return mList.size
    }

    override fun onBindViewHolder(holder: InstructorViewHolder, position: Int) {
        holder.video.setVideoPath(mList[position].videoURL)
        holder.titleTv.text = mList[position].title
        holder.description.text = mList[position].description
    }
}