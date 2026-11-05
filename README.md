# Calculadora (VST3)

Desarrollador: **PAPIELGUAPO**.

Plugin VST3 de 64 bits para Windows, con una interfaz de 250 × 321 píxeles inspirada en la referencia.

![Imagen guía](docs/guia.png)

## Descargar

Descarga `Calculadora-VST3-Windows-x64.zip` desde la sección [Releases](../../releases/latest) y extrae `Calculadora.vst3`.

## Funciones

- Operaciones encadenadas: suma, resta, multiplicación y división.
- Porcentaje, cambio de signo, coma decimal y repetición de «=».
- Entrada con clics o teclado: números, `+`, `-`, `*`, `/`, `,`/`.`, `%`, `Enter`, `Esc` y retroceso.
- Pasa el audio del anfitrión sin modificarlo.

## Instalar

Copia la carpeta completa `Calculadora.vst3` en una ruta VST3, por ejemplo:

`C:\Program Files\Common Files\VST3`

Después, vuelve a escanear los plugins en tu DAW. El binario incluido es x64.

## Compilar desde el código fuente

Requiere JUCE, CMake 3.22 o posterior y Visual Studio 2022 o posterior con el compilador C++ para x64.

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_PATH="C:/JUCE"
cmake --build build --config Release --target Calculadora1a1_VST3
```

Si JUCE está en otra ubicación, cambia `JUCE_PATH` al directorio raíz de JUCE.
