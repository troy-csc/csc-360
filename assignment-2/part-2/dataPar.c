#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#include "dataPar.h"

#define NUM_THREADS 2

#define NUM_PTS 18
#define NUM_PTS_2 365
#define NUM_PTS_3 3652

#define NUM_LINES (NUM_PTS * (NUM_PTS - 1))/2
#define NUM_LINES_2 (NUM_PTS_2 * (NUM_PTS_2 - 1))/2
#define NUM_LINES_3 (NUM_PTS_3 * (NUM_PTS_3 - 1))/2

// Struct for saving results of min sar by both threads
typedef struct min_s {
    int index;
    float sar;
} min_t;

// Global variables for checking time elapsed during the algorithm
clock_t s, f;
double time_elapsed;

// Arrays for storing data and lines
float p1_data[NUM_PTS_2];
float p2_data[NUM_PTS_3];

line_t p1_lines[NUM_LINES_2];
line_t p2_lines[NUM_LINES_3];

float p1_sar[NUM_LINES_2];
float p2_sar[NUM_LINES_3];

// Thread array
pthread_t threads[NUM_THREADS];

int num_lines = 0;
int num_threads = 1;
min_t thread_output[2];

int main(int argc, char* argv[]) {
    /*
     * First Task: Calculate min Sum of Absolute Residuals for                                                            *             4 cases: 6, 10, 14, 18 data points given (BRUTE FORCE APPROACH)                                        * data in test1.txt
     *
     */

    printf("TASK 1: Calculate L1 line for 6, 10, 14 and 18 data points given\n");

    // Open file if it exists
    FILE *file = fopen("data1.txt", "r");
    if(file == NULL) {
        fprintf(stderr, "Unable to open file\n");
        exit(0);
    }

    // start clock
    s = clock();

    int num_pts = 0;

    float data[18]; // data array for first task

    // read data (print for debugging)
    while(fscanf(file, "%f", &data[num_pts]) != EOF) {
        //printf("%.1f\n", data[num_pts]);
        num_pts++;
    }

    line_t set_of_lines[(num_pts * (num_pts - 1))/2];
    float sum_residuals[(num_pts * (num_pts - 1))/2];

    calc(6, data, set_of_lines, sum_residuals);
    calc(10, data, set_of_lines, sum_residuals);
    calc(14, data, set_of_lines, sum_residuals);
    calc(18, data, set_of_lines, sum_residuals);

    // end clock
    f = clock();
    time_elapsed = ((double)(f-s))/CLOCKS_PER_SEC;
    printf("Time taken: %f\n\n", time_elapsed);

    fclose(file);

    /*
     * Second Task: Calculate L1 line for year 2002 data of fisheries
     *
     */

    printf("TASK 2: Calculate L1 line for year 2002 data given\n");

    // Open file if it exists else exit
    FILE *file2 = fopen("data2.txt", "r");
    if(file2 == NULL) {
        fprintf(stderr, "Unable to open file\n");
        exit(0);
    }

    s = clock();

    num_pts = 0;
	
	// read data (print for debugging)
    while(fscanf(file2, "%f", &p1_data[num_pts]) != EOF) {
        //printf("%.1f\n", p1_data[num_pts]);
        num_pts++;
    }

    calc(num_pts, p1_data, p1_lines, p1_sar);

    f = clock();
    time_elapsed = ((double)(f-s))/CLOCKS_PER_SEC;
    printf("Time taken: %f\n\n", time_elapsed);

    fclose(file);

    /*
     * TASK 2 PART 2: Calculate L1 line and min SAR for the full
     * 10 years of data (data3.txt)
     *
     */

    printf("FULL 10 YR DATA COMPUTATION\n");

    // Open file if it exists else exit
    FILE *file3 = fopen("data3.txt", "r");
    if(file3 == NULL) {
        fprintf(stderr, "Unable to open file\n");
        exit(0);
    }

    s = clock();

    num_pts = 0;

    // read data (print for debugging)
    while(fscanf(file3, "%f", &p2_data[num_pts]) != EOF) {
        //printf("%.1f\n", p2_data[num_pts]);
        num_pts++;
    }

    printf("%d points\n", num_pts);

    // calculate lines
    calc_set_lines(num_pts, p2_data, p2_lines, p2_sar);

    // create threads
    int i;
    for(i=0; i<NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, p_calculate, &num_pts);
    }

    // wait for threads to finish
    for(i=0; i<NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // check minimum's returned by threads for absolute min
    if(thread_output[0].sar < thread_output[1].sar) {
        printf("Min Sum of Absolute Residuals: %f\n", thread_output[0].sar);
        printf("Line of min sum of absolute residuals:\nSlope: %f Intercept: %f\n", p2_lines[thread_output[0].index].slope, p2_lines[thread_output[0].index].intercept);
    }
	else {
        printf("Min Sum of Absolute Residuals: %f\n", thread_output[1].sar);
        printf("Line of min sum of absolute residuals:\nSlope: %f Intercept: %f\n", p2_lines[thread_output[1].index].slope, p2_lines[thread_output[1].index].intercept);
    }

    f = clock();
    time_elapsed = ((double)(f-s))/CLOCKS_PER_SEC;
    printf("Time taken: %f\n\n", time_elapsed);

    fclose(file);

    return 0;
}

void calc(int n, float data[], line_t set_of_lines[], float sum_residuals[]) {
    printf("%d points\n", n);
    calc_set_lines(n, data, set_of_lines, sum_residuals);
    calc_sum_res(n, data, set_of_lines, sum_residuals);
    min_sum_res(data, set_of_lines, sum_residuals);
}

/*
 * Calculate the set of all possible lines with the given data points
 *
 */
void calc_set_lines(int num_pts, float data[], line_t set_of_lines[], float sum_residuals[]) {
    for(int i=0; i<num_pts; i++) {
        for(int j=i; j<num_pts; j++) {
            if(i == j) {
                continue;
            }

            line_t line;
            line.slope = (data[j]-data[i])/((j+1)-(i+1));
            line.intercept = data[j] - (line.slope * (j+1));
            set_of_lines[num_lines++] = line; // Add line to set of lines array
            //printf("Slope: %f Intercept: %f\n", line.slope, line.intercept);
        }
    }
}

void calc_sum_res(int num_pts, float data[], line_t set_of_lines[], float sum_residuals[]) {
    for(int i=0; i<num_lines; i++) { // for all lines
        sum_residuals[i] = 0; // Initilize sum to 0
        for(int j=0; j<num_pts; j++) { // for all points
            // absolute val of (slope*x + intercept) - y
            sum_residuals[i] += fabs(((set_of_lines[i].slope * (j+1)) + set_of_lines[i].intercept) - data[j]);
        }
        //printf("SAR: %f\n", sum_residuals[i]);
    }
}

void min_sum_res(float data[], line_t set_of_lines[], float sum_residuals[]) {
    float min = sum_residuals[0];
    int min_sar_index;

    for(int i=0; i<num_lines; i++) {
        if(sum_residuals[i] < min) {
            min = sum_residuals[i];                                                                                              min_sar_index = i;
        }
    }

    printf("Min Sum of Absolute Residuals: %f\n", min);
    printf("Line of min sum of absolute residuals:\nSlope: %f Intercept: %f\n", set_of_lines[min_sar_index].slope, set_of_lines[min_sar_index].intercept);

    num_lines = 0;
}

void *p_calculate(void *num) {
    int *n = num;
    int tnum = num_threads++;

    p_calc_split(*n, tnum);

    pthread_exit(NULL);
}

void p_calc_split(int n, int tnum) {
    int i, num_i, s = 0;
    if(tnum == 1) {
        num_i = num_lines/2;
    }
    else {
        s = num_lines - (num_lines/2);
        num_i = num_lines;
    }

    for(i=s; i<num_i; i++) { //for all lines
        p2_sar[i] = 0; // Initilize sum to 0
        for(int j=0; j<n; j++) { // for all points
            // absolute val of (slope*x + intercept) - y
            p2_sar[i] += fabs(((p2_lines[i].slope * (j+1)) + p2_lines[i].intercept) - p2_data[j]);
        }
        //printf("SAR: %f\n", sum_residuals[i]);
    }

    float min = p2_sar[0];
    int min_sar_index;
    for(i=s; i<num_i; i++) {
        if(p2_sar[i] < min) {
            min = p2_sar[i];
            min_sar_index = i;
        }
    }

    thread_output[tnum-1].index = min_sar_index;
    thread_output[tnum-1].sar = min;
}