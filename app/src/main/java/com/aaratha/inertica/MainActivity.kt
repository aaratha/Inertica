package com.aaratha.inertica

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import android.opengl.GLSurfaceView
import android.content.res.AssetManager
import javax.microedition.khronos.opengles.GL10
import javax.microedition.khronos.egl.EGLConfig



class MainActivity : AppCompatActivity() {

    companion object {
        // Used to load the 'inertica' library on application startup.
        init {
            System.loadLibrary("inertica")
        }
    }

    external fun nativeInit(assetManager: AssetManager)
    external fun nativePause()
    external fun nativeResume()

    external fun nativeSurfaceCreated()
    external fun nativeSurfaceChanged(width: Int, height: Int)
    external fun nativeDrawFrame()

    private lateinit var glSurfaceView: GLSurfaceView  // <-- declare it here


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        System.loadLibrary("inertica")

        glSurfaceView = GLSurfaceView(this)
        glSurfaceView.setEGLContextClientVersion(2)
        glSurfaceView.setRenderer(object : GLSurfaceView.Renderer {
            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
                nativeSurfaceCreated()
            }
            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) { nativeSurfaceChanged(width, height) }
            override fun onDrawFrame(gl: GL10?) { nativeDrawFrame() }
        })

        setContentView(glSurfaceView)  // set the GLSurfaceView as the root view
        nativeInit(assets)

    }

    override fun onPause() {
        super.onPause()
        nativePause()
        glSurfaceView.onPause()
    }

    override fun onResume() {
        super.onResume()
        nativeResume()
        glSurfaceView.onResume()
    }

}
