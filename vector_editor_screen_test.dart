import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:ultra_vectorizer/features/editor/screens/vector_editor_screen.dart';

void main() {
  group('VectorEditorScreen', () {
    testWidgets('renders initial state with start button', (tester) async {
      await tester.pumpWidget(
        MaterialApp(
          home: VectorEditorScreen(imagePath: '/test/image.png'),
        ),
      );

      // Debe mostrar botón de inicio en estado inicial
      expect(find.text('INICIAR VECTORIZACIÓN'), findsOneWidget);
      expect(find.byType(ElevatedButton), findsOneWidget);
    });

    testWidgets('shows app bar title', (tester) async {
      await tester.pumpWidget(
        MaterialApp(
          home: VectorEditorScreen(imagePath: '/test/image.png'),
        ),
      );

      expect(find.text('Anubis Vector Forge'), findsOneWidget);
    });

    testWidgets('does not show loading initially', (tester) async {
      await tester.pumpWidget(
        MaterialApp(
          home: VectorEditorScreen(imagePath: '/test/image.png'),
        ),
      );

      expect(find.byType(CircularProgressIndicator), findsNothing);
    });
  });
}
