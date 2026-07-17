package com.ultra.vectorizer.infrastructure.telemetry

import android.util.Log
import com.ultra.vectorizer.domain.telemetry.ITelemetryService
import com.ultra.vectorizer.domain.telemetry.TelemetryEvent

class UltraTelemetryService : ITelemetryService {
    override fun track(event: TelemetryEvent) {
        when (event) {
            is TelemetryEvent.VectorizationStarted ->
                Log.i("UltraTelemetry", "START | Path: ${event.imagePath} | Precision: ${event.precision}")
            is TelemetryEvent.VectorizationCompleted ->
                Log.i("UltraTelemetry", "COMPLETED | ${event.durationMs}ms | Nodes: ${event.nodes}")
            is TelemetryEvent.Error ->
                Log.e("UltraTelemetry", "ERROR | CODE: ${event.code} | MSG: ${event.message}")
        }
    }
}
