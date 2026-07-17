package com.ultra.vectorizer.domain

import com.ultra.vectorizer.data.engine.VectorizationEngine
import com.ultra.vectorizer.domain.telemetry.ITelemetryService
import com.ultra.vectorizer.domain.telemetry.TelemetryEvent
import io.mockk.*
import kotlinx.coroutines.test.runTest
import org.junit.Assert.*
import org.junit.Before
import org.junit.Test

class VectorizeUseCaseTest {

    private lateinit var engine: VectorizationEngine
    private lateinit var telemetry: ITelemetryService
    private lateinit var useCase: VectorizeUseCase

    @Before
    fun setUp() {
        engine = mockk(relaxed = true)
        telemetry = mockk(relaxed = true)
        useCase = VectorizeUseCase(engine, telemetry)
    }

    @Test
    fun `execute succesful vectorization returns correct result`() = runTest {
        val expected = VectorResult(
            svg = "<svg><!-- nodes=42 --></svg>",
            nodes = 42,
            timeMs = 120,
            success = true
        )
        coEvery { engine.processImage("/tmp/test.png", 0.95f) } returns expected

        val result = useCase.execute("/tmp/test.png", 0.95f)

        assertEquals(true, result.success)
        assertEquals(42, result.nodes)
        assertEquals(120L, result.timeMs)
        assertEquals("<svg><!-- nodes=42 --></svg>", result.svg)

        verify {
            telemetry.track(match<TelemetryEvent.VectorizationStarted> {
                it.imagePath == "/tmp/test.png" && it.precision == 0.95f
            })
            telemetry.track(match<TelemetryEvent.VectorizationCompleted> {
                it.durationMs == 120L && it.nodes == 42 && it.success
            })
        }
    }

    @Test
    fun `execute failed vectorization tracks error`() = runTest {
        val failed = VectorResult("", 0, 0, false, "Decode failed")
        coEvery { engine.processImage("/tmp/bad.jpg", 0.5f) } returns failed

        val result = useCase.execute("/tmp/bad.jpg", 0.5f)

        assertFalse(result.success)
        assertEquals("Decode failed", result.error)

        verify {
            telemetry.track(match<TelemetryEvent.VectorizationStarted> {
                it.imagePath == "/tmp/bad.jpg"
            })
            telemetry.track(match<TelemetryEvent.Error> {
                it.code == "VECTORIZATION_FAILED" && it.message == "Decode failed"
            })
        }
    }

    @Test
    fun `execute with empty path still tracked`() = runTest {
        val failed = VectorResult("", 0, 0, false, "File not found")
        coEvery { engine.processImage("", 0.0f) } returns failed

        val result = useCase.execute("", 0.0f)

        assertFalse(result.success)
        verify(atLeast = 2) { telemetry.track(any()) }
    }
}
