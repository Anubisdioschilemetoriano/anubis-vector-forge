/// Entidad inmutable del resultado de vectorización
class VectorResult {
  final String svg;
  final int nodes;
  final int timeMs;
  final bool success;
  final String? error;

  const VectorResult({
    required this.svg,
    required this.nodes,
    required this.timeMs,
    required this.success,
    this.error,
  });

  /// Construye desde mapa devuelto por MethodChannel nativo
  factory VectorResult.fromMap(Map<dynamic, dynamic> map) {
    return VectorResult(
      svg: map['svg'] as String? ?? '',
      nodes: (map['nodes'] as num?)?.toInt() ?? 0,
      timeMs: (map['time'] as num?)?.toInt() ?? 0,
      success: true,
    );
  }

  /// Construye resultado de error
  factory VectorResult.error(String message) {
    return VectorResult(
      svg: '',
      nodes: 0,
      timeMs: 0,
      success: false,
      error: message,
    );
  }

  /// Extrae el conteo de nodos desde el comentario <!-- nodes=N --> en el SVG
  int get nodesFromSvg {
    if (!success || svg.isEmpty) return 0;
    final regex = RegExp(r'<!-- nodes=(\d+) -->');
    final match = regex.firstMatch(svg);
    if (match != null) {
      return int.tryParse(match.group(1)!) ?? nodes;
    }
    return nodes;
  }
}
