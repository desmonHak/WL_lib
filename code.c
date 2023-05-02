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

    MyFile mi_archivo_cpy;
    open_file(&mi_archivo_cpy, "code1.c", READ_WRITE );
    if (mi_archivo_cpy.archivo == OPEN_ERROR) {
        printf("Error al abrir el archivo\n");
        return 1;
    }
    mi_archivo_cpy.size = mi_archivo.size;
    printf("size del archivo: %d\n", mi_archivo_cpy.size);

    write_file(&mi_archivo_cpy, mi_archivo.data);
    if(mi_archivo_cpy.size == WRITE_ERROR){
        printf("Error al escribir el archivo\n");
    }
    printf("size del archivo: %d\n", mi_archivo_cpy.size);

    puts("Archivo abierto");
    close_file(&mi_archivo);
    close_file(&mi_archivo_cpy);
    return 0;
}
