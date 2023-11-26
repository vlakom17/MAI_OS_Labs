#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <sys/mman.h> 
#include <semaphore.h> 
 
void create_two_processes(int* a, int* b) { 
    *a = fork(); 
    if (*a > 0) { 
        *b = fork(); 
    } 
    if (*a == -1 || *b == -1) { 
        perror("fork error\n"); 
        exit(-1); 
    } 
} 
int main(int argc, char *argv[]) { 
    if (argc != 2) { 
        printf("Wrong number of args\n"); 
        exit(-1); 
    }
    FILE *f = fopen("output.txt","w");
    fclose(f);
    int parent_pid = getpid(), pid1, pid2; 
    // Opening of the mapped file 
    int fd = open(argv[1], O_RDWR | O_CREAT | O_APPEND, 0644); 
    if (fd == -1) { 
        perror("Can't open file\n"); 
        exit(-1); 
    } 
    ftruncate(fd, 1000); 
    size_t file_size = lseek(fd, 0, SEEK_END); 
    char* mapped = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
    if (mapped == MAP_FAILED) { 
        perror("Map failed\n"); 
        exit(-1); 
    } 
    // Reading test file
    char c;
    int i = 0;
    while ((c = getchar()) != EOF) {
        mapped[i] = c;
        i++;
    }
    // Semaphore creating
    sem_t* sem = sem_open("s_laba3", O_CREAT, 0777, 0); 
    if (sem == SEM_FAILED) { 
        perror("Semaphore failed\n"); 
        exit(-1); 
    } 
    sem_post(sem); 
    create_two_processes(&pid1, &pid2); // Create Child1 & Child2 processes
    // Parent process
    if (pid1 > 0 && pid2 > 0) {  
        waitpid(pid1, NULL, WUNTRACED); 
        waitpid(pid2, NULL, WUNTRACED); 
        ftruncate(fd, file_size); 
        printf("%s\n", mapped); 
    } 
    // Child1 process 
    else if (pid1 == 0) { 
        execl("./child1", "./child1", argv[1], "/s_laba3", NULL); 
    } 
    // Child2 process 
    else { 
        execl("./child2", "./child2", argv[1], "/s_laba3", NULL); 
    } 
    sem_close(sem); 
    sem_unlink("/s_laba3"); 
    printf("The result was written to %s\n", argv[1]); 
    munmap(mapped, file_size); 
    close(fd); 
    return 0; 
}
