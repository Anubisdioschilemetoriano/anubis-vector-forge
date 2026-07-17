package com.ultra.vectorizer.data.engine

import android.graphics.Bitmap
import android.util.Log

class PotraceNativeBridge {
    init {
        try {
            System.loadLibrary("ultra-potrace")
        } catch (e: UnsatisfiedLinkError) {
            Log.e("PotraceNative", "Native library 'ultra-potrace' not found!", e)
        }
    }

    external fun nativeTrace(bitmap: Bitmap, precision: Float): String
}
