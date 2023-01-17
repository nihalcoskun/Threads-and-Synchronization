#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <ctype.h>

#define MAX_LINE_LEN 256

// Global variables
char **data_array;
int lines_read = 0;
sem_t read_sem;
sem_t upper_sem;
sem_t replace_sem;
sem_t write_sem;

typedef struct thread_args{
    int thread_id;
}thread_args;

// Thread functions

void *read_thread(void *args) {
    thread_args *my_args = (thread_args*) args;
    int thread_id = my_args->thread_id;
    int line_number;

    while(1) {
        // Acquire the semaphore for reading
        sem_wait(&read_sem);

        // Check if all lines have been read
        if (line_number == lines_read) {
            sem_post(&read_sem);
            break;
        }

        // Assign a unique line number
        line_number = lines_read++;

        // Release the semaphore
        sem_post(&read_sem);

        // Read the line from the file
        FILE *fp = fopen("input.txt", "r");
        char line[MAX_LINE_LEN];
        for (int i = 0; i < line_number; i++) {
            fgets(line, MAX_LINE_LEN, fp);
        }
        fclose(fp);

        // Store the line in the array
        data_array[line_number] = strdup(line);

        printf("Read thread %d read line %d: %s", thread_id, line_number, line);
    }

    pthread_exit(NULL);
}

void *upper_thread(void *args) {
    thread_args *my_args = (thread_args*) args;
    int thread_id = my_args->thread_id;
    int line_number;

    while(1) {
        // Acquire the semaphore for upper casing
        sem_wait(&upper_sem);

        // Check if all lines have been upper cased
        if (line_number == lines_read) {
            sem_post(&upper_sem);
            break;
        }

        // Assign a unique line number
        line_number = lines_read++;

        // Release the semaphore
        sem_post(&upper_sem);

        // Convert the string to uppercase
        for (int i = 0; data_array[line_number][i]; i++) {
            data_array[line_number][i] = toupper((unsigned char) data_array[line_number][i]);
        }

        printf("Upper thread %d upper cased line %d: %s", thread_id, line_number, data_array[line_number]);
    }

    pthread_exit(NULL);
}

void *replace_thread(void *args) {
    thread_args *my_args = (thread_args*) args;
    int thread_id = my_args->thread_id;
    int line_number;

    while(1) {
        // Acquire the semaphore for replacing spaces
        sem_wait(&replace_sem);

        // Check if all lines have been replaced
        if (line_number == lines_read) {
            sem_post(&replace_sem);
            break;
        }

        // Assign a unique line number
        line_number = lines_read++;

        // Release the semaphore
        sem_post(&replace_sem);

        // Replace spaces with underscores
        char *p = data_array[line_number];
        while (*p != '\0') {
            if (*p == ' ') {
                *p = '_';
            }
            p++;
        }

        printf("Replace thread %d replaced spaces in line %d: %s", thread_id, line_number, data_array[line_number]);
    }

    pthread_exit(NULL);
}

void *write_thread(void *arg) {
     thread_args *my_args = (thread_args*) arg;
    int thread_id = my_args->thread_id;
    int line_number;

    while(1) {
        // Acquire the semaphore for writing
        sem_wait(&write_sem);

        // Check if all
        thread_args *my_args = (thread_args*) arg;
    int thread_id = my_args->thread_id;
    int line_number;

    while(1) {
        // Acquire the semaphore for writing
        sem_wait(&write_sem);

        // Check if all lines have been written
        if (line_number == lines_read) {
            sem_post(&write_sem);
            break;
        }

        // Assign a unique line number
        line_number = lines_read++;

        // Release the semaphore
        sem_post(&write_sem);

        // Write the line to the file
        FILE *fp = fopen("output.txt", "w");
        for (int i = 0; i < lines_read; i++) {
            fputs(data_array[i], fp);
        }
        fclose(fp);

        printf("Write thread %d wrote line %d: %s", thread_id, line_number, data_array[line_number]);
    }

    pthread_exit(NULL);
}
}
int main(int argc, char *argv[]) {
    // Parse the command line arguments
    char *file_name = argv[1];
    int num_read_threads = atoi(argv[2]);
    int num_upper_threads = atoi(argv[3]);
    int num_replace_threads = atoi(argv[4]);
     int num_write_threads = atoi(argv[5]);

      // Initialize the semaphores
    sem_init(&read_sem, 0, 1);
    sem_init(&upper_sem, 0, 1);
    sem_init(&replace_sem, 0, 1);
    sem_init(&write_sem, 0, 1);

    // Allocate memory for the data array
    data_array = (char **)malloc(sizeof(char *) * num_read_threads);

    // Create the read threads
    pthread_t read_threads[num_read_threads];
    struct thread_args *read_args = malloc(sizeof(struct thread_args) * num_read_threads);
    for (int i = 0; i < num_read_threads; i++) {
        read_args[i].thread_id = i;
        pthread_create(&read_threads[i], NULL, read_thread, &read_args[i]);
    }

    // Create the upper threads
    pthread_t upper_threads[num_upper_threads];
    struct thread_args *upper_args = malloc(sizeof(struct thread_args) * num_upper_threads);
    for (int i = 0; i < num_upper_threads; i++) {
        upper_args[i].thread_id = i;
        pthread_create(&upper_threads[i], NULL, upper_thread, &upper_args[i]);
    }

    // Create the replace threads
    pthread_t replace_threads[num_replace_threads];
    struct thread_args *replace_args = malloc(sizeof(struct thread_args) * num_replace_threads);
    for (int i = 0; i < num_replace_threads; i++) {
        replace_args[i].thread_id = i;
        pthread_create(&replace_threads[i], NULL, replace_thread, &replace_args[i]);
    }

    // Create the write thread
    pthread_t write_threads[num_write_threads];
    struct thread_args *write_args = malloc(sizeof(struct thread_args) * num_write_threads);
    for (int i = 0; i < num_write_threads; i++) {
        write_args[i].thread_id = i;
        pthread_create(&write_threads[i], NULL, write_thread, &write_args[i]);
    }
    // Wait for the threads to complete
    for (int i = 0; i < num_read_threads; i++) {
        pthread_join(read_threads[i], NULL);
    }

    for (int i = 0; i < num_upper_threads; i++) {
        pthread_join(upper_threads[i], NULL);
    }

    for (int i = 0; i < num_replace_threads; i++) {
        pthread_join(replace_threads[i], NULL);
    }

    for (int i = 0; i < num_write_threads; i++) {
        pthread_join(write_threads[i], NULL);
    }
    //Free the memory
    free(read_args);
    free(upper_args);
    free(replace_args);
    free(write_args);

    // Cleanup and exit
    for (int i = 0; i < lines_read; i++) {
        free(data_array[i]);
    }
    free(data_array);

    sem_destroy(&read_sem);
    sem_destroy(&upper_sem);
    sem_destroy(&replace_sem);
    sem_destroy(&write_sem);
    return 0;
}


