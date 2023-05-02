#ifndef __WL_LIB_H__
#define __WL_LIB_H__

#include <stdint.h>

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__NT__)
#include <tchar.h>
#include <Windows.h>
#include <fileapi.h>
typedef HANDLE File;
typedef LPCSTR name_file;

#ifdef _WIN64
typedef ULONGLONG Size_file;
#else
typedef DWORD Size_file;
#endif

#ifndef GENERIC_READ
#define GENERIC_READ         0x80000000
#endif
#ifndef GENERIC_WRITE
#define GENERIC_WRITE        0x40000000
#endif
#ifndef FILE_APPEND_DATA
#define FILE_APPEND_DATA     0x00000004
#endif
#ifndef TRUNCATE_EXISTING
#define TRUNCATE_EXISTING    0x00000080
#endif
#ifndef CREATE_ALWAYS
#define CREATE_ALWAYS        0x00000002
#endif
#ifndef CREATE_NEW
#define CREATE_NEW           0x00000001
#endif

typedef enum mode {
    READ       = GENERIC_READ,
    WRITE      = GENERIC_WRITE,
    READ_WRITE = GENERIC_READ | GENERIC_WRITE,
    APPEND     = FILE_APPEND_DATA,
    TRUNCATE   = TRUNCATE_EXISTING,
    CREATE     = CREATE_ALWAYS,
    EXCL       = CREATE_NEW
} mode;

#define OPEN_ERROR INVALID_HANDLE_VALUE
#define READ_ERROR 0
#define WRITE_ERROR 0
#elif __linux__
#include <fcntl.h>
#include <sys/stat.h>

typedef FILE* File;
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
typedef uint64_t Size_file;
#else
typedef uint32_t Size_file;
#endif

typedef enum mode {
    READ       = O_RDONLY,
    WRITE      = O_WRONLY,
    READ_WRITE = O_RDWR,
    APPEND     = O_APPEND,
    TRUNCATE   = O_TRUNC,
    CREATE     = O_CREAT,
    EXCL       = O_EXCL
} mode;

#define OPEN_ERROR NULL

#endif

typedef struct MyFile {
    File archivo;        // archivo abierto
    Size_file size;      // tamaño del archivo
    char *data;          // datos  del archivo leeidos o a escribir/escritos
    Size_file last_char; // ultimo byte leeido o escrito con exito
} MyFile;

void open_file(MyFile*, name_file, mode);
void read_file(MyFile*);
void write_file(MyFile *my_file, const char *data);
File open_f(name_file, mode);
Size_file get_size_file(File);

#include "WL_lib.c"
#else
    #error "No se esta compilando para la version c99 o superior."
#endif
