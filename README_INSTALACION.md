# ANUBIS VECTOR FORGE — Guía de Instalación Completa

App Flutter + plugin nativo Kotlin/C++ (Potrace) que vectoriza imágenes en el dispositivo.

## 📋 QUÉ INCLUYE ESTE ZIP
- Host app Flutter completo (pantalla principal + selector de imagen)
- Plugin Ultra Vectorizer (Kotlin + C++ Potrace real)
- GitHub Actions workflow listo para compilar APK en la nube
- Tests, ProGuard, licencia GPL documentada

## 🚀 OPCIÓN 1: COMPILAR EN LA NUBE (solo necesitas el teléfono)

### Requisitos
- Cuenta GitHub gratuita
- App Termux (desde F-Droid, NO Play Store)

### Pasos
1. Instala Termux desde F-Droid y abrelo.
2. En Termux pega línea por línea:
   ```
   pkg update && pkg upgrade -y
   pkg install -y git gh wget unzip
   ```
3. Autentícate en GitHub:
   ```
   gh auth login
   ```
   Elige: GitHub.com → HTTPS → yes (pastes token). Crea token en github.com → Settings → Developer settings → Personal access tokens → repo. Pega aquí.
4. Sube el proyecto:
   ```
   cd ~
   mkdir uv && cd uv
   # Transfiere este ZIP al teléfono y muévelo a la carpeta de Termux
   # (usa un gestor de archivos con acceso a la carpeta de Termux,
   #  normalmente /storage/emulated/0/Download → /data/data/com.termux/files/home/uv)
   unzip ultra_vectorizer_host.zip
   rm ultra_vectorizer_host.zip
   git init
   git add .
   git commit -m "Anubis Vector Forge inicial"
   gh repo create anubis-vector-forge --public --push
   ```
5. Ve a github.com → tu repo → pestaña **Actions**. El workflow `.github/workflows/build_apk.yml` se dispara solo con el push. Espera ~6-8 min.
6. Cuando termine: Actions → último run → sección **Artifacts** → descarga `anubis-vector-forge-apk`.
7. En el teléfono: Ajustes → Seguridad → Orígenes desconocidos → activa. Abre el APK descargado → Instalar.

## 💻 OPCIÓN 2: COMPILAR EN PC (más rápido)

### Requisitos
- Flutter SDK 3.22+ (flutter.dev)
- Android Studio + SDK 34 + NDK 25.2.9519653
- Git

### Pasos
1. Descomprime el ZIP en tu PC.
2. Edita `android/local.properties`:
   ```
   sdk.dir=C:\\Users\\tu\\AppData\\Local\\Android\\Sdk   (Windows)
   # o
   sdk.dir=/home/tu/Android/Sdk                          (Linux/Mac)
   flutter.sdk=/ruta/a/flutter
   ```
3. En terminal:
   ```
   flutter pub get
   flutter build apk --release
   ```
4. APK en `build/app/outputs/flutter-apk/app-release.apk`. Conéctalo por USB (depurración USB on) y:
   ```
   flutter install
   ```
   O copia el APK al teléfono y ábrelo.

## ⚠️ NOTAS
- La app pide permiso de galería para leer la imagen.
- Licencia GPLv2+ aplica al motor Potrace (ver LICENSE.txt). Uso personal OK.
- Arquitecturas: arm64-v8a + armeabi-v7a (99% de teléfonos).

## 🔧 SOLUCIÓN DE PROBLEMAS
- **Error NDK**: asegúrate de tener NDK 25.2.9519653 en Android SDK Manager.
- **Error de plugin no encontrado**: verifica que `ultra_vectorizer/` esté al mismo nivel que `pubspec.yaml` del host.
- **APK no instala**: activa "Orígenes desconocidos" en Ajustes → Seguridad.
