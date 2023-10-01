#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#define BUFFER_SIZE 1024
int main() {
    
   int parent_pid = getpid(), cpid1, cpid2;

   int pipe1fd[2]; // parent to child 1
   if (pipe(pipe1fd) == -1){
    write(STDERR_FILENO, "Ошибка в создании канала\n", 25 * sizeof(char));
   }

   int pipefd[2]; // child 1 to child 2
   if (pipe(pipefd) == -1){
    write(STDERR_FILENO, "Ошибка в создании канала\n", 25 * sizeof(char));
   }

   int pipe2fd[2]; // child 2 to parent
   if (pipe(pipe2fd) == -1){
    write(STDERR_FILENO, "Ошибка в создании канала\n", 25 * sizeof(char));
   }

   char *buffer = malloc(BUFFER_SIZE);
   size_t bytesRead;
   if ((bytesRead = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0){
        // Проверяем, была ли введена пустая строка
        if (bytesRead == 1 && buffer[0] == '\n') {
            return 0;
        }

        // Удаляем символ новой строки
        if (buffer[bytesRead - 1] == '\n') {
            buffer[bytesRead - 1] = '\0';
        }
        
        cpid1 = fork();
        if (cpid1 == -1) {
        perror("Ошибка при создании 1 дочернего процесса");
        exit(EXIT_FAILURE);
        }
        cpid2 = fork();
        if (cpid2 == -1) {
        perror("Ошибка при создании 2 дочернего процесса");
        exit(EXIT_FAILURE);
        }
        // Если это родительский процесс
        if (cpid1 > 0 && cpid2 > 0){
        close(pipe1fd[0]);
        write(pipe1fd[1], buffer, BUFFER_SIZE);
        close(pipe1fd[1]);
        // записываем из родительского процесса в первый дочерний
        char result[BUFFER_SIZE] = "";
        close(pipe2fd[1]);
        read(pipe2fd[0], result, BUFFER_SIZE);
        
        write(STDOUT_FILENO, result, BUFFER_SIZE);
        write(STDOUT_FILENO, "\n", 1);
        close(pipe2fd[0]);
        }
        // первый дочерний процесс
        else if (cpid1 == 0){
            close(pipe1fd[1]);
            dup2(pipe1fd[0], STDIN_FILENO);

            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            execl("./child1", "./child1", NULL);
        }
        // второй дочерний процесс
        else if (cpid2 == 0){
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            
            close(pipe2fd[0]);
            dup2(pipe2fd[1], STDOUT_FILENO);
            execl("./child2", "./child2", NULL);
        }
   }
   free(buffer);
   return(main());
}
