package com.ultra.vectorizer.presentation

import com.ultra.vectorizer.domain.VectorizeUseCase
import com.ultra.vectorizer.data.engine.VectorizationEngine
import com.ultra.vectorizer.domain.telemetry.ITelemetryService
import com.ultra.vectorizer.infrastructure.telemetry.UltraTelemetryService
import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import kotlinx.coroutines.*

class UltraVectorizerPlugin : FlutterPlugin, MethodChannel.MethodCallHandler {

    private lateinit var channel: MethodChannel
    private lateinit var useCase: VectorizeUseCase
    private lateinit var engine: VectorizationEngine
    private val pluginScope = CoroutineScope(SupervisorJob() + Dispatchers.Default)

    override fun onAttachedToEngine(binding: FlutterPlugin.FlutterPluginBinding) {
        engine = VectorizationEngine()
        val telemetry: ITelemetryService = UltraTelemetryService()
        useCase = VectorizeUseCase(engine, telemetry)

        channel = MethodChannel(binding.binaryMessenger, "com.ultra.vectorizer/engine")
        channel.setMethodCallHandler(this)
    }

    override fun onMethodCall(call: MethodCall, result: MethodChannel.Result) {
        if (call.method != "vectorizeImage") {
            result.notImplemented()
            return
        }

        val args = call.arguments as? Map<*, *>
        val path = args?.get("path") as? String
        val precision = (args?.get("precision") as? Double)?.toFloat() ?: 1.0f

        if (path == null) {
            result.error("INVALID_ARGUMENT", "Parameter 'path' is required", null)
            return
        }

        pluginScope.launch {
            try {
                val data = withTimeout(15000) { useCase.execute(path, precision) }

                withContext(Dispatchers.Main) {
                    if (data.success) {
                        result.success(mapOf(
                            "svg" to data.svg,
                            "nodes" to data.nodes,
                            "time" to data.timeMs
                        ))
                    } else {
                        result.error("VECTORIZATION_FAILED", data.error, null)
                    }
                }
            } catch (e: CancellationException) {
                withContext(Dispatchers.Main) {
                    result.error("TIMEOUT", "Vectorization exceeded 15 seconds", null)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    result.error("SYSTEM_ERROR", e.message, null)
                }
            }
        }
    }

    override fun onDetachedFromEngine(binding: FlutterPlugin.FlutterPluginBinding) {
        channel.setMethodCallHandler(null)
        pluginScope.cancel()
        engine.close()
    }
}
