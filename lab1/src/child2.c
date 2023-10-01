#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 1024

int main() {
    char *buffer = malloc(BUFFER_SIZE);
    ssize_t bytesRead;


    while ((bytesRead = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        // Проверяем, была ли введена пустая строка
        if (bytesRead == 1 && buffer[0] == '\n') {
            break;
        }

        // Удаляем символ новой строки
        if (buffer[bytesRead - 1] == '\n') {
            buffer[bytesRead - 1] = '\0';
        }
        for (int i = 0; i < bytesRead - 1; i++) {
            if (buffer[i] == ' ') buffer[i] = '_'; 
        }

        // Вывод строки
        write(STDOUT_FILENO, buffer, strlen(buffer));
        write(STDOUT_FILENO, "\n", 1);
    }

    free(buffer);
    return 0;
}