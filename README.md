# 🎯 GraphQuest

GraphQuest es un juego de exploración por escenarios conectados donde el jugador debe recolectar objetos valiosos, administrar su inventario y llegar al escenario final antes de que se agote el tiempo.

## 📜 Historia del juego

¡Bienvenido a **GraphQuest**! Eres un aventurero que debe explorar una red de escenarios interconectados, recolectando objetos de valor y administrando cuidadosamente el peso de tu inventario y el tiempo limitado que tienes. Tu misión es alcanzar el **escenario final** antes de que el tiempo se acabe.

---

## ▶️ Cómo jugar

Al iniciar el juego, los escenarios se cargan desde el archivo `graphquest.csv` y el jugador comienza en el escenario de entrada.

### 🎮 Menú de juego

Durante cada turno, el jugador puede elegir entre las siguientes acciones:

1. **Recoger items**: 
   - Muestra los objetos disponibles en el escenario actual.
2. **Descartar items**: 
   - Muestra los objetos en el inventario.
   - El jugador puede eliminar un objeto para liberar peso.

3. **Moverse a otro escenario**:
   - Cuando eliges la opción "Moverse a otro escenario", se muestran las direcciones disponibles desde el escenario actual, junto con el ID del escenario conectado en cada dirección:
     ```bash
     Movimientos disponibles desde el escenario 'Sala de entrada':
      1. Arriba: -1
      2. Abajo: 3
      3. Izquierda: -1
      4. Derecha: 5
     ```
   - Si el número es positivo (por ejemplo `3` o `5`), significa que puedes moverte en esa dirección hacia el escenario con ese ID.
   - Si el número es `-1`, eso indica que hay una pared o no existe un escenario en esa dirección. Es decir, el movimiento está bloqueado.
   - El movimiento consume tiempo en función del peso:  
     \[`tiempo_usado = (peso_total + 1) / 10`\]

4. **Reiniciar Partida**:
   - Reinicia el estado del jugador, inventario vacío, puntaje en cero y vuelve al escenario de entrada.

5. **Salir del juego**:
   - Termina el programa.

---

## ⏳ Reglas del juego

- **Tiempo límite**: Comienzas con 10 unidades de tiempo.
  - Cada movimiento o acción consume tiempo.
  - Si el tiempo llega a cero, pierdes la partida.

- **Inventario**:
  - Entre mas objetos tengas en inventario se te descontara más tiempo.

- **Objetos (Items)**:
  - Cada item tiene un nombre, valor y peso.
  - Los valores se suman al **puntaje total**.

- **Escenarios**:
  - Tienen nombre, descripción y conexiones (arriba, abajo, izquierda, derecha).
  - Algunos contienen objetos.
  - Uno es el **escenario final**. Al llegar allí, ganas.

---

## 🗂️ Archivos importantes

- `tarea3.c`: Código principal del juego.
- `tdas/`: Carpeta con archivos de estructuras de datos (`list.c`, `list.h`, `map.c`, `map.h`, `extra.c`, `extra.h`).
- `graphquest.csv`: Archivo con la definición de los escenarios y los objetos disponibles.

---

## 🚀 Compilación y ejecución

### 💻 Desde la terminal (Windows/Linux/macOS/WSL/MinGW)

```bash
gcc tarea3.c tdas/list.c tdas/extra.c tdas/map.c -o tarea3
./tarea3
