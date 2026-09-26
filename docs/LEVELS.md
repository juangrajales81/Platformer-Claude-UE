# Formato de los niveles

Los niveles son archivos de texto en `Content/Levels/LevelN.txt` (N = 1, 2, ...).
Se cargan en orden; al terminar el último, la partida está ganada.

- Cada carácter es una celda de 1×1 m. La **última** fila del archivo está a Z = 0.
- Las líneas que empiezan por `;` son comentarios.
- Las filas pueden tener distinta longitud; lo que falta se considera vacío.
- Guarda el archivo en UTF-8.

## Leyenda

| Carácter | Elemento |
|----------|----------|
| `.` o espacio | Vacío |
| `#` | Suelo (hierba en la superficie, tierra debajo) |
| `=` | Bloque de piedra (plataforma sólida) |
| `P` | Punto de inicio del jugador |
| `G` | Meta del nivel (se hace visible y funcional en la fase 7) |
