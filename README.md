## Compilacion

Seguir las siguientes instrucciones para compilar el `sensorSO.c` y `lector.c`:

sensorSO.c:
```shell
gcc -g sensorSO.c -o compiled/sensorSO -lm
```

lector.c:
```shell
gcc -g lector.c -o compiled/lector -lc
```

concurrent.c:
```shell
gcc -g concurrent.c -o compiled/concurrent -Wall -lpthread
```

El trabajo consiste en subir a youtube un video de máximo 10 mins en ingles y minimo 7 minutos, que explique el problema (max 1/5 del tiempo de duracion del video), propuesta de diseño y resultados del proyecto finalizado del primer parcial. Debe de tener ayudas visuales para explicar su diseño y su justificación y mostrar resultados cuantitativos usando gráficos o tablas