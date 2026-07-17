import 'package:flutter/material.dart';
import 'package:image_picker/image_picker.dart';
import 'package:ultra_vectorizer/features/editor/screens/vector_editor_screen.dart';

void main() => runApp(const AnubisApp());

class AnubisApp extends StatelessWidget {
  const AnubisApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Anubis Vector Forge',
      theme: ThemeData.dark().copyWith(
        primaryColor: const Color(0xFF00C4A0),
        colorScheme: ColorScheme.dark(
          primary: const Color(0xFF00C4A0),
          secondary: const Color(0xFF00C4A0),
        ),
        scaffoldBackgroundColor: Colors.black,
        appBarTheme: const AppBarTheme(
          backgroundColor: Colors.black87,
          foregroundColor: Color(0xFF00C4A0),
        ),
      ),
      home: const HomeScreen(),
    );
  }
}

class HomeScreen extends StatelessWidget {
  const HomeScreen({super.key});

  Future<void> _pickImage(BuildContext context) async {
    final picker = ImagePicker();
    final XFile? file = await picker.pickImage(source: ImageSource.gallery);
    if (file != null && context.mounted) {
      Navigator.push(
        context,
        MaterialPageRoute(
          builder: (_) => VectorEditorScreen(imagePath: file.path),
        ),
      );
    } else if (context.mounted) {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('No se seleccionó imagen')),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Anubis Vector Forge')),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            const Icon(Icons.auto_awesome,
                size: 100, color: Color(0xFF00C4A0)),
            const SizedBox(height: 24),
            const Text(
              'Forja de Vectores de Anubis',
              style: TextStyle(
                  color: Colors.white,
                  fontSize: 22,
                  fontWeight: FontWeight.bold),
            ),
            const SizedBox(height: 12),
            const Text(
              'Selecciona una imagen para trazarla',
              style: TextStyle(color: Colors.grey),
            ),
            const SizedBox(height: 32),
            ElevatedButton.icon(
              onPressed: () => _pickImage(context),
              icon: const Icon(Icons.image),
              label: const Text('SELECCIONAR IMAGEN'),
              style: ElevatedButton.styleFrom(
                backgroundColor: const Color(0xFF00C4A0),
                foregroundColor: Colors.black,
                padding: const EdgeInsets.symmetric(
                    horizontal: 32, vertical: 16),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
