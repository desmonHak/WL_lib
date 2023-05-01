#include <stdio.h>
#include "WL_lib.h"

int main(){

    MyFile mi_archivo;
    open_file(&mi_archivo, "code.c", READ | WRITE | APPEND);

    if (mi_archivo.archivo == OPEN_ERROR) {
        printf("Error al abrir el archivo\n");
        return 1;
    }
    printf("size del archivo: %d\n", mi_archivo.size);

    // Leer el archivo
    read_file(&mi_archivo);
    if (mi_archivo.data == READ_ERROR){
        printf("Error al leer el archivo\n");
    }else {
        mi_archivo.data [mi_archivo.size] = 0x0;
        printf("Datos leidos: %s\n", mi_archivo.data );
    }

    puts("Archivo abierto");
    close_file(&mi_archivo);
    return 0;
}