import 'package:flutter/material.dart';
import '../../../core/vectorization/ultra_vectorizer_service.dart';
import '../../../core/vectorization/vector_result.dart';

class VectorEditorScreen extends StatefulWidget {
  final String imagePath;
  const VectorEditorScreen({super.key, required this.imagePath});

  @override
  State<VectorEditorScreen> createState() => _VectorEditorScreenState();
}

class _VectorEditorScreenState extends State<VectorEditorScreen> {
  final _vectorizer = UltraVectorizerService();
  VectorResult? _result;
  bool _isLoading = false;
  String? _svgPreview;

  Future<void> _startVectorization() async {
    setState(() => _isLoading = true);

    final result = await _vectorizer.vectorize(
      imagePath: widget.imagePath,
      precision: 0.95,
    );

    setState(() {
      _result = result;
      _isLoading = false;
      if (result.success && result.svg.isNotEmpty) {
        _svgPreview = result.svg;
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Anubis Vector Forge'),
        backgroundColor: Colors.black87,
        foregroundColor: const Color(0xFF00C4A0),
      ),
      backgroundColor: Colors.black,
      body: Center(
        child: _isLoading
            ? const Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  CircularProgressIndicator(color: Color(0xFF00C4A0)),
                  SizedBox(height: 20),
                  Text(
                    'Trazando vectores...',
                    style: TextStyle(color: Colors.grey),
                  ),
                ],
              )
            : _result == null
                ? Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      Icon(Icons.image_search,
                          size: 80, color: Colors.grey.shade700),
                      const SizedBox(height: 20),
                      ElevatedButton.icon(
                        onPressed: _startVectorization,
                        icon: const Icon(Icons.auto_fix_high),
                        label: const Text('INICIAR VECTORIZACIÓN'),
                        style: ElevatedButton.styleFrom(
                          backgroundColor: const Color(0xFF00C4A0),
                          foregroundColor: Colors.black,
                          padding: const EdgeInsets.symmetric(
                              horizontal: 32, vertical: 16),
                        ),
                      ),
                    ],
                  )
                : _result!.success
                    ? Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          const Icon(Icons.check_circle,
                              size: 60, color: Color(0xFF00C4A0)),
                          const SizedBox(height: 16),
                          Text(
                            '${_result!.nodesFromSvg} nodos generados',
                            style: const TextStyle(
                                color: Colors.white, fontSize: 18),
                          ),
                          Text(
                            '${_result!.timeMs} ms',
                            style: const TextStyle(color: Colors.grey),
                          ),
                          const SizedBox(height: 20),
                          SizedBox(
                            height: 200,
                            child: _svgPreview != null
                                ? Container(
                                    padding: const EdgeInsets.all(12),
                                    decoration: BoxDecoration(
                                      border: Border.all(
                                          color: const Color(0xFF00C4A0)),
                                      borderRadius: BorderRadius.circular(8),
                                    ),
                                    child: Column(
                                      children: [
                                        const Text('SVG generado correctamente',
                                            style: TextStyle(
                                                color: Color(0xFF00C4A0))),
                                        const SizedBox(height: 8),
                                        Expanded(
                                          child: SingleChildScrollView(
                                            child: Text(
                                              _svgPreview!,
                                              style: const TextStyle(
                                                  color: Colors.white70,
                                                  fontSize: 8,
                                                  fontFamily: 'monospace'),
                                            ),
                                          ),
                                        ),
                                      ],
                                    ),
                                  )
                                : const SizedBox.shrink(),
                          ),
                        ],
                      )
                    : Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          const Icon(Icons.error,
                              size: 60, color: Colors.redAccent),
                          const SizedBox(height: 16),
                          Text(
                            'Error: ${_result!.error ?? "Desconocido"}',
                            style: const TextStyle(color: Colors.redAccent),
                            textAlign: TextAlign.center,
                          ),
                        ],
                      ),
      ),
    );
  }
}
