package com.ultra.vectorizer.infrastructure.telemetry

import com.ultra.vectorizer.domain.telemetry.TelemetryEvent
import org.junit.Assert.*
import org.junit.Test

class UltraTelemetryServiceTest {

    @Test
    fun `track all event types without crash`() {
        val service = UltraTelemetryService()

        service.track(TelemetryEvent.VectorizationStarted("/x.png", 0.9f))
        service.track(TelemetryEvent.VectorizationCompleted(300, 55, true))
        service.track(TelemetryEvent.Error("TEST", "all good"))

        // Si no crashea, pasa. El test verifica robustez de la implementación.
        assertTrue(true)
    }

    @Test
    fun `service is instantiable`() {
        val service = UltraTelemetryService()
        assertNotNull(service)
    }
}
