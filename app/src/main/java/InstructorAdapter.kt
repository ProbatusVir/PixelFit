import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.webkit.WebSettings
import android.webkit.WebView
import android.webkit.WebViewClient
import android.widget.TextView
import android.widget.VideoView
import androidx.cardview.widget.CardView
import androidx.recyclerview.widget.RecyclerView
import com.example.myapplication.R

class InstructorAdapter(var mList : List<InstructorData>) : RecyclerView.Adapter<InstructorAdapter.InstructorViewHolder>() {

    private var playerView : WebView? = null

    inner class InstructorViewHolder(v : View) : RecyclerView.ViewHolder(v) {
        val card : CardView = v.findViewById(R.id.firstCard)
        val video : WebView = v.findViewById(R.id.video1)
        val titleTv : TextView = v.findViewById(R.id.title)
        val description : TextView = v.findViewById(R.id.description)
//        val equipment : TextView = v.findViewById(R.id.equip)
//        val targets : TextView = v.findViewById(R.id.target)
//        val calories : TextView = v.findViewById(R.id.calorie)
    }

    fun setFilteredList(instructorList : List<InstructorData>) {
        this.mList = instructorList
        notifyDataSetChanged()
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): InstructorViewHolder {
        val view = LayoutInflater.from(parent.context).inflate(R.layout.instructor_layout, parent, false)
        return InstructorViewHolder(view)
    }

    override fun getItemCount(): Int {
        return mList.size
    }

    override fun onBindViewHolder(holder : InstructorViewHolder, position: Int) {
        val webSettings : WebSettings = holder.video.settings
        webSettings.javaScriptEnabled = true
        webSettings.cacheMode = WebSettings.LOAD_DEFAULT

        holder.video.webViewClient = WebViewClient()

        holder.video.loadUrl(mList[position].videoURL)
        //prepares the video
//        holder.video.setOnCompletionListener {
//            playerPos = null
//            playerView = null
//
//        }
        holder.titleTv.text = mList[position].title
        holder.description.text = mList[position].description
//        holder.equipment.text = mList[position].equipment
//        holder.targets.text = mList[position].targets
//        holder.calories.text = mList[position].calories

        holder.card.setOnClickListener {
            toggleVideoPlayback(holder.video)
//            if(playerPos != mList[position].videoURL.toInt()) {
//                playerView?.stopPlayback()
//                playerView?.visibility = View.GONE
//
//                playerPos = mList[position].videoURL.toInt()
//                playerView = holder.video
//
//                //Start playback
//                holder.video.visibility = View.VISIBLE
//                holder.video.start()
//            }
//            if(!holder.video.isPlaying){
//                holder.video.start()
//            }else{
//                holder.video.pause()
//            }
        }
    }
    private fun toggleVideoPlayback(webView: WebView) {
        if(playerView != null && playerView == webView) {
            webView.loadUrl("javascript:player.pauseVideo();")
            playerView = null
        }else{
            playerView?.loadUrl("javascript:player.pauseVideo();")
            webView.loadUrl("javascript:player.playVideo();")
            playerView = webView
            //webView.reload()
        }
    }
    fun pauseAll(){
        playerView?.loadUrl("javascript:player.pauseVideo();")
        playerView = null
    }
}