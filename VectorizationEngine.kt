package com.ultra.vectorizer.data.engine

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import com.ultra.vectorizer.domain.VectorResult
import java.io.File
import java.util.concurrent.Executors
import java.util.concurrent.TimeUnit
import kotlinx.coroutines.*

class VectorizationEngine : AutoCloseable {

    private val nativeBridge = PotraceNativeBridge()
    private val executor = Executors.newFixedThreadPool(Runtime.getRuntime().availableProcessors())
    private val dispatcher = executor.asCoroutineDispatcher()

    suspend fun processImage(imagePath: String, precision: Float): VectorResult = withContext(dispatcher) {
        try {
            val file = File(imagePath)
            if (!file.exists()) {
                return@withContext VectorResult("", 0, 0, false, "File not found")
            }

            val decodeBounds = BitmapFactory.Options().apply {
                inJustDecodeBounds = true
            }
            BitmapFactory.decodeFile(imagePath, decodeBounds)
            if (decodeBounds.outWidth <= 0) {
                return@withContext VectorResult("", 0, 0, false, "Decode bounds failed — invalid image")
            }

            val sampleSize = calculateSampleSize(decodeBounds, 2048, 2048)
            val options = BitmapFactory.Options().apply {
                inSampleSize = sampleSize
                inJustDecodeBounds = false
                inPreferredConfig = Bitmap.Config.ARGB_8888
            }

            val bitmap = BitmapFactory.decodeFile(imagePath, options)
                ?: return@withContext VectorResult("", 0, 0, false, "Bitmap decode returned null")

            val start = System.currentTimeMillis()
            val svg = nativeBridge.nativeTrace(bitmap, precision)
            val duration = System.currentTimeMillis() - start

            bitmap.recycle()

            VectorResult(svg, 0, duration, true) // nodes extraídos por wrapper y embebidos en SVG
        } catch (e: Exception) {
            VectorResult("", 0, 0, false, e.message)
        }
    }

    private fun calculateSampleSize(options: BitmapFactory.Options, reqW: Int, reqH: Int): Int {
        var sample = 1
        while ((options.outHeight / sample) > reqH || (options.outWidth / sample) > reqW) {
            sample *= 2
        }
        return sample
    }

    override fun close() {
        executor.shutdown()
        try {
            if (!executor.awaitTermination(2, TimeUnit.SECONDS)) {
                executor.shutdownNow()
            }
        } catch (e: Exception) {
            executor.shutdownNow()
        }
    }
}
