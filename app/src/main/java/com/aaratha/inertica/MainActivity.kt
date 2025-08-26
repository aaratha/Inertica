package com.aaratha.inertica

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import com.aaratha.inertica.databinding.ActivityMainBinding



class MainActivity : AppCompatActivity() {

    /**
     * A native method that is implemented by the 'inertica' native library,
     * which is packaged with this application.
     */

    companion object {
        // Used to load the 'inertica' library on application startup.
        init {
            System.loadLibrary("inertica")
        }
    }

    external fun nativeInit()
    external fun nativeUpdate()
    external fun nativePause()
    external fun nativeResume()

    private lateinit var binding: ActivityMainBinding

    private val updateHandler = android.os.Handler(android.os.Looper.getMainLooper())
    private val updateRunnable = object : Runnable {
        override fun run() {
            nativeUpdate()
            updateHandler.postDelayed(this, 16) // ~60 fps
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        System.loadLibrary("inertica")

        nativeInit()

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
    }


    override fun onResume() {
        super.onResume()
        nativeResume()
        updateHandler.post(updateRunnable)
    }

    override fun onPause() {
        super.onPause()
        nativePause()
        updateHandler.removeCallbacks(updateRunnable)
    }

}