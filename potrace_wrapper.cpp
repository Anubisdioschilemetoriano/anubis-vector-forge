/* ----------------------------------------------------------------------------
   potrace_wrapper.cpp — Puente JNI entre Flutter/Kotlin y el motor Potrace

   Pipeline real:
   1. Recibe jobject Bitmap desde Kotlin
   2. Lee píxeles vía AndroidBitmap_lockPixels
   3. Convierte RGBA_8888 a 1-bit (binarización luma + umbral derivado de precision)
   4. Construye potrace_bitmap_t y llama potrace_trace()
   5. Extrae SVG y lo devuelve como jstring a Kotlin → Flutter

   El SVG contiene <!-- nodes=N --> para que el cliente Dart/Kotlin
   pueda extraer el conteo de nodos real.
   ---------------------------------------------------------------------------- */

#include <jni.h>
#include <android/bitmap.h>
#include <android/log.h>
#include <string>
#include "potrace/potrace.h"

#define LOG_TAG "PotraceJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C"
JNIEXPORT jstring JNICALL
Java_com_ultra_vectorizer_data_engine_PotraceNativeBridge_nativeTrace(
        JNIEnv* env,
        jobject /* this */,
        jobject bitmap,
        jfloat precision) {

    // ---------- Paso 1: Obtener info del bitmap ----------
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
        LOGE("AndroidBitmap_getInfo failed");
        return env->NewStringUTF(
            "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"200\" height=\"200\">"
            "<rect width=\"200\" height=\"200\" fill=\"#FF0000\"/>"
            "</svg>");
    }

    LOGI("Bitmap: %dx%d format=%d stride=%d precision=%.3f",
         info.width, info.height, info.format, info.stride, precision);

    // ---------- Paso 2: Lock pixels y leer ----------
    void* pixels = nullptr;
    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) {
        LOGE("AndroidBitmap_lockPixels failed");
        return env->NewStringUTF(
            "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"200\" height=\"200\">"
            "<rect width=\"200\" height=\"200\" fill=\"#FF0000\"/>"
            "</svg>");
    }

    int w = info.width;
    int h = info.height;

    // ---------- Paso 3: Binarización ----------
    // Umbral derivado de precision: 0.0→0 (todo negro), 1.0→255 (solo blanco puro)
    // Valor típico usable: precision=0.5 → threshold=128
    int threshold = (int)(precision * 255.0f);
    if (threshold < 32) threshold = 32;   // floor
    if (threshold > 224) threshold = 224; // ceiling

    LOGI("Binarization threshold=%d (precision=%.3f)", threshold, precision);

    potrace_bitmap_t* bm = potrace_bitmap_new(w, h);
    if (!bm) {
        AndroidBitmap_unlockPixels(env, bitmap);
        LOGE("potrace_bitmap_new failed");
        return env->NewStringUTF(
            "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"200\" height=\"200\">"
            "<rect width=\"200\" height=\"200\" fill=\"#FF0000\"/>"
            "</svg>");
    }

    // Recorrer cada pixel RGBA_8888, calcular luma (rec.601), aplicar threshold
    uint8_t* px = static_cast<uint8_t*>(pixels);
    int black_pixels = 0;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // AndroidBitmap format RGBA_8888: 4 bytes por pixel
            int offset = (y * info.stride) + (x * 4);
            uint8_t r = px[offset];
            uint8_t g = px[offset + 1];
            uint8_t b = px[offset + 2];
            // Ignorar alpha offset+3 para binarización

            // Luma Rec.601: Y = 0.299*R + 0.587*G + 0.114*B
            int y_luma = (int)(0.299 * r + 0.587 * g + 0.114 * b);

            if (y_luma < threshold) {
                BM_SET(bm, x, y);
                black_pixels++;
            }
            // else queda en 0 (blanco) por calloc
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    LOGI("Binarized: %d black pixels out of %d (%.1f%%)",
         black_pixels, w * h, 100.0 * black_pixels / (w * h));

    // ---------- Paso 4: Trazar con Potrace ----------
    potrace_param_t* param = potrace_param_new();
    if (!param) {
        potrace_bitmap_free(bm);
        LOGE("potrace_param_new failed");
        return env->NewStringUTF(
            "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"200\" height=\"200\">"
            "<rect width=\"200\" height=\"200\" fill=\"#FF0000\"/>"
            "</svg>");
    }
    param->turdsize = 2;
    param->alphamax = 1;
    param->opticurve = 1;

    potrace_state_t* st = potrace_trace(bm, param);
    if (!st || !st->data || st->len <= 0) {
        potrace_param_free(param);
        potrace_bitmap_free(bm);
        LOGE("potrace_trace produced no output");
        return env->NewStringUTF(
            "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"200\" height=\"200\">"
            "<rect width=\"200\" height=\"200\" fill=\"#FFFF00\"/>"
            "</svg>");
    }

    LOGI("Trace completed: SVG=%d bytes", st->len);

    // ---------- Paso 5: Emitir SVG como jstring ----------
    jstring result = env->NewStringUTF((const char*)st->data);

    // Limpieza
    potrace_state_free(st);
    potrace_param_free(param);
    potrace_bitmap_free(bm);

    return result;
}
