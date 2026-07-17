import 'package:flutter/services.dart';
import 'vector_result.dart';

/// Servicio que se comunica con el motor nativo Kotlin/C++
class UltraVectorizerService {
  static const MethodChannel _channel =
      MethodChannel('com.ultra.vectorizer/engine');

  /// Vectoriza una imagen usando el motor nativo
  /// [imagePath]: ruta absoluta al archivo de imagen en el dispositivo
  /// [precision]: 0.0–1.0, controla el umbral de binarización
  Future<VectorResult> vectorize({
    required String imagePath,
    double precision = 1.0,
  }) async {
    try {
      final response = await _channel.invokeMethod<Map<dynamic, dynamic>>(
        'vectorizeImage',
        <String, dynamic>{
          'path': imagePath,
          'precision': precision,
        },
      );

      if (response == null) {
        return VectorResult.error('No response from native engine');
      }

      return VectorResult.fromMap(response);
    } on PlatformException catch (e) {
      return VectorResult.error('Platform Error: ${e.code} — ${e.message}');
    } catch (e) {
      return VectorResult.error('Unexpected error: $e');
    }
  }

  /// Valida que el MethodChannel esté conectado al plugin nativo
  Future<bool> isAvailable() async {
    try {
      final result = await _channel.invokeMethod<bool>('isAvailable');
      return result ?? false;
    } catch (_) {
      return false;
    }
  }
}
