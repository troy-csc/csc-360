#ifndef DATAPAR_H
#define DATAPAR_H

#include <stddef.h>

typedef struct line {
    float slope;
    float intercept;
} line_t;

void calc(int n, float data[], line_t set_of_lines[], float sum_residuals[]);

void calc_set_lines(int num_pts, float data[], line_t set_of_lines[], float sum_residuals[]);

void calc_sum_res(int num_pts, float data[], line_t set_of_lines[], float sum_residuals[]);

void min_sum_res(float data[], line_t set_of_lines[], float sum_residuals[]);

void *p_calculate(void *num);

void p_calc_split(int n, int tnum);

#endif