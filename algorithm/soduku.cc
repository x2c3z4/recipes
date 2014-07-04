/*
 * Copyright (c) 2014
 * Author: Feng,Li (lifeng1519@gmail.com)
 */

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

const int NUM = 9;
const int N = NUM * NUM;
const int NEIGHBOR = 20;
int spaces[N];
int board[N];
int nspaces;
int neighhbors[N][NEIGHBOR];

void init_neighbors() {
  for (int i = 0; i < N; i++) {
    int index = 0;
    // row,col
    int row = i / NUM;
    int col = i % NUM;
    for (int j = 0; j < NUM; j++) {
      int label_x = row * NUM + j;
      int label_y = j * NUM + col;
      if (label_x != i) {
        neighhbors[i][index++] = label_x;
      }
      if (label_y != i) {
        neighhbors[i][index++] = label_y;
      }
    }
    // cube
    int cube_x = row - row % 3;
    int cube_y = col - col % 3;
    for (int x = cube_x; x < cube_x + 3; x++) {
      for (int y = cube_y; y < cube_y + 3; y++) {
        int label = x * NUM + y;
        if (x == row || y == col) {
          continue;
        }
        neighhbors[i][index++] = label;
        assert(index <= NEIGHBOR);
      }
    }
  }
}

void print_neighbors() {
  for (int i = 0; i < N; i++) {
    printf("Cell[%d]: \n", i);
    for (int j = 0; j < NEIGHBOR; j++) {
      printf("%d, ", neighhbors[i][j]);
    }
    printf("\n\n");
  }
}

bool avaliable(int guess, int cell) {
  assert(guess > 0 && guess <= NUM);
  assert(cell >= 0 && cell < N);
  for (int i = 0; i < NEIGHBOR; i++) {
    int neighbor = neighhbors[cell][i];
    if (board[neighbor] == guess) {
      return false;
    }
  }
  return true;
}
bool solve(int which_space) {
  if (which_space >= nspaces) {
    return true;
  }

  int cell = spaces[which_space];
  for (int guess = 1; guess <= NUM; guess++) {
    if (avaliable(guess, cell)) {
      // hold
      assert(board[cell] == 0);
      board[cell] = guess;

      if (solve(which_space + 1)) {
        return true;
      }
      // unhold
      assert(board[cell] == guess);
      board[cell] = 0;
    }
  }

  return false;
}

void print_cwd() {
  char buf[256];
  getcwd(buf, sizeof(buf));
  printf("cwd: %s\n", buf);
}

void print_board(const char* des) {
  printf("%s:\n", des);
  for (int i = 0; i < NUM; i++) {
    printf("\t%d %d %d | %d %d %d | %d %d %d\n",
           board[i * NUM + 0],
           board[i * NUM + 1],
           board[i * NUM + 2],
           board[i * NUM + 3],
           board[i * NUM + 4],
           board[i * NUM + 5],
           board[i * NUM + 6],
           board[i * NUM + 7],
           board[i * NUM + 8]);
    if ( i % 3 == 2 ) {
      printf("\t---------------------\n");
    }
  }
  printf("\n");
}
void init_board_with_file(const char* file) {
  FILE* f;
  f = fopen(file, "r");
  int x, y, v;
  assert(f != NULL);
  memset(board, 0, sizeof(board));
  while (fscanf(f, "%d %d %d\n", &x, &y, &v) != EOF) {
    // printf("(%d,%d,%d)\n", x, y, v);
    int index = (x - 1) * NUM + y - 1;
    assert(index >= 0 && index < N);
    board[index] = v;
  }
  fclose(f);
}

void init_spaces() {
  nspaces = 0;
  for (int i = 0; i < N; i++) {
    if (board[i] == 0) {
      spaces[nspaces++] = i;
    }
  }
}
double timeDelta(struct timeval *stop, struct timeval *start) {
  return (stop->tv_sec - start->tv_sec) * 1000000 + (stop->tv_usec - start->tv_usec);
}

int main() {
  struct timeval t0, t1;
  gettimeofday(&t0,NULL);
  for (int i = 0; i < 100; i++) {
    init_neighbors();
    init_board_with_file("test.dat");
    print_board("init");
    init_spaces();

    solve(0);
    print_board("result");
  }
  gettimeofday(&t1,NULL);
  printf("Time: %f us\n",timeDelta(&t1, &t0) / 100);
  return 0;
}
