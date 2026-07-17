import 'package:flutter_test/flutter_test.dart';
import 'package:ultra_vectorizer/core/vectorization/vector_result.dart';

void main() {
  group('VectorResult.fromMap', () {
    test('parses successful response correctly', () {
      final map = <dynamic, dynamic>{
        'svg': '<svg><!-- nodes=42 --></svg>',
        'nodes': 42,
        'time': 120,
      };

      final result = VectorResult.fromMap(map);

      expect(result.success, isTrue);
      expect(result.nodes, 42);
      expect(result.timeMs, 120);
      expect(result.svg, contains('nodes=42'));
    });

    test('handles missing keys gracefully', () {
      final map = <dynamic, dynamic>{};

      final result = VectorResult.fromMap(map);

      expect(result.success, isTrue);
      expect(result.svg, '');
      expect(result.nodes, 0);
      expect(result.timeMs, 0);
    });

    test('handles null map values', () {
      final map = <dynamic, dynamic>{
        'svg': null,
        'nodes': null,
        'time': null,
      };

      final result = VectorResult.fromMap(map);

      expect(result.svg, '');
      expect(result.nodes, 0);
      expect(result.timeMs, 0);
    });
  });

  group('VectorResult.error', () {
    test('creates error result', () {
      final result = VectorResult.error('Something went wrong');

      expect(result.success, isFalse);
      expect(result.error, 'Something went wrong');
      expect(result.svg, '');
    });
  });

  group('VectorResult.nodesFromSvg', () {
    test('extracts node count from SVG comment', () {
      final result = VectorResult(
        svg: '<svg><!-- nodes=88 --></svg>',
        nodes: 0,
        timeMs: 100,
        success: true,
      );

      expect(result.nodesFromSvg, 88);
    });

    test('returns field nodes when no SVG comment present', () {
      final result = VectorResult(
        svg: '<svg></svg>',
        nodes: 15,
        timeMs: 50,
        success: true,
      );

      expect(result.nodesFromSvg, 15);
    });

    test('returns 0 when SVG is empty and success is false', () {
      final result = VectorResult.error('fail');

      expect(result.nodesFromSvg, 0);
    });
  });
}
