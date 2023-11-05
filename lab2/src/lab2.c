#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int round_count; // Количество раундов
int successes = 0; // Количество успешных раундов
int max_threads; // Максимальное количество потоков
pthread_mutex_t mutex; // Мьютекс для обеспечения синхронизации

// Функция для обработки данных в потоке
void* process_data(void* arg) {
    int thread_id = *(int*)arg;
    //printf("%d\n", thread_id);
    int rounds_per_thread = round_count / max_threads; // Количество раундов для каждого потока
    int remaining_rounds = 0;
    int a[round_count];
    int b[round_count];
    for (int i = 0; i < round_count; i++) a[i] = 52;
    for (int i = 0; i < round_count; i++) b[i] = 52;

    // Распределение оставшихся раундов на последний поток
    if (thread_id == max_threads - 1) {
        remaining_rounds = round_count % max_threads; // Оставшиеся раунды после деления по равным частям
    }
    
    // Выполнение эксперимента для каждого потока
    for (int i = thread_id*rounds_per_thread; i < rounds_per_thread*(thread_id + 1) + remaining_rounds; i++) {

        // Перемешивание колоды карт
       do {a[i] = rand() % 52;
        b[i] = rand() % 52;} while (a[i] == b[i]);
    }
    /*for(int i = 0; i < round_count; i++) printf("%d ",a[i]);
    printf("\n");
    for(int i = 0; i < round_count; i++) printf("%d ",b[i]);
    printf("\n");*/
    for (int i = thread_id*rounds_per_thread; i < rounds_per_thread*(thread_id + 1) + remaining_rounds; i++) {
        // Проверка наличия двух одинаковых карт сверху
        if (a[i] % 13 == b[i] % 13) {
            pthread_mutex_lock(&mutex);
            successes++;
            pthread_mutex_unlock(&mutex);
        }
        
        
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Необходимо задать ограничение на количество потоков и количество раундов\n");
        return -1;
    }

    max_threads = atoi(argv[1]);
    round_count = atoi(argv[2]);


    pthread_t threads[max_threads];
    int thread_ids[max_threads];

   

    // Инициализация мьютекса
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Ошибка при инициализации мьютекса\n");
        return -1;
    }

    // Инициализация генератора случайных чисел
    srand(time(NULL));

    clock_t start_time = clock();

    // Создание и запуск потоков
    for (int i = 0; i < max_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, process_data, &thread_ids[i]);
    }

    // Ожидание завершения всех потоков
    for (int i = 0; i < max_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    clock_t end_time = clock();

    // Уничтожение мьютекса
    pthread_mutex_destroy(&mutex);
    
   
    // Расчет и вывод экспериментальной вероятности
    double experimental_probability = (double) successes / round_count;
    printf("Экспериментальная вероятность: %f\n", experimental_probability);
    
    
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC; // Подсчет времени в секундах
    printf("Время выполнения программы: %lf секунд\n", execution_time);

    return 0;
}