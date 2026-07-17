package com.ultra.vectorizer.domain

data class VectorResult(
    val svg: String,
    val nodes: Int,
    val timeMs: Long,
    val success: Boolean,
    val error: String? = null
)
