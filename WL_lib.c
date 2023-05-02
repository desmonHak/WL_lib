#ifndef __WL_LIB_C__
#define __WL_LIB_C__

File open_f(name_file name_file_open, mode mode_open)
{

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__NT__)
    File my_file = CreateFile(
        name_file_open,
        mode_open,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

#elif __linux__
    File my_file = open(
        name_file_open,
        mode_open);
#endif

    return my_file;
}

Size_file get_size_file(File my_file)
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
    // Mueve el indicador de posición al final del archivo
    fseek(my_file, 0, SEEK_END);
    // Obtiene la posición actual del indicador(tamaño del archivo)
    fileSize = ftell(file);
#endif

    return fileSize;
}

void open_file(MyFile *my_file, name_file name_file_open, mode mode_open)
{
    my_file->archivo = open_f(name_file_open, mode_open);
    my_file->size = get_size_file(my_file->archivo);
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
        my_file->data = data;
        if (my_file->size == 0)
        {
            my_file->size = strlen(my_file->data);
        }
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__NT__)
#ifdef _WIN64
OVERLAPPED overlapped = { 0 };

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
    my_file->last_char = fread(my_file->data, 1, my_file->size, my_file->archivo);
    if (my_file->last_char == 0)
    {
        my_file->data = READ_ERROR;
    }
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
