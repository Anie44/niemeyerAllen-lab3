// lab3.c
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab3.h"

extern int** sudoku_board;
int* worker_validation;

int** read_board_from_file(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening file");
        return NULL;
    }

    int** board = malloc(ROW_SIZE * sizeof(int*));
    for (int i = 0; i < ROW_SIZE; i++) {
        board[i] = malloc(COL_SIZE * sizeof(int));
        for (int j = 0; j < COL_SIZE; j++) {
            if (fscanf(fp, "%d,", &board[i][j]) != 1) {
                fprintf(stderr, "Error reading from file.\n");
                fclose(fp);
                return NULL;
            }
        }
    }

    fclose(fp);
    return board;
}

void* validate(void* arg) {
    param_struct* params = (param_struct*)arg;
    int* validation = &worker_validation[params->id];

    for (int i = params->starting_row; i <= params->ending_row; i++) {
        for (int j = params->starting_col; j <= params->ending_col; j++) {
            for (int k = i + 1; k <= params->ending_row; k++) {
                if (params->is_row && sudoku_board[i][j] == sudoku_board[k][j]) {
                    *validation = 0;
                    pthread_exit(NULL);
                } else if (!params->is_row && sudoku_board[j][i] == sudoku_board[j][k]) {
                    *validation = 0;
                    pthread_exit(NULL);
                }
            }
        }
    }

    *validation = 1;
    pthread_exit(NULL);
}

int is_board_valid() {
    pthread_t tid[NUM_OF_THREADS];
    pthread_attr_t attr;
    worker_validation = malloc(NUM_OF_THREADS * sizeof(int));

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int index = 0;

    for (int i = 0; i < ROW_SIZE; i++) {
        param_struct* params = malloc(sizeof(param_struct));
        params->id = index++;
        params->starting_row = i;
        params->ending_row = i;
        params->starting_col = 0;
        params->ending_col = COL_SIZE - 1;
        params->is_row = 1;
        pthread_create(&tid[params->id], &attr, validate, (void*)params);
    }

    for (int j = 0; j < COL_SIZE; j++) {
        param_struct* params = malloc(sizeof(param_struct));
        params->id = index++;
        params->starting_row = 0;
        params->ending_row = ROW_SIZE - 1;
        params->starting_col = j;
        params->ending_col = j;
        params->is_row = 0;
        pthread_create(&tid[params->id], &attr, validate, (void*)params);
    }

    for (int i = 0; i < ROW_SIZE; i += 3) {
        for (int j = 0; j < COL_SIZE; j += 3) {
            param_struct* params = malloc(sizeof(param_struct));
            params->id = index++;
            params->starting_row = i;
            params->ending_row = i + 2;
            params->starting_col = j;
            params->ending_col = j + 2;
            params->is_row = 1; // Subgrids are treated as rows
            pthread_create(&tid[params->id], &attr, validate, (void*)params);
        }
    }

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    int board_valid = 1;
    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (worker_validation[i] == 0) {
            board_valid = 0;
            break;
        }
    }

    free(worker_validation);
    pthread_attr_destroy(&attr);

    return board_valid;
}
