#ifndef __WL_LIB_C__
#define __WL_LIB_C__

Size_file get_size_file(MyFile my_file)
{
    return my_file.size;
}
bool error_open_file(MyFile my_file)
{
    if (my_file.archivo == OPEN_ERROR)
        return true;
    else
        return false;
}
bool error_read_file(MyFile my_file)
{
    if (my_file.data == READ_ERROR)
        return true;
    else
        return false;
}
bool error_mode_file(MyFile my_file)
{
    if (my_file.archivo == OPEN_MODE_ERROR)
        return true;
    else
        return false;
}
char *get_data_file(MyFile my_file)
{
    return my_file.data;
}

File open_f(name_file name_file_open, mode mode_open)
{
    File my_file;
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__NT__)

    DWORD dwShareMode, dwDesiredAccess, dwCreationDisposition;
    switch (mode_open & (READ | WRITE))
    {
    case READ:
        // permitir acceso de lectura a otros procesos
        dwShareMode = FILE_SHARE_READ;
        dwDesiredAccess = READ;
        dwCreationDisposition = OPEN_EXISTING;
        break;
    case WRITE:
        // permitir acceso de escritura a otros procesos
        dwShareMode = FILE_SHARE_WRITE;
        dwDesiredAccess = WRITE;
        dwCreationDisposition = OPEN_EXISTING;
        break;
    case READ | WRITE:
        // permitir acceso de escritura y lectura a otros procesos
        dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
        dwDesiredAccess = READ | WRITE;
        dwCreationDisposition = OPEN_ALWAYS;
    default:
        // permite acceso de lectura, escritura y eliminacion para otros procesos
        dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
        dwDesiredAccess = mode_open;
        dwCreationDisposition = OPEN_ALWAYS;
        break;
    }

    my_file = CreateFile(
        name_file_open,
        dwDesiredAccess,
        dwShareMode,
        NULL,                  // atributos de seguridad
        dwCreationDisposition, // abrir el archivo existente
        FILE_ATTRIBUTE_NORMAL, // atributos del archivo
        NULL);

#elif __linux__
    char mode_linux[4] = {0, 0, 0, 0};
    switch (mode_open & (READ | WRITE | APPEND | TRUNCATE | CREATE | EXCL))
    {
    case READ:
        mode_linux[0] = 'r'; // Abrir archivo para lectura. El archivo debe existir.
        break;
    case WRITE:
    case WRITE | TRUNCATE:
        mode_linux[0] = 'w'; // Abrir archivo para escritura. Si el archivo no existe, se crea. Si el archivo existe, se trunca.
        break;
    case CREATE | WRITE | APPEND:
    case APPEND:
        mode_linux[0] = 'a'; // Abrir archivo para escritura. Si el archivo no existe, se crea. Si el archivo existe, la escritura se realiza al final del archivo.
        break;
    case READ | WRITE: // el archivo debe existir, permite leer y escribir
        mode_linux[0] = 'r';
        mode_linux[1] = '+';
        break;
    case CREATE | WRITE: // crea el archivo si no existe, permite leer y escribir
        mode_linux[0] = 'w';
        mode_linux[1] = '+';
        break;
    case CREATE | APPEND: // Abrir archivo para lectura y escritura. Si el archivo no existe, se crea. Si el archivo existe, la escritura se realiza al final del archivo.
        mode_linux[0] = 'a';
        mode_linux[1] = '+';
        break;
    case TRUNCATE: // truncar el archivo si existe
        mode_linux[0] = 'w';
        mode_linux[1] = 'c';
        break;
    case EXCL | WRITE: // permite escribir, pero si el archivo existe retorna error.
        mode_linux[0] = 'w';
        mode_linux[1] = 'x';
        break;
    case READ | EXCL | WRITE: // permite escribir, pero si el archivo existe retorna error.
        mode_linux[0] = 'x';
        mode_linux[1] = '+';
        break;
    case EXCL | CREATE | WRITE: // abre un archivo para escritura, creándolo sólo si no existe; retorna error si ya existe
        mode_linux[0] = 'w';
        mode_linux[1] = 'x';
        mode_linux[1] = '+';
    default:
        // el modo de apertura no es adecuado
        return OPEN_MODE_ERROR;
    }
    my_file = fopen(
        name_file_open,
        mode_linux);
    if (my_file == NULL)
    {
        return OPEN_ERROR;
    }
#endif

    return my_file;
}

Size_file _get_size_file(File my_file)
{

    Size_file fileSize = 0;

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__NT__)
#ifdef _WIN64

    wchar_t file_path_name[MAX_PATH];
    DWORD file_path = GetFinalPathNameByHandleW(my_file, file_path_name, MAX_PATH, VOLUME_NAME_DOS);
    // obtener el nombre del archivo abierto mediante su handle

    if (file_path != 0)
    {
        // obtener el tamaño del archivo mediante sus atributos mediante la funcion correspondiente de 64bits
        WIN32_FILE_ATTRIBUTE_DATA fileAttrData;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
        GetFileAttributesExW(file_path_name, GetFileExInfoStandard, &fileAttrData);
#pragma GCC diagnostic pop
        fileSize = ((Size_file)fileAttrData.nFileSizeHigh << 32) | fileAttrData.nFileSizeLow;
        return fileSize;
    }
#else
    // para masquinas windows de 32bits
    fileSize = GetFileSize(my_file, NULL);
#endif

#elif __linux__
    if (my_file == OPEN_ERROR || my_file == NULL)
    {
        fileSize = READ_ERROR;
    }
    else
    {
        // Mueve el indicador de posición al final del archivo
        fseek(my_file, 0L, SEEK_END);
        // Obtiene la posición actual del indicador(tamaño del archivo)
        fileSize = ftell(my_file);
        //
        rewind(my_file);
    }
#endif

    return fileSize;
}

void open_file(MyFile *my_file, name_file name_file_open, mode mode_open)
{
    my_file->archivo = open_f(name_file_open, mode_open);
    my_file->size = _get_size_file(my_file->archivo);
    my_file->data = NULL;
    my_file->last_char = 0;
}

void write_file(MyFile *my_file, const char *data)
{

    if (my_file->archivo == NULL)
    {
        my_file->size = WRITE_ERROR;
        my_file->data = NULL;
        return;
    }
    else
    {
        my_file->data = (char *)data;
        if (my_file->size == 0)
        {
            my_file->size = strlen(my_file->data);
        }
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__NT__)
#ifdef _WIN64
        OVERLAPPED overlapped = {0};

        if (!WriteFileEx(my_file->archivo, my_file->data, my_file->size, &overlapped, NULL))
        {
            my_file->size = WRITE_ERROR;
            my_file->data = NULL;
            my_file->last_char = 0;
        }
#else
        if (!WriteFile(my_file->archivo, my_file->data, my_file->size, &(my_file->last_char), NULL))
        {
            my_file->size = WRITE_ERROR;
            my_file->data = NULL;
        }
#endif
#elif __linux__
        if (fputs(my_file->data, my_file->archivo) == EOF)
        {
            my_file->size = WRITE_ERROR;
            my_file->data = NULL;
        }
#endif
    }
}

void read_file(MyFile *my_file)
{
    if (my_file->data == NULL)
    {
        my_file->data = (char *)malloc(my_file->size * sizeof(char));
    }
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__NT__)
#ifdef _WIN64
    OVERLAPPED overlapped = {0};
    HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    overlapped.hEvent = hEvent;
    if (!ReadFileEx(my_file->archivo, my_file->data, my_file->size, &overlapped, NULL))
    {
        if (GetLastError() != ERROR_IO_PENDING)
        {
            my_file->data = READ_ERROR;
        }
        else
        {
            WaitForSingleObject(overlapped.hEvent, INFINITE);
        }
    }
    CloseHandle(overlapped.hEvent);
#else
    if (!ReadFile(my_file->archivo, my_file->data, my_file->size, &(my_file->last_char), NULL))
    {
        my_file->data = READ_ERROR;
    }
#endif
#elif __linux__
    my_file->last_char = fread(my_file->data, sizeof(char), my_file->size, my_file->archivo);
    if (my_file->last_char == 0)
    {
        my_file->data = READ_ERROR;
    }
    /*for (my_file->last_char = 0; my_file->last_char < my_file->size; my_file->last_char++){
        char ch = fgetc(my_file->archivo);
        printf("%c\n", ch);
        my_file->data[my_file->last_char] = ch;
    }*/
#endif
}

void close_file(MyFile *my_file)
{
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__NT__)
    CloseHandle(my_file->archivo);
#elif __linux__
    fclose(my_file->archivo);
#endif
}

#endif
