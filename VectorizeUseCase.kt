package com.ultra.vectorizer.domain

import com.ultra.vectorizer.data.engine.VectorizationEngine
import com.ultra.vectorizer.domain.telemetry.ITelemetryService
import com.ultra.vectorizer.domain.telemetry.TelemetryEvent

class VectorizeUseCase(
    private val engine: VectorizationEngine,
    private val telemetry: ITelemetryService
) {
    suspend fun execute(path: String, precision: Float): VectorResult {
        telemetry.track(TelemetryEvent.VectorizationStarted(path, precision))
        
        val result = engine.processImage(path, precision)
        
        if (result.success) {
            telemetry.track(TelemetryEvent.VectorizationCompleted(result.timeMs, result.nodes, true))
        } else {
            telemetry.track(TelemetryEvent.Error("VECTORIZATION_FAILED", result.error ?: "Unknown error"))
        }
        
        return result
    }
}
