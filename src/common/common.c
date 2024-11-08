#include "common.h"

void free_matrix(int **matrix, int H) {
  if (matrix != NULL) {
    for (int i = 0; i < H; i++) {
      if (matrix[i] != NULL) {
        free(matrix[i]);
      }
    }
    free(matrix);
    matrix = NULL;
  }
}

int **create_matrix(int H, int W) {
  int **matrix = (int **)malloc(H * sizeof(int *));
  for (int i = 0; i < H; i++) {
    int *row = (int *)malloc(W * sizeof(int));
    matrix[i] = row;
    for (int j = 0; j < W; j++) {
      matrix[i][j] = 0;
    }
  }
  return matrix;
}

void copy_matrix(int **dest, int **src, int H, int W) {
  for (int i = 0; i < H; i++) {
    for (int j = 0; j < W; j++) {
      dest[i][j] = src[i][j];
    }
  }
}