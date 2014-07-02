#include <stdio.h>
#include <assert.h>

const int NUM = 9;
const int N = NUM * NUM;
const int NEIGHBOR = 20;
int spaces[N];
int board[N];
int nspaces;
int neighhbors[N][NEIGHBOR];

void init_neighbors() {
  int index = 0;
  for(int i=0; i < N; i++) {
    // row
    int row = i % NUM;
    for(int j = 0; j < NUM; j++) {
      int label = row * NUM + j;
      if ( label != i ) {
        neighhbors[i][index++] = label;
      }
    }
    // col
    int col = i / NUM;
    for(int j=0; j < NUM; j++) {
      int label = j * NUM + col;
      if ( label != i ) {
        neighhbors[i][index++] = label;
      }
    }
    // cube
    int cube_x = row - row % 3;
    int cube_y = col - col % 3;
    for(int x = cube_x; x < cube_x + 3; x++) {
      for ( int y = cube_y; y < cube_y + 3; y++) {
        int label = x * NUM + y;
        if ( x == row || y == col) {
          continue;
        }
        neighhbors[i][index++] = label;
        assert(index <= NEIGHBOR);
      }
    }

  }
}

void print_neighbors() {
  for(int i = 0; i < N; i++) {
    printf("Cell[%d]: ",i);
    for(int j =0; j < NEIGHBOR; j++) {
      printf("%d",neighhbors[i][j]);
    }
  }
}

bool avaliable(int guess,int cell) {
  assert(guess >0 && guess <= NUM);
  assert(cell >=0 && cell < N);
  for(int i = 0; i < NEIGHBOR; i++) {
    int neighbor = neighhbors[cell][i];
    if( board[neighbor] == guess) {
      return false;
    }
  }
  return true;
}
bool solve(int which_space)
{
  if ( which_space >= nspaces) {
    return true;
  }

  int cell = spaces[which_space];
  for(int guess = 1; guess <= NUM; guess++) {
    if(avaliable(guess,cell)) {
      // hold
      assert(board[cell] == 0);
      board[cell] = guess;

      if(solve(which_space + 1)) {
        return true;
      }
      // unhold
      assert(board[cell] == guess);
      board[cell] = 0;
    }
  }

  return false;
}
int main() {
  init_neighbors();
  print_neighbors();
  return 0;
}
