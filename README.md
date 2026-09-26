# Platformer — plataformas 2.5D estilo Giana Sisters (Unreal Engine 5.8)

Pequeño juego de plataformas de desplazamiento lateral, escrito íntegramente en C++.
Consulta el plan por fases en [docs/PLAN.md](docs/PLAN.md).

## Requisitos

- Unreal Engine 5.8 (instalado en `D:\EPIC\UE_5.8`)
- Visual Studio 2022/2026 con la carga de trabajo **"Desarrollo de juegos con C++"**,
  que incluye el compilador MSVC v14.x y un **Windows 10/11 SDK**.

## Compilar y ejecutar

```bat
Scripts\Build.bat      :: compila el módulo del editor
Scripts\RunEditor.bat  :: abre el proyecto en el editor (pulsa Play)
Scripts\RunGame.bat    :: lanza el juego en una ventana, sin editor
```

También puedes hacer clic derecho sobre `Platformer.uproject` → *Generate Visual Studio
project files* y compilar desde Visual Studio.
