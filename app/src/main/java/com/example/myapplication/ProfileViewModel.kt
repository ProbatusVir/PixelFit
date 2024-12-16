package com.example.myapplication

import android.content.Context
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import org.json.JSONObject
import java.io.File
import java.io.FileWriter
import java.io.IOException

class ProfileViewModel : ViewModel() {

    private val _username = MutableLiveData<String>()
    val username: LiveData<String> get() = _username

    private val _bio = MutableLiveData<String>()
    val bio: LiveData<String> get() = _bio

    private val _height = MutableLiveData<String>()
    val height: LiveData<String> get() = _height

    private val _weight = MutableLiveData<String>()
    val weight: LiveData<String> get() = _weight

    private val _sex = MutableLiveData<String>()
    val sex: LiveData<String> get() = _sex

    private val _calorieGoal = MutableLiveData<String>()
    val calorieGoal: LiveData<String> get() = _calorieGoal

    fun updateProfile(username: String, bio: String, height: String, weight: String, sex: String, calorieGoal: String) {
        _username.value = username
        _bio.value = bio
        _height.value = height
        _weight.value = weight
        _sex.value = sex
        _calorieGoal.value = calorieGoal
    }

    fun saveProfileToJson(context: Context, fileName: String) {
        val jsonObject = JSONObject()
        jsonObject.put("username", _username.value)
        jsonObject.put("bio", _bio.value)
        jsonObject.put("height", _height.value)
        jsonObject.put("weight", _weight.value)
        jsonObject.put("sex", _sex.value)
        jsonObject.put("calorieGoal", _calorieGoal.value)

        val file = File(context.filesDir, fileName)
        try {
            val writer = FileWriter(file)
            writer.write(jsonObject.toString())
            writer.close()
        } catch (ex: IOException) {
            ex.printStackTrace()
        }
    }

    fun loadProfileFromJson(context: Context, fileName: String) {
        val file = File(context.filesDir, fileName)
        if (!file.exists()) return
        try {
            val jsonString = file.readText(Charsets.UTF_8)
            val jsonObject = JSONObject(jsonString)
            _username.value = jsonObject.optString("username", "Unknown")
            _bio.value = jsonObject.optString("bio", "")
            _height.value = jsonObject.optString("height", "")
            _weight.value = jsonObject.optString("weight", "")
            _sex.value = jsonObject.optString("sex", "")
            _calorieGoal.value = jsonObject.optString("calorieGoal", "")
        } catch (ex: IOException) {
            ex.printStackTrace()
        }
    }
}
