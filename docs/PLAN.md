# Plan de desarrollo — Platformer (estilo Giana Sisters)

Juego de plataformas 2.5D con desplazamiento lateral hecho con Unreal Engine 5.8.
Todo el juego está escrito en C++ y **no depende de assets binarios**: los niveles
se describen en archivos de texto (`Content/Levels/*.txt`) y la geometría usa las
formas básicas del motor (`/Engine/BasicShapes`). Así cada fase es fácil de revisar en Git.

| Fase | Contenido | Estado |
|------|-----------|--------|
| 1 | Estructura del proyecto (`.uproject`, módulo C++, targets, config, scripts) | ✅ |
| 2 | Personaje jugable: movimiento lateral, salto variable, cámara 2.5D, controles, luz y cielo | ✅ |
| 3 | Constructor de niveles a partir de mapas ASCII (nivel 1: terreno) | ✅ |
| 4 | Bloques interactivos (ladrillos, bloques bonus) y diamantes | ⬜ |
| 5 | Enemigos: patrulla, pisotón y daño al jugador | ⬜ |
| 6 | Power-ups: "punk" (rompe ladrillos) y "fuego" (dispara burbujas) | ⬜ |
| 7 | Reglas de juego y HUD: vidas, puntos, tiempo, muerte, meta, game over | ⬜ |
| 8 | Pulido: varios niveles, fondo con parallax, pantalla de título | ⬜ |
