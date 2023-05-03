# WL_lib 

-----
WL_lib (Windows & Linux Library) Es una libreria que permite un cierto alto nivel de funciones, para no tener que lidiar con las cabeceras unicad de cada sistema.

Actualmente hemos creado una funcion `open_file` que nos permite abrir archivos independientemente del si el sistema es Linux o Windows, para la tarea, intenamente se usa las funciones de `fileapi.h` con su funcion `CreateFile` para abrir archivos en sistemas Windows, o en caso de ser un sistema linux donde se esta compilando, usar la clasica `fopen`. 

Se espera en un futuro hacer lo mismo con socket's, lo que deberia permitir la facilidad de escribir software multiplataforma que haga uso de estos
----