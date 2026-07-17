package com.ultra.vectorizer.domain.telemetry

interface ITelemetryService {
    fun track(event: TelemetryEvent)
}
