# Formato de los niveles

Los niveles son archivos de texto en `Content/Levels/LevelN.txt` (N = 1, 2, ...).
Se cargan en orden; al terminar el último, la partida está ganada.

- Cada carácter es una celda de 1×1 m. La **última** fila del archivo está a Z = 0.
- Las líneas que empiezan por `;` son comentarios. El **primer** comentario es el título
  del nivel, que se muestra al empezarlo.
- Las filas pueden tener distinta longitud; lo que falta se considera vacío.
- Guarda el archivo en UTF-8.

## Leyenda

| Carácter | Elemento |
|----------|----------|
| `.` o espacio | Vacío |
| `#` | Suelo (hierba en la superficie, tierra debajo) |
| `=` | Bloque de piedra (plataforma sólida) |
| `B` | Ladrillo: rebota al golpearlo; se rompe con el poder "punk" |
| `?` | Bloque bonus con un diamante |
| `*` | Bloque bonus con una bola de sueño (power-up) |
| `D` | Diamante (100 puntos; 100 diamantes = vida extra) |
| `E` | Enemigo (búho que patrulla y se gira en paredes y bordes) |
| `^` | Pinchos (dañan al tocarlos) |
| `P` | Punto de inicio del jugador |
| `G` | Meta del nivel (mástil con bandera) |
