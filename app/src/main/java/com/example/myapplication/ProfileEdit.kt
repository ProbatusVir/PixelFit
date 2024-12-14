package com.example.myapplication

import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.appcompat.app.AppCompatActivity.RESULT_OK
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import androidx.navigation.fragment.findNavController
import com.example.myapplication.databinding.FragmentProfileEditBinding
import java.io.File
import java.io.FileInputStream

class ProfileEdit : Fragment() {
    private var _binding: FragmentProfileEditBinding? = null
    private val binding get() = _binding!!

    private val profileViewModel: ProfileViewModel by activityViewModels()

    private lateinit var openImageIntent : Intent

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentProfileEditBinding.inflate(inflater, container, false)

        openImageIntent = Intent(Intent.ACTION_OPEN_DOCUMENT).apply {
            type = "image/png"
            addCategory(Intent.CATEGORY_OPENABLE)
            activity?.intent?.action = Intent.ACTION_GET_CONTENT
            activity?.intent?.addFlags(Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION)
        }

        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding.profileImage.setOnClickListener {
            startActivityForResult(openImageIntent, OPEN_IMAGE)
        }

        binding.btnSave.setOnClickListener {
            val username = binding.username.text.toString()
            val bio = binding.userBio.text.toString()
            val height = binding.heightSpinner.selectedItem.toString()
            val weight = binding.weightSpinner.selectedItem.toString()
            val sex = binding.sexSpinner.selectedItem.toString()
            val calorieGoal = binding.calorieSpinner.selectedItem.toString()

            profileViewModel.updateProfile(username, bio, height, weight, sex, calorieGoal)
            profileViewModel.saveProfileToJson(requireContext(), "profile.json")

            findNavController().navigate(R.id.action_profileEdit_to_profileFragment)
        }

        loadPfp()
    }


    @Deprecated("Deprecated in Java")
    override fun onActivityResult(requestCode: Int, resultCode: Int, returnIntent: Intent?) {
        super.onActivityResult(requestCode, resultCode, returnIntent)
        if (resultCode != RESULT_OK) {
            return
        } //
        if (requestCode == OPEN_IMAGE) {
            //Get the data
            val returnUri = returnIntent?.data ?: return
            val pfd = activity?.contentResolver?.openFileDescriptor(returnIntent.data!!, "r")
            val fis = FileInputStream(pfd?.fileDescriptor)
            val data = fis.readAllBytes()

            //Write the data
            val out = File(context?.filesDir, Shared.PROFILE_IMAGE_NAME)
            out.writeBytes(data)

            //Display the data
            loadPfp()
            val act : MainActivity = activity as MainActivity
            act.loadPfp()

        }
    }

    private fun loadPfp() {
        val pfp = binding.profileImage
        val uri = Shared.getPfpUri() ?: return

        pfp.setImageURI(null)
        pfp.setImageURI(uri)
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
        (activity as MainActivity).loadPfp()
    }

    companion object {
        const val OPEN_IMAGE = 56 //can be literally any distinct number
    }
}
