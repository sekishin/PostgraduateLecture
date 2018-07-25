#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define SIZE 256	// 正方行列の最大サイズ
#define THNUM 64	// 生成するスレッドの最大数
#define LOOP 1000	// 時間計測で繰り返す回数

int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];	// 行列

typedef struct {
/* Cのどの要素を計算するかを格納するための構造体 */
	int row;	// Cの行
	int column;	// Cの列
	int size;	// 行列の大きさ
} Part;

typedef struct {
	Part p[SIZE*SIZE];
	int size;
} Parts;

void calc_matrix(Parts *ps)
{
	int k, l;	// 反復変数
	Part tmp;
	for (l = 0; l < ps->size; l++) {
		tmp = ps->p[l];
#ifdef DEBUG
		printf("row: %d, column; %d, size: %d\n", tmp.row, tmp.column, tmp.size);
#endif
		int sum = 0;
		for (k = 0; k < tmp.size; k++) {
			sum += A[tmp.row][k] * B[k][tmp.column];
		}
		C[tmp.row][tmp.column] = sum;
	}
	pthread_exit(0);
}

void show_matrix(int n)
{
	int i, j;
	puts("matrix A");
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			printf("%2d ", A[i][j]);
		}
		puts("");
	}
	puts("matrix B");
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			printf("%2d ", B[i][j]);
		}
		puts("");
	}
}

void error(char *msg)
{
	puts(msg);
	exit(1);
}
 
int main(int argc, char **argv)
{
	int n;	// 行列サイズ
	int thnum;	// スレッド数
	pthread_t threads[THNUM];
	int status;
	int i, j;
	Parts ps[THNUM];
	int num;
	int t;
	struct timeval start, end;
	time_t s, e;

	/* 引数処理 */
	if (argc < 3) error("Usage: execute_file_name matrix_size thread_size");
	n = atoi(argv[1]);
	thnum = atoi(argv[2]);
	if (n <= 0 || SIZE < n) error("illegal matrix size");
	if (thnum <= 0 || THNUM < thnum) error("illegal thread size");

	/* 初期化 */
	num = 0;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			A[i][j] = rand() % 100;
			B[i][j] = rand() % 100;
			t = num % thnum;
			if (num == t) ps[num].size = 0;
			Part tmp = {i, j, n};
			ps[t].p[ps[t].size] = tmp;
			ps[t].size++;
			num++;
		}
	}
#ifdef DEBUG
	show_matrix(n);
#endif
	
	/* スレッド生成 */
	gettimeofday(&start, NULL);
	time(&s);
	for (t = 0; t < LOOP; t++) {
		for (num = 0; num < thnum; num++) {
			pthread_create(&threads[num], NULL, (void *)calc_matrix, &ps[num]);
		}
		for (num = 0; num < thnum; num++) {
			pthread_join(threads[num], (void **)&status);
#ifdef DEBUG
			printf("finish thread %d with return value %d\n", num, status);
#endif
		}
	}
	time(&e);
	gettimeofday(&end, NULL);
#ifdef DEBUG
	puts("matrix C");
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			printf("%6d ", C[i][j]);
		}
		puts("");
	}
#endif
	double time = (double)(end.tv_sec*1000000+end.tv_usec) - (start.tv_sec*1000000+start.tv_usec);
	time /= LOOP;
	printf("実時間(ms): %.3f\n", (double)(e-s)/LOOP);
	printf("処理時間(ms): %.4f\n", time/1000);
	return 0;
}
