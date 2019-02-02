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

## FUNCIONAMIENTO
Existen 4 programas principales y un archivo de configuracion
El programa sensorSO crea un sensor con un giroscopio y un calculo de distancia
El programa lector calcula la distancia real en base al giroscopio y a la distancia
    Ambos usan memoria compartida para comunicarse como tambien banderas para evitar datos aberrantes
El programa remote crea un lector por sensor. De igual manera usa memoria compartida para la comunicacion entre estos dos. Con el archivo startup.x crea sensores y lectores dinamicamente pero minimo deben de haber 3 como fue especificado inicialmente. El archivo de configuracion sigue la estructura previamente mencionada arriba, de esa forma se pueden crear nuevos sensores.
El programa remote tambien crea un proceso hijo llamado visor, el cual visualiza los resultados de todos los lectores y calcula la desviacion estandar, y en base a esto indentifica si el carro proximo es un obstaculo o no.
    El programa principal remote se comunica con memoria compartida con el programa visor, pasando como parametros los keys necesarios al momento de crearlo.
    El programa principal remote tambien cuenta con un menu para matar algun sensor, pero esto no mata a su lector, tambien puede reiniciar los sensores necesarios. Y finalmente puede cambiar las variables de I, Q, T y W. 
        I y Q son variables especificas por cada sensor
        T y W son variables usadas solamente por el proceso visor

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