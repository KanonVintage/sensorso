## Correr `./makeit` para compilar y correr automaticamente

`startup.x` es el archivo de configuracion

Tiene la siguiente estructura:
```shell
    nombre:keyd,keyt;keyI,Iinicial;keyQ,Qinicial;keyX
```

keyd: key de la memoria compartida para la distancia
keyt: key de la memoria compartida para el giroscopio
keyI: key de la memoria compartida para el intervalo
keyQ: key de la memoria compartida para el muestreo
keyX: key de la memoria compartida para el resultado final

makeit es un shell script que realiza lo siguiente con `sensorSO.c`, `remote.c` y `lector.c`:

sensorSO.c:
```shell
gcc -g sensorSO.c -o run/sensorSO -lm
```

lector.c:
```shell
gcc -g lector.c -o run/lector -lc
```

remote.c:
```shell
gcc -g remote.c -o run/remote -lc
```

El trabajo consiste en subir a youtube un video de máximo 10 mins en ingles y minimo 7 minutos, que explique el problema (max 1/5 del tiempo de duracion del video), propuesta de diseño y resultados del proyecto finalizado del primer parcial. Debe de tener ayudas visuales para explicar su diseño y su justificación y mostrar resultados cuantitativos usando gráficos o tablas