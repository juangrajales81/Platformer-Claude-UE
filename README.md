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

## Controles

| Acción | Teclado | Mando |
|--------|---------|-------|
| Moverse | A / D o ← / → | Stick izquierdo / cruceta |
| Saltar (mantener = más alto) | Espacio, W o ↑ | A (botón inferior) |
| Disparar (con poder de fuego) | F, J o Ctrl izq. | X (botón izquierdo) |

## Reglas

- Empiezas con 3 vidas y 300 segundos por nivel.
- Cada diamante vale 100 puntos; con 100 diamantes ganas una vida.
- Pisa a los búhos para eliminarlos. Si te tocan pierdes el poder o, sin poder, una vida.
- Caer a un foso o quedarte sin tiempo también cuesta una vida.
- Al llegar a la bandera, el tiempo sobrante se suma a los puntos (10 por segundo).
