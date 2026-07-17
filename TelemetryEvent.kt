package com.ultra.vectorizer.domain.telemetry

sealed class TelemetryEvent {
    data class VectorizationStarted(
        val imagePath: String,
        val precision: Float
    ) : TelemetryEvent()

    data class VectorizationCompleted(
        val durationMs: Long,
        val nodes: Int,
        val success: Boolean
    ) : TelemetryEvent()

    data class Error(
        val code: String,
        val message: String
    ) : TelemetryEvent()
}
