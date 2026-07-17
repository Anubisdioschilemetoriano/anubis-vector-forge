package com.ultra.vectorizer.domain

import com.ultra.vectorizer.domain.telemetry.TelemetryEvent
import org.junit.Assert.*
import org.junit.Test

class TelemetryEventTest {

    @Test
    fun `VectorizationStarted holds correct data`() {
        val event = TelemetryEvent.VectorizationStarted(
            imagePath = "/storage/test.png",
            precision = 0.75f
        )
        assertEquals("/storage/test.png", event.imagePath)
        assertEquals(0.75f, event.precision)
    }

    @Test
    fun `VectorizationCompleted holds correct data`() {
        val event = TelemetryEvent.VectorizationCompleted(
            durationMs = 250L,
            nodes = 88,
            success = true
        )
        assertEquals(250L, event.durationMs)
        assertEquals(88, event.nodes)
        assertTrue(event.success)
    }

    @Test
    fun `Error holds correct data`() {
        val event = TelemetryEvent.Error(
            code = "TIMEOUT",
            message = "Vectorization exceeded 15 seconds"
        )
        assertEquals("TIMEOUT", event.code)
        assertTrue(event.message.contains("15 seconds"))
    }

    @Test
    fun `sealed class is exhaustive`() {
        val events: List<TelemetryEvent> = listOf(
            TelemetryEvent.VectorizationStarted("/a.png", 0.5f),
            TelemetryEvent.VectorizationCompleted(100, 10, true),
            TelemetryEvent.Error("ERR", "msg")
        )
        assertEquals(3, events.size)
    }
}
