#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

typedef struct sortData {
    double *a;
    int low;
    int high;
    int numThreads;
} sortingParameters;

void swap(double * p1, double * p2);
int partition(double  *a, int low, int high, int pivot);
void quicksort(double  *a, int low, int high);
void thread_quicksort(double  *a, int low, int high, int numThreads);
void printArray(double  *a, int size);
int medianOfThree(double *a, int low, int high);

// Time function
static double get_wall_seconds() 
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
  return seconds;
}

void* helpSort(void* arg) 
{
    sortingParameters* parameters = arg;
    thread_quicksort(parameters->a, parameters->low, parameters->high, parameters->numThreads);
    return NULL;
}

void thread_quicksort(double  *a, int low, int high, int numThreads) 
{
    if (low < high) {
        int pivotPos = medianOfThree(a,low,high);
        //int pivotPos = low;
        int pivot = partition(a, low, high, pivotPos);
        if (numThreads > 0) {
            pthread_t thread;
            sortingParameters threadParameters = {a, low, pivot-1, numThreads-2};
            pthread_create(&thread, NULL, helpSort, &threadParameters);
            thread_quicksort(a, pivot+1, high, numThreads-2); // recursive call (for same thread)

            pthread_join(thread, NULL);
        }
        else {
        quicksort(a,low,pivot-1);
        quicksort(a,pivot+1,high);
        }
    }
} 

int main(int argc, char **argv)
{ 
    int numThreads= 0;
    if (argc > 1){
        numThreads = strtol(argv[1], NULL, 10);
        printf("Number of threads: %d\n", numThreads);
    }

    int N = 1e8;
    if (argc > 2){
        N = strtol(argv[2], NULL, 10); 
        printf("Number of array elements: %d", N);  
    }

    srand ( time(NULL) ); // initializing the random function
    double  *a = (double *) malloc(N * sizeof(double ));
    double  *acopy = (double *) malloc(N * sizeof(double ));
    for (int i=0; i<N; i++) {
        acopy[i] = drand48(); // random numbers 
    } 

    /*
    double  *a = (double *) malloc(N * sizeof(double ));
    double *acopy = (double*) malloc(N * sizeof(double));
    for (int i=0; i<N; i++) {
        acopy[i] = i;
    } // already sorted list, increasing order
    */

    // Copy array 
    memcpy(a, acopy, N*sizeof(double ));

    //printf("\nUnsorted array: ");
    //printArray(a,N);
    
    double time1 = get_wall_seconds();
    //quicksort(a,0,N-1);
    double time2 = get_wall_seconds(); 
    
    memcpy(a, acopy, N*sizeof(double ));
    double time1p = get_wall_seconds();
    thread_quicksort(a, 0, N-1, numThreads);
    double time2p = get_wall_seconds(); 

    //printf("\nSorted array: ");
    //printArray(a,N);

    printf("\n");
    printf("Execution time for sequential: %f\n", time2-time1);
    printf("Execution time for parallel: %f\n", time2p-time1p);

    free(a);
    free(acopy);
    pthread_exit(NULL);
}

// Sequential quicksort
void quicksort(double  *a, int low, int high)
{
    /*// Non-optimized version
    if (low < high) {
        //int pivot = low + (high-low)/2; // median as pivot
        int pivot = low; // first as pivot
        pivot = partition(a,low,high,pivot);
        quicksort(a,low,pivot-1);
        quicksort(a,pivot+1,high);
    }*/

    // Optimized version
    while (low < high) {
        //int pivotPos = low;
        int pivotPos = medianOfThree(a,low,high);
        int pivot = partition(a,low,high,pivotPos);
        if (pivot-low < high-pivot) { // left part, tail recursion insertion sort
            quicksort(a,low,pivot-1);
            low = pivot + 1;
        }
        else { // right part 
            quicksort(a,pivot+1,high);
            high = pivot - 1;
        }
    }
}
    
void swap(double * p1, double * p2)
{
    double  tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}

int medianOfThree(double *a, int low, int high) 
{
    int mid = (low+high)/2;
    if ( a[high] < a[low]) {
        swap(&a[high], &a[low]);
    }
    if (a[mid] < a[low]) {
        swap(&a[mid], &a[low]);
    }
    if (a[high] < a[mid]) {
        swap (&a[high], &a[mid]);
    }
    return mid;
}

int partition(double  *a, int low, int high, int pivot) 
{
    double pivotVal = a[pivot];
    swap(&a[pivot],&a[high]);
    int l = low;

    for (int j=low; j<high; j++) {
        if (a[j]<=pivotVal) {
            swap(&a[j],&a[l]);
            l++;
        }
    }
    swap(&a[l],&a[high]);
    return l;
}

void printArray(double  *a, int size) 
{
    for (int i=0; i<size; i++) {
        printf("%f ", a[i]);
    }
}