#include "merge_sort.h"
/// @brief TESTER LAB ECOLEEEEEEEE
/// @param left a
/// @param right a
/// @param array a
/// @param size a
/// @param action a

void write_array_into_file(int left, int right, int *array, int size, const char *action) {
    FILE *log_file = fopen("sorted_array.txt", "a");

    fprintf(log_file, "%s\n", action);
    fprintf(log_file, "start = %d, end = %d, sorted = [", left, right);

    for (int i = left; i <= right; i++) {
        fprintf(log_file, "%d", array[i]);
        if (i < right) {
            fprintf(log_file, ", ");
        }
    }

    fprintf(log_file, "]\n");
    fclose(log_file);
}

void time_sort(struct timeval *start, struct timeval *end) {
    long seconds, useconds;
    seconds = end->tv_sec - start->tv_sec;
    useconds = end->tv_usec - start->tv_usec;

    printf("Temps d'exécution du tri : %ld secondes et %ld microsecondes\n", seconds, useconds);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <array_size> <num_processes>\n", argv[0]);
        exit(1);
    }

    int array_size = atoi(argv[1]);
    int num_processes = atoi(argv[2]);
    int segment_size = array_size / num_processes;

    shared_data = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    shared_data->array = mmap(NULL, array_size * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    shared_data->size = array_size;

    printf("Array size: %d\n", array_size);
    printf("Number of processes: %d\n", num_processes);
    printf("Segment size: %d\n", segment_size);

    /* Populate the array to test the sort */
    srand(time(NULL));
    for (int i = 0; i < array_size; i++) {
        shared_data->array[i] = rand() % MAX_NUM_SIZE;
    }

    show_array();  // Avant le tri

    FILE *log_file = fopen("sorted_array.txt", "w");
    
    fprintf(log_file, "Array = [");
    for (int i = 0; i < array_size; i++) {
        fprintf(log_file, "%d", shared_data->array[i]);
        if (i < array_size - 1) {
            fprintf(log_file, ", ");
        }
    }
    fprintf(log_file, "]\n\n");
    fclose(log_file);

    pid_t pids[num_processes];
    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < num_processes; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            int left = i * segment_size;
            int right = (i == num_processes - 1) ? (array_size - 1) : (left + segment_size - 1);
            merge_sort(left, right);

            char action[100];
            snprintf(action, sizeof(action), "Process %d sorted", i);
            write_array_into_file(left, right, shared_data->array, shared_data->size, action);

            exit(0);
        } 
    }

    while(wait(NULL) > 0);

    for (int i = 1; i < num_processes; i++) {
        merge(0, (i * segment_size) - 1, array_size - 1);
    }
    write_array_into_file(0, array_size - 1, shared_data->array, shared_data->size, "\nLast sorted");

    show_array();  // Après tri

    gettimeofday(&end, NULL);
    time_sort(&start, &end);

    munmap(shared_data->array, array_size * sizeof(int));
    munmap(shared_data, sizeof(SharedData));

    return 0;   
}

void merge_sort(int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(left, mid);
        merge_sort(mid + 1, right);
        merge(left, mid, right);
    }
}

void merge(int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));

    for (i = 0; i < n1; i++)
        L[i] = shared_data->array[left + i];
    for (j = 0; j < n2; j++)
        R[j] = shared_data->array[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            shared_data->array[k] = L[i];
            i++;
        } else {
            shared_data->array[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        shared_data->array[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        shared_data->array[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

void show_array(){
    printf("Sorted array: ");
    for (int i = 0; i < shared_data->size; i++) {
        printf("%d ", shared_data->array[i]);
    }
    printf("\n");
}

// void log_sort(int left, int right, int *array, int size, const char *action) {
//     FILE *log_file = fopen("sorted_array.txt", "a");


//     fprintf(log_file, "%s\n", action);
//     fprintf(log_file, "Start = %d, End = %d, sorted = [", left, right);

//     for (int i = left; i <= right; i++) {
//         fprintf(log_file, "%d", array[i]);
//         if (i < right) {
//             fprintf(log_file, ", ");
//         }
//     }

//     fprintf(log_file, "]\n");
//     fclose(log_file);
// }

// void time_sort(struct timeval *start, struct timeval *end) {
//     long seconds, useconds;
//     seconds = end->tv_sec - start->tv_sec;
//     useconds = end->tv_usec - start->tv_usec;

//     printf("Temps d'exécution du tri : %ld secondes et %ld microsecondes\n", seconds, useconds);
// }

// void merge_sort_thread(int left, int right);

// void *merge_sort_worker(void *arg) {
//     int *params = (int *)arg;
//     int left = params[0];
//     int right = params[1];

//     merge_sort_thread(left, right);
//     free(arg);  // Free allocated memory for the thread params
//     return NULL;
// }

// void merge_sort_thread(int left, int right) {
//     if (left < right) {
//         int mid = left + (right - left) / 2;

//         // Create threads for left and right halves
//         pthread_t left_thread, right_thread;
//         int *left_params = malloc(2 * sizeof(int));
//         int *right_params = malloc(2 * sizeof(int));
//         left_params[0] = left;
//         left_params[1] = mid;
//         right_params[0] = mid + 1;
//         right_params[1] = right;

//         pthread_create(&left_thread, NULL, merge_sort_worker, left_params);
//         pthread_create(&right_thread, NULL, merge_sort_worker, right_params);

//         // Wait for both threads to finish
//         pthread_join(left_thread, NULL);
//         pthread_join(right_thread, NULL);

//         merge(left, mid, right);
//     }
// }

// void merge(int left, int mid, int right) {
//     int i, j, k;
//     int n1 = mid - left + 1;
//     int n2 = right - mid;

//     int L[n1], R[n2];

//     for (i = 0; i < n1; i++)
//         L[i] = shared_data->array[left + i];
//     for (j = 0; j < n2; j++)
//         R[j] = shared_data->array[mid + 1 + j];

//     i = 0;
//     j = 0;
//     k = left;

//     while (i < n1 && j < n2) {
//         if (L[i] <= R[j]) {
//             shared_data->array[k] = L[i];
//             i++;
//         } else {
//             shared_data->array[k] = R[j];
//             j++;
//         }
//         k++;
//     }

//     while (i < n1) {
//         shared_data->array[k] = L[i];
//         i++;
//         k++;
//     }

//     while (j < n2) {
//         shared_data->array[k] = R[j];
//         j++;
//         k++;
//     }
// }

// void show_array() {
//     printf("Sorted array: ");
//     for (int i = 0; i < shared_data->size; i++) {
//         printf("%d ", shared_data->array[i]);
//     }
//     printf("\n");
// }

// int main(int argc, char *argv[]) {
//     if (argc != 3) {
//         fprintf(stderr, "Usage: %s <array_size> <num_threads>\n", argv[0]);
//         exit(1);
//     }

//     int array_size = atoi(argv[1]);
//     int num_threads = atoi(argv[2]);
//     int segment_size = array_size / num_threads;

//     shared_data = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
//     shared_data->array = mmap(NULL, array_size * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
//     shared_data->size = array_size;

//     printf("Array size: %d\n", array_size);
//     printf("Number of threads: %d\n", num_threads);
//     printf("Segment size: %d\n", segment_size);

//     /* Populate the array to test the sort */
//     srand(time(NULL));
//     for (int i = 0; i < array_size; i++) {
//         shared_data->array[i] = rand() % MAX_NUM_SIZE;
//     }

//     show_array(); // Before sorting

//     FILE *log_file = fopen("sorted_array.txt", "w");

//     fprintf(log_file, "Array = [");
//     for (int i = 0; i < array_size; i++) {
//         fprintf(log_file, "%d", shared_data->array[i]);
//         if (i < array_size - 1) {
//             fprintf(log_file, ", ");
//         }
//     }
//     fprintf(log_file, "]\n\n");
//     fclose(log_file);

//     pthread_t threads[num_threads];
//     struct timeval start, end;
//     gettimeofday(&start, NULL);

//     // Create threads for sorting
//     for (int i = 0; i < num_threads; i++) {
//         int *params = malloc(2 * sizeof(int));
//         params[0] = i * segment_size;
//         params[1] = (i == num_threads - 1) ? (array_size - 1) : (params[0] + segment_size - 1);

//         pthread_create(&threads[i], NULL, merge_sort_worker, params);
//     }

//     // Wait for all threads to finish
//     for (int i = 0; i < num_threads; i++) {
//         pthread_join(threads[i], NULL);
//     }

//     // Merge the results
//     for (int i = 1; i < num_threads; i++) {
//         merge(0, (i * segment_size) - 1, array_size - 1);
//     }
//     log_sort(0, array_size - 1, shared_data->array, shared_data->size, "\nlast sorted");

//     show_array(); // After sorting

//     gettimeofday(&end, NULL);
//     time_sort(&start, &end);

//     munmap(shared_data->array, array_size * sizeof(int));
//     munmap(shared_data, sizeof(SharedData));

//     return 0;
// }
