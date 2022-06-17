#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "matrix-mult-pthread.h"
//#include "function-stats.h"

/*
#define FUNC_ID_MULTIPLY 0 // Intentionally overlaps with FUNC_MAIN
#define FUNC_ID_ELEMENT 1
#define FUNC_ID_DECOMPOSE 2
#define FUNC_ID_COMPOSE 3
#define FUNC_ID_COMPUTE_M 4
#define FUNC_ID_COMPUTE_C 5
#define FUNC_ID_ADD 6
#define FUNC_ID_SUBTRACT 7
#define FUNC_ID_NAIVE_MULTIPLY 8
#define FUNC_ID_STRASSEN_MULTIPLY 9
*/

u_int16_t strassen_threshold = DEFAULT_THRESHOLD;
u_int16_t parallel_recursion = DEFAULT_RECURSION;

int32_t thread_count = 0;
pthread_mutex_t thread_count_mutex;
pthread_attr_t thread_attributes;

void create_pthread_tools(void) {

  pthread_mutex_init(&thread_count_mutex, NULL);
  pthread_attr_init(&thread_attributes);
  pthread_attr_setdetachstate(&thread_attributes, PTHREAD_CREATE_JOINABLE);
}

void destroy_pthread_tools(void) {

  pthread_mutex_destroy(&thread_count_mutex);
  pthread_attr_destroy(&thread_attributes);
}

void set_strassen_threshold(u_int16_t value) {

  strassen_threshold = value;
}

void set_parallel_recursion(u_int16_t value) {

  parallel_recursion = value;
}

void *perform_task(void *task_ptr) {

  Task *task = (Task *) task_ptr;

  //printf("%4hu Starting task %p (ID %hhu)...\n", task->size, task, task->id);
  switch (task->id) {
  case TASK_ID_M1:
    compute_m1(task->a, task->b, &task->m[0]);
    break;
  case TASK_ID_M2:
    compute_m2(task->a, task->b, &task->m[1]);
    break;
  case TASK_ID_M3:
    compute_m3(task->a, task->b, &task->m[2]);
    break;
  case TASK_ID_M4:
    compute_m4(task->a, task->b, &task->m[3]);
    break;
  case TASK_ID_M5:
    compute_m5(task->a, task->b, &task->m[4]);
    break;
  case TASK_ID_M6:
    compute_m6(task->a, task->b, &task->m[5]);
    break;
  case TASK_ID_M7:
    compute_m7(task->a, task->b, &task->m[6]);
    break;
  case TASK_ID_C11:
    compute_c11(task->m, &task->c[_11]);
    break;
  case TASK_ID_C12:
    compute_c12(task->m, &task->c[_12]);
    break;
  case TASK_ID_C21:
    compute_c21(task->m, &task->c[_21]);
    break;
  case TASK_ID_C22:
    compute_c22(task->m, &task->c[_22]);
    break;
  default:
    break;
  }

  // Update thread count
  pthread_mutex_lock(&thread_count_mutex);
  int32_t threads_avail = MAX_THREADS - thread_count--;
  pthread_mutex_unlock(&thread_count_mutex);

  //printf("%4hu Finished task %p (ID %hhu)... %d / %d threads available \n",
  //       task->size, task, task->id, threads_avail, MAX_THREADS);
  free(task);

  return NULL;
}

void assign_task(u_int8_t task_id, Task *task_info, pthread_t *thread) {

  // Create task struct, to be freed by the thread that uses it
  Task *task = malloc(sizeof(Task));
  task->size = task_info->size;
  task->id = task_id;
  task->a = task_info->a;
  task->b = task_info->b;
  task->c = task_info->c;
  task->m = task_info->m;

  // Determine whether to create a new thread for this task
  int32_t use_new_thread;
  if (task->size < strassen_threshold << parallel_recursion) {
    // Make sure there are available threads
    pthread_mutex_lock(&thread_count_mutex);
    use_new_thread = (thread_count < MAX_THREADS) ? 1 : 0;
    thread_count += use_new_thread;
    pthread_mutex_unlock(&thread_count_mutex);
  } else {
    // Always compute in series at higher layers
    use_new_thread = 0;
  }

  // Perform the task
  if (use_new_thread) {
    pthread_create(thread, &thread_attributes, perform_task, (void *) task);
  } else {
    *thread = 0;
    pthread_mutex_lock(&thread_count_mutex);
    thread_count++;
    pthread_mutex_unlock(&thread_count_mutex);
    perform_task((void *) task);
  }
}

int32_t *element(Matrix *mat, u_int16_t i, u_int16_t j) {

  //enter_func(FUNC_ID_ELEMENT);

  if (i < 0 || i >= mat->rows) {
    //leave_func(FUNC_ID_ELEMENT);
    return NULL;
  } else if (j < 0 || j >= mat->cols) {
    //leave_func(FUNC_ID_ELEMENT);
    return NULL;
  } else {
    //leave_func(FUNC_ID_ELEMENT);
    return (int32_t *) (mat->data + i * mat->cols + j);
  }
}

void decompose(Matrix *mat, Matrix subs[2][2]) {

  //enter_func(FUNC_ID_DECOMPOSE);

  // Assume the matrix is square
  
  // Initialize the new matrices
  u_int16_t size = mat->rows / 2;
  for (u_int8_t i = 0; i < 2; i++) {
    for (u_int8_t j = 0; j < 2; j++) {
      subs[i][j].rows = size;
      subs[i][j].cols = size;
      subs[i][j].data = (int32_t *) calloc(size * size, sizeof(int32_t));
    }
  }

  // Fill the new matrices
  for (u_int16_t i = 0; i < mat->rows; i++) {
    for (u_int16_t j = 0; j < mat->cols; j++) {
      *element(&subs[i / size][j / size], i % size, j % size) = *element(mat, i, j);
    }
  }

  //leave_func(FUNC_ID_DECOMPOSE);
}

void compose(Matrix subs[2][2], Matrix *mat) {

  //enter_func(FUNC_ID_COMPOSE);
  
  // Assume the matrix is square
  
  // Initialize the new matrix
  u_int16_t size = subs[0][0].rows;
  mat->rows = size * 2;
  mat->cols = size * 2;
  mat->data = (int32_t *) calloc(mat->rows * mat->cols, sizeof(int32_t));

  // Fill the new matrix
  for (u_int16_t i = 0; i < mat->rows; i++) {
    for (u_int16_t j = 0; j < mat->rows; j++) {
      *element(mat, i, j) = *element(&subs[i / size][j / size], i % size, j % size);
    }
  }

  //leave_func(FUNC_ID_COMPOSE);
}

void compute_m1(Matrix *sub1, Matrix *sub2, Matrix *m1) {

  Matrix temp1;
  Matrix temp2;

  add(&sub1[_11], &sub1[_22], &temp1);
  add(&sub2[_11], &sub2[_22], &temp2);
  multiply(&temp1, &temp2, m1);

  free(temp1.data);
  free(temp2.data);
}

void compute_m2(Matrix *sub1, Matrix *sub2, Matrix *m2) {

  Matrix temp;

  add(&sub1[_21], &sub1[_22], &temp);
  multiply(&temp, &sub2[_11], m2);

  free(temp.data);
}

void compute_m3(Matrix *sub1, Matrix *sub2, Matrix *m3) {

  Matrix temp;

  subtract(&sub2[_12], &sub2[_22], &temp);
  multiply(&sub1[_11], &temp, m3);

  free(temp.data);
}

void compute_m4(Matrix *sub1, Matrix *sub2, Matrix *m4) {

  Matrix temp;

  subtract(&sub2[_21], &sub2[_11], &temp);
  multiply(&sub1[_22], &temp, m4);

  free(temp.data);
}

void compute_m5(Matrix *sub1, Matrix *sub2, Matrix *m5) {

  Matrix temp;

  add(&sub1[_11], &sub1[_12], &temp);
  multiply(&temp, &sub2[_22], m5);

  free(temp.data);
}

void compute_m6(Matrix *sub1, Matrix *sub2, Matrix *m6) {

  Matrix temp1;
  Matrix temp2;

  subtract(&sub1[_21], &sub1[_11], &temp1);
  add(&sub2[_11], &sub2[_12], &temp2);
  multiply(&temp1, &temp2, m6);

  free(temp1.data);
  free(temp2.data);
}

void compute_m7(Matrix *sub1, Matrix *sub2, Matrix *m7) {

  Matrix temp1;
  Matrix temp2;

  subtract(&sub1[_12], &sub1[_22], &temp1);
  add(&sub2[_21], &sub2[_22], &temp2);
  multiply(&temp1, &temp2, m7);

  free(temp1.data);
  free(temp2.data);
}

void compute_c11(Matrix *m, Matrix *c11) {

  Matrix temp1;
  Matrix temp2;

  subtract(&m[0], &m[4], &temp1);
  add(&m[3], &m[6], &temp2);
  add(&temp1, &temp2, c11);

  free(temp1.data);
  free(temp2.data);
}

void compute_c12(Matrix *m, Matrix *c12) {

  add(&m[2], &m[4], c12);
}

void compute_c21(Matrix *m, Matrix *c21) {

  add(&m[1], &m[3], c21);
}

void compute_c22(Matrix *m, Matrix *c22) {

  Matrix temp1;
  Matrix temp2;

  subtract(&m[0], &m[1], &temp1);
  add(&m[2], &m[5], &temp2);
  add(&temp1, &temp2, c22);

  free(temp1.data);
  free(temp2.data);
}

Matrix *add(Matrix *mat1, Matrix *mat2, Matrix *sum) {

  //enter_func(FUNC_ID_ADD);
  
  // Check that both matrices have the same dimensions
  if (mat1->rows != mat2->rows || mat1->cols != mat2->cols) {
    return NULL;
  }

  // Initialize the new matrix
  sum->rows = mat1->rows;
  sum->cols = mat2->cols;
  sum->data = (int32_t *) calloc(sum->rows * sum->cols, sizeof(int32_t));
  
  // Compute the sum
  for (u_int16_t i = 0; i < sum->rows; i++) {
    for (u_int16_t j = 0; j < sum->cols; j++) {
      *element(sum, i, j) = *element(mat1, i, j) + *element(mat2, i, j);
    }
  }

  //leave_func(FUNC_ID_ADD);
  
  return sum;
}

Matrix *subtract(Matrix *mat1, Matrix *mat2, Matrix *diff) {

  //enter_func(FUNC_ID_SUBTRACT);
  
  // Check that both matrices have the same dimensions
  if (mat1->rows != mat2->rows || mat1->cols != mat2->cols) {
    return NULL;
  }

  // Initialize the new matrix
  diff->rows = mat1->rows;
  diff->cols = mat2->cols;
  diff->data = (int32_t *) calloc(diff->rows * diff->cols, sizeof(int32_t));

  // Compute the sum
  for (u_int16_t i = 0; i < diff->rows; i++) {
    for (u_int16_t j = 0; j < diff->cols; j++) {
      *element(diff, i, j) = *element(mat1, i, j) - *element(mat2, i, j);
    }
  }

  //leave_func(FUNC_ID_SUBTRACT);

  return diff;
}

Matrix *multiply(Matrix *mat1, Matrix *mat2, Matrix *prod) {

  //enter_func(FUNC_ID_MULTIPLY);
  
  // Check that the matrices have the same inner dimension
  if (mat1->cols != mat2->rows) {
    return NULL;
  }

  // Initialize the new matrix
  prod->rows = mat1->rows;
  prod->cols = mat2->cols;

  // Perform the appropriate multiplication algorithm
  if (mat1->cols < strassen_threshold) {
    naive_multiply(mat1, mat2, prod);
  } else {
     strassen_multiply(mat1, mat2, prod);
  }

  //leave_func(FUNC_ID_MULTIPLY);
  
  return prod;
}

Matrix *naive_multiply(Matrix *mat1, Matrix *mat2, Matrix *prod) {

  //enter_func(FUNC_ID_NAIVE_MULTIPLY);
  
  // Assume the matrices have the same inner dimension

  // Build the matrix
  prod->data = (int32_t *) calloc(prod->rows * prod->cols, sizeof(int32_t));
  
  // Compute the product
  for (u_int16_t i = 0; i < prod->rows; i++) {
    for (u_int16_t j = 0; j < prod->cols; j++) {
      for (u_int16_t k = 0; k < mat1->cols; k++) {
	*element(prod, i, j) += *element(mat1, i, k) * *element(mat2, k, j);
      }
    }
  }

  //leave_func(FUNC_ID_NAIVE_MULTIPLY);
  
  return prod;
}

Matrix *strassen_multiply(Matrix *mat1, Matrix *mat2, Matrix *prod) {

  
  // Assume the matrices have the same inner dimension

  // Check that the matrices have even dimensions
  if (mat1->rows & 1 || mat1->cols & 1 || mat2->rows & 1 || mat2->cols & 1) {
    return NULL; // TODO: fix this rather than quitting
  }

  // Check that the matrices are square
  if (mat1->rows != mat1->cols || mat2->rows != mat2->cols) {
    return NULL; // TODO: fix this rather than quitting
  }

  // Split the matrices
  Matrix sub1[2][2];
  Matrix sub2[2][2];
  Matrix sub3[2][2];
  Matrix temp[7];
  decompose(mat1, sub1);
  decompose(mat2, sub2);

  // Generic info to be passed to ALL threads
  Task task_info;
  task_info.size = mat1->rows;
  task_info.a = (Matrix *) sub1;
  task_info.b = (Matrix *) sub2;
  task_info.c = (Matrix *) sub3;
  task_info.m = (Matrix *) temp;

  /* DANGER: passing the child threads pointers to the parent thread's stack!
     Arguably OK because the threads will only ever read from these pointers,
     never write to them, and because the function waits for all the child
     threads to complete before returning. */

  // Compute temporary matrices (M1, ..., M7)

  // Assign six computations to the child threads
  pthread_t threads[6] = {0};
  for (u_int8_t i = 0; i < 6; i++) {
    assign_task(TASK_ID_M2 + i, &task_info, &threads[i]);
  }
  
  // Save the seventh computation for the parent thread
  //printf("%4hu Starting task %p (ID 0)... (local)\n", task_info.size, &task_info);
  compute_m1((Matrix *) sub1, (Matrix *) sub2, &temp[0]);
  //printf("%4hu Finished task %p (ID 0)... (local)\n", task_info.size, &task_info);

  // Wait for all threads to finish before continuing
  
  for (u_int8_t i = 0; i < 6; i++) {
    if (threads[i] != 0) {
      pthread_join(threads[i], NULL);
    }
  }
  
  // Compute submatrices (C11, C12, C21, C22) and create product

  // Assign three computations to the child threads
  for (u_int8_t i = 0; i < 3; i++) {
    assign_task(TASK_ID_C11 + i, &task_info, &threads[i]);
  }

  // Save the fourth computation for the parent thread
  compute_c22(temp, &sub3[1][1]);

  // Wait for all threads to finish before continuing
  for (u_int8_t i = 0; i < 3; i++) {
    if (threads[i] != 0) {
      pthread_join(threads[i], NULL);
    }
  }

  compose(sub3, prod);

  // Clean up
  for (u_int8_t i = 0; i < 2; i++) {
    for (u_int8_t j = 0; j < 2; j++) {
      free(sub1[i][j].data);
      free(sub2[i][j].data);
      free(sub3[i][j].data);
    }
  }

  for (u_int8_t i = 0; i < 7; i++) {
    free(temp[i].data);
  }
  
  return prod;
}

Matrix *input_matrix() {

  Matrix *mat = malloc(sizeof(Matrix));
  
  printf("# rows: ");
  scanf("%hu", &mat->rows);
  printf("# cols: ");
  scanf("%hu", &mat->cols);
  
  mat->data = (int32_t *) calloc(mat->rows * mat->cols, sizeof(int32_t));
  printf("Enter matrix:\n");
  for (u_int16_t i = 0; i < mat->rows; i++) {
    for (u_int16_t j = 0; j < mat->cols; j++) {
      scanf("%d", element(mat, i, j));
    }
  }
  
  return mat;
}

void print_matrix(Matrix *mat) {

  for (u_int16_t i = 0; i < mat->rows; i++) {
    for (u_int16_t j = 0; j < mat->cols; j++) {
      printf("%4d", *element(mat, i, j));
    }
    printf("\n");
  }
}

void destroy_matrix(Matrix *mat) {

  free(mat->data);
  free(mat);
}

int main(int argc, char **argv) {

  create_pthread_tools();
  
  printf("Matrix A:\n");
  Matrix *mat1 = input_matrix();

  printf("Matrix B:\n");
  Matrix *mat2 = input_matrix();

  printf("Product AB:\n");
  Matrix *prod = (Matrix *) malloc(sizeof(Matrix));
  multiply(mat1, mat2, prod);

  print_matrix(prod);

  destroy_pthread_tools();

  destroy_matrix(mat1);
  destroy_matrix(mat2);
  destroy_matrix(prod);

  return 0;
}



