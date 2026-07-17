# Ultra Vectorizer - ProGuard / R8 rules
# Mantener la clase que carga la librería nativa y su método JNI
-keep class com.ultra.vectorizer.data.engine.PotraceNativeBridge {
    *;
}
-keepclasseswithmembernames class * {
    native <methods>;
}
# Mantener sealed class de telemetría (evita que R8 las fusione/clasifique mal)
-keep class com.ultra.vectorizer.domain.telemetry.TelemetryEvent** { *; }
-keep class com.ultra.vectorizer.domain.VectorResult { *; }
# Conservar nombres de MethodChannel (no ofuscar strings del puente Flutter)
-keepclassmembers class * {
    @android.webkit.JavascriptInterface <methods>;
}
