#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>
#include <string.h>

#include "X11Macros.h"

#define WALL_TEMP 20.0
#define FP_TEMP 100.0
#define MAX_N 500
#define DEFAULT_N 100
#define DEFAULT_T 100
#define THREADS 4

const char *POINTS = "Enter the number of points in each dimension, currently 100";
const char *READ_ERROR = "Error reading input";
const char *TIME = "Enter the number of time steps, currently 100";
const char *NOTICE = "Scale factor = 10. Note if > 1, actual borders on two sides (right and bottom) will not display";
const char *FP_PRELUDE = "Begin and end of fireplace: ";
const char *INITIAL = "Initial numbers";
const char *FINAL = "Sequential execution numbers";
const char *PARALLEL = "Parallel execution numbers";

// Used to hold a pair of integers for fireplace dimensions
struct Int_Pair {
  int first;
  int last;
};

//functions
int getInput(const char* prompt);
struct Int_Pair getFireplace(int dimension);
bool errorCheck(int dimension, int iter, double (*a)[MAX_N][MAX_N] , double (*b)[MAX_N][MAX_N]);
void printArr(double (*arr)[MAX_N][MAX_N], int dimension, int final_index, const char* str);
void initialize(double (*arr)[MAX_N][MAX_N], int dimension, int start, int end, int current, int next);
void propogate(double (*arr)[MAX_N][MAX_N], int dimension, int iterations);
void parallelPropogate(double (*arr)[MAX_N][MAX_N], int dimension, int iterations);
void draw(double (*arr)[MAX_N][MAX_N], int dimension, int index);
long getColor(double val);
void close_x();

// Return integer input after printing prompt
int getInput(const char* prompt) {
  int to_return;
  printf("%s\n", prompt);
  int result = scanf("%d", &to_return);
  if (result == EOF || result == 0) {
    printf("%s\n", READ_ERROR);
    exit(EXIT_FAILURE);
  }
  return to_return;
}

// Given the dimension, return the size of the fireplace
struct Int_Pair getFireplace(int dimension) {
  int begin, end;
  begin = (int) (dimension * .4);
  end = (int) (dimension * .6) - 1;
  struct Int_Pair to_return = {begin, end};
  return to_return;
}

void printArr(double (*arr)[MAX_N][MAX_N], int dimension, int final_index, const char* str) {
  int i,j;
  int step = dimension / 10;
  printf("%s\n", str);
  for(i = 0; i < dimension; i = i + step) {
    if (i > 0)
      printf("\n");
    for(j = 0; j < dimension; j = j + step) {
      printf("%6.2f\t", arr[final_index][i][j]);
    }
  }
  printf("\n");
}

void initialize(double (*arr)[MAX_N][MAX_N], int dimension, int start, int end, int current, int next) {
  int i,j;

  for(i = 0; i < dimension; i++) {
    for(j = 0; j < dimension; j++) {
      if (i == 0 && j >= start && j <= end) {
        arr[current][i][j] = 100.0;
        arr[next][i][j] = 100.0;
      } else {
        arr[current][i][j] = 20.0;
        arr[next][i][j] = 20.0;
      }
    }
  }
}

bool errorCheck(int dimension, int iter, double (*a)[MAX_N][MAX_N] , double (*b)[MAX_N][MAX_N]) {
  int i,j;

  for(i = 0; i < dimension; i++) {
    for(j = 0; j < dimension; j++) {
      if ((a[iter][i][j] - b[iter][i][j]) > 0.01 || (b[iter][i][j] - a[iter][i][j] > 0.01))
        return true;
    }
  }
  return false;
}

void propogate(double (*arr)[MAX_N][MAX_N], int dimension, int iterations) {
  int current = 0;
  int next = 1;
  int iter, i, j;

  for (iter = 0; iter < iterations; iter++) {
    for (i = 1; i < dimension - 1; i++)
      for (j = 1; j < dimension - 1; j++)
        arr[next][i][j] = 0.25 * (arr[current][i-1][j] + arr[current][i+1][j]
                              + arr[current][i][j-1] + arr[current][i][j+1]);
    current = next;
    next = 1 - current;
  }
}

void parallelPropogate(double (*arr)[MAX_N][MAX_N], int dimension, int iterations) {
  int current = 0;
  int next = 1;
  int iter, i, j;

  for (iter = 0; iter < iterations; iter++) {
    #pragma omp parallel for shared(dimension,iterations,arr,current,next) private(i,j,iter) collapse(2)
    for (i = 1; i < dimension - 1; i++) {
      for(j = 1; j < dimension - 1; j++) {
        arr[next][i][j] = 0.25 * (arr[current][i-1][j] + arr[current][i+1][j]
                              + arr[current][i][j-1] + arr[current][i][j+1]);
      }
    }
    #pragma barrier
    current = next;
    next = 1 - current;
  }

}

void draw(double (*arr)[MAX_N][MAX_N], int dimension, int index) {
    int i,j;
    initX11(dimension,dimension); // X11 setup
    XClearWindow(display, win);
    usleep(10000); //if X11 hadn't launched I needed additional time before drawing

    for(i = 0; i < dimension; i++) {
      for(j = 0; j < dimension; j++) {
        double current = arr[index][i][j];
        long clr = getColor(current);
        XSetForeground(display, gc, clr);
        XDrawPoint(display, win, gc, j, i);
      }
    }
    XFlush(display);
    XEvent event;
    while(1) {
      XNextEvent(display, &event);
      if (event.type==KeyPress) {
        XFreeGC(display, gc);
        XDestroyWindow(display, win);
        XCloseDisplay(display);
        exit(EXIT_SUCCESS);
      }
    }
}

long getColor(double val) {
  if (val < 10.0)
    return CYAN;
  else if (val < 20.0)
    return BLUE;
  else if (val < 30.0)
    return GREEN;
  else if (val < 40.0)
    return YELLOW;
  else if (val < 50.0)
    return ORANGE;
  else if (val < 60.0)
    return RED;
  else if (val < 70.0)
    return PURPLE;
  else if (val < 80.0)
    return MAGENTA;
  else if (val < 90.0)
    return VIOLET;
  else
    return BLACK;
}

int main(void) {
  omp_set_num_threads(THREADS);
  int N,T;
  int user_N, user_T;
  int current, next;
  struct Int_Pair fireplace;
  double seq[2][MAX_N][MAX_N];
  double par[2][MAX_N][MAX_N];
  int i,j;
  int index_of_final;
  double start, finish, sequential_time, parallel_time;

  //Default values of variables to follow
  N = DEFAULT_N;
  T = DEFAULT_T;
  current = 0;
  next = 1;

  N = getInput(POINTS);
  T = getInput(TIME);
  printf("%s\n", NOTICE);
  fireplace = getFireplace(N);
  printf("%s %d to %d\n", FP_PRELUDE, fireplace.first, fireplace.last);
  if (T % 2 == 0) {
    index_of_final = 0;
  } else {
    index_of_final = 1;
  }

  initialize(seq, N, fireplace.first, fireplace.last, current, next);
  initialize(par, N, fireplace.first, fireplace.last, current, next);

  printArr(seq, N, index_of_final, INITIAL);
  start = omp_get_wtime();
  propogate(seq, N, T);
  finish = omp_get_wtime();
  sequential_time = finish - start;
  printArr(seq, N, index_of_final, FINAL);
  start = omp_get_wtime();
  parallelPropogate(par, N, T);
  finish = omp_get_wtime();
  parallel_time = finish - start;
  printArr(par, N, index_of_final, PARALLEL);

  if (errorCheck(N, index_of_final, seq, par)) {
    printf("Error:Sequential and Parallel are not equal.");
    return(1);
  }

  printf("Speed up factor: %f", sequential_time/parallel_time);

  draw(seq,N,index_of_final);
}
