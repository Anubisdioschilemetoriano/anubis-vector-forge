package com.ultra.vectorizer.domain

import org.junit.Assert.*
import org.junit.Test

class VectorResultTest {

    @Test
    fun `success result has expected fields`() {
        val result = VectorResult(
            svg = "<svg></svg>",
            nodes = 10,
            timeMs = 50,
            success = true
        )
        assertTrue(result.success)
        assertNull(result.error)
        assertEquals("<svg></svg>", result.svg)
    }

    @Test
    fun `failed result carries error message`() {
        val result = VectorResult(
            svg = "",
            nodes = 0,
            timeMs = 0,
            success = false,
            error = "Decode bounds failed"
        )
        assertFalse(result.success)
        assertEquals("Decode bounds failed", result.error)
    }

    @Test
    fun `copy preserves fields`() {
        val original = VectorResult("/test", 42, 150, true)
        val copy = original.copy(timeMs = 200)
        assertEquals(42, copy.nodes)
        assertEquals(200L, copy.timeMs)
    }
}
