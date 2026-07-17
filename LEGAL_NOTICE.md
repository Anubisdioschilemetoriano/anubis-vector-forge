# Ultra Vectorizer — Aviso Legal

## Componentes y sus licencias

| Componente | Ubicación | Licencia | Restricción |
|---|---|---|---|
| Potrace vendor | `src/main/cpp/potrace/` | **GPLv2+** | Distribución pública requiere liberar código fuente |
| Wrapper JNI | `src/main/cpp/potrace_wrapper.cpp` | **GPLv2+** (derivado) | Ídem — linkea con Potrace |
| Plugin Kotlin | `src/main/java/com/ultra/vectorizer/` | **MIT** | Sin restricciones |
| Capa Flutter/Dart | `lib/` | **MIT** | Sin restricciones |
| Tests | `src/test/`, `test/` | **MIT** | Sin restricciones |

## Escenarios de uso

### ✅ Uso interno / prototipo / desarrollo privado
Sin obligaciones. La GPL solo se activa al **distribuir** el software.

### ⚠️ Distribución pública (Google Play, APK descargable, etc.)
Debes:
1. Incluir `LICENSE.txt` en la app (sección "Acerca de" o similar)
2. Publicar el código fuente de `potrace_core.cpp`, `potrace.h` y `potrace_wrapper.cpp` (ej. repo GitHub público)
3. La app puede ser gratuita o de pago — GPL no prohíbe uso comercial

### 🔄 Alternativa MIT
Si necesitas distribución comercial sin obligaciones GPL, solicita la generación de un tracer propio (marching squares + Ramer-Douglas-Peucker) bajo MIT. El resto del proyecto (Kotlin, Dart, tests) ya es MIT.
