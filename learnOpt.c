/*
 * learn.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include "MT.h"
#include "learn.h"
#include "feature.h"

#define min(x, y)   ((x)>(y) ? (y) : (x))

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

void nmf_learn(double **data, int n_rows, int n_cols, int n_class, double **W, double **H, int maxiter, int blk_size){
    double **X_hat;
    int i,j,k,ii,jj;
    double flops = 0;
    double gflops = 0;

    // get a start time for gflop calculation
    double start_time = get_time();

    X_hat = (double **)calloc(n_rows,sizeof(double *));
    for(i = 0;i < n_rows;i++){
        X_hat[i] = (double *)calloc(n_cols,sizeof(double));
    }

    double start_time_for_init = get_time();
    // initialize W, H
    init_genrand(2468);
    // W(n_rows, n_class)
    for(j = 0;j < n_class;j++){
      for(i = 0;i < n_rows;i++){
            W[i][j] = genrand_real3();
            flops++;
        }
    }
    // H(n_class, n_cols)
    for(j = 0;j < n_cols;j++){
      for(i = 0;i < n_class;i++){
            H[i][j] = genrand_real3();
            flops++;
        }
    }
    double end_time_for_init = get_time();
    printf("init: %f\n", end_time_for_init - start_time_for_init);

    // Add timing around X_hat calculation
    double start_time_for_X_hat = get_time();
    // X_hat = W x H
    for(j = 0; j < n_cols; j++){
      // for (ii = 0; ii < n_rows; ii+=blk_size) {
      //   for(i = ii; i < min(ii+blk_size, n_rows); i++){
      for(i = 0;i < n_rows;i++){
              X_hat[i][j] = 0.0;
              for(k = 0; k < n_class; k++){
                  X_hat[i][j] += W[i][k] * H[k][j];
                  flops+=2; // 2 flops - 1 multiply and 1 add
              }
          }
      // }
    }
    double end_time_for_X_hat = get_time();
    printf("X_hat: %f\n", end_time_for_X_hat - start_time_for_X_hat);


    FILE *ofp;
    if((ofp = fopen("ISD.txt","w")) == NULL){
        fprintf(stderr,"nmf_learn:: cannnot open output file.\n");
        exit(1);
    }
    fprintf(ofp,"STEP\tISD\n");


    // iteration
    int it;
    double numerator;
    double denominator;
    double isd;
    double prev_isd;
    double converge_threshold = 1.0e-12;
    double epsilon = 1.0e-12;
    double time_for_ISD_accum = 0.0;
    double time_for_W_accum = 0.0;
    double time_for_H_accum = 0.0;
    double time_for_X_hat_accum = 0.0;
    for(it = 0;it < maxiter;it++){
        // add timing around IS divergence calculation for each iteration and accumulate total time at the end of iterations
        double start_time_for_ISD = get_time();
        // compute IS divergence
        isd = 0.0;
        for(j = 0;j < n_cols;j++){
          // for (ii = 0; ii < n_rows; ii+=blk_size) {
          //   for(i = ii; i < min(ii+blk_size, n_rows); i++){
              for(i = 0;i < n_rows;i++){
                  isd += ((data[i][j]+epsilon) / (X_hat[i][j]+epsilon)) - log((data[i][j]+epsilon) / (X_hat[i][j]+epsilon)) - 1.0;
                  flops+=4; // 4 flops - 2 divides, 1 log, and 1 subtract
              }
          // }
        }
       // only ouput this value every 100 iterations
        if(it % 500 == 0){
            fprintf(stdout,"\nIS Divergence = %.8f\n",isd);
            // printf("iteration %2d / %3d..\n",it+1,maxiter);
            fflush(stdout);
        }
        if(it == maxiter - 1) {
            fprintf(stdout,"\nIS Divergence = %.8f\n",isd);
            fflush(stdout);
        }
        fprintf(ofp,"%d\t%.8f\n",it,isd);
        if((it != 0) && ((prev_isd*1.05) < isd)) break;
        if((it != 0) && (fabs(prev_isd - isd) < converge_threshold)){
            printf("converged.\n");
            break;
        }
        prev_isd = isd;

        // update rules for minimizing IS divergence
        // update W
        double start_time_for_W = get_time();
        // for (ii = 0; ii < n_rows; ii+=blk_size) {
        //   for(i = ii; i < min(ii+blk_size, n_rows); i++){
            for(i = 0;i < n_rows;i++){
              for(k = 0;k < n_class;k++){
                  if(W[i][k] != 0.0){
                    // for (jj = 0; jj < n_cols; jj += blk_size) {
                        numerator = 0.0;
                        denominator = 0.0;
                        // for (j = jj; j < min(jj + blk_size, n_cols); j++) {
                          for(j = 0;j < n_class;j++){
                            numerator += ((data[i][j] + epsilon) * H[k][j]) / ((X_hat[i][j] + epsilon) * (X_hat[i][j] + epsilon));
                            denominator += H[k][j] / (X_hat[i][j] + epsilon);
                            flops += 6; // 6 flops - 2 divides, 1 multiply, 1 add, 1 subtract, and 1 divide
                        }
                        double W_ik_update = sqrt(numerator / (denominator + epsilon)); // Add epsilon to avoid division by zero
                        W[i][k] *= W_ik_update;
                        flops += 2; // 2 flops - 1 multiply and 1 divide

                        if (W[i][k] < epsilon) {
                            W[i][k] = 0.0;
                        }
                    // }
                  }
              }
          // }
        }
        double end_time_for_W = get_time();
        time_for_W_accum += end_time_for_W - start_time_for_W;

        // update X_hat
        double start_time_for_X_hat = get_time();
        for(j = 0;j < n_cols;j++){
          // for (ii = 0; ii < n_rows; ii+=blk_size) {
          //   for(i = ii; i < min(ii+blk_size, n_rows); i++){
              for(i = 0;i < n_rows;i++){
                  X_hat[i][j] = 0.0;
                  for(k = 0;k < n_class;k++){
                      X_hat[i][j] += W[i][k] * H[k][j];
                      flops+=2; // 2 flops - 1 multiply and 1 add
                  }
              }
          // }
        }
        double end_time_for_X_hat = get_time();
        time_for_X_hat_accum += end_time_for_X_hat - start_time_for_X_hat;

        // update H
        double start_time_for_H = get_time();
            // for (jj = 0; jj < n_cols; jj += blk_size) {
            //     for (j = jj; j < min(jj + blk_size, n_cols); j++) {
                for(j = 0;j < n_class;j++){
                  for (k = 0; k < n_class; k++) {
                    if (H[k][j] != 0.0) {
                        numerator = 0.0;
                        denominator = 0.0;
                        // for (ii = 0; ii < n_rows; ii += blk_size) {
                        //     for (i = ii; i < min(ii + blk_size, n_rows); i++) {
                              for(i = 0;i < n_rows;i++){
                                numerator += ((data[i][j] + epsilon) * W[i][k]) / ((X_hat[i][j] + epsilon) * (X_hat[i][j] + epsilon));
                                denominator += W[i][k] / (X_hat[i][j] + epsilon);
                                flops += 6; // 6 flops - 2 divides, 1 multiply, 1 add, 1 subtract, and 1 divide
                            }
                        // }
                        double H_kj_update = sqrt(numerator / (denominator + epsilon)); // Add epsilon to avoid division by zero
                        H[k][j] *= H_kj_update;
                        flops += 2; // 2 flops - 1 multiply and 1 divide

                        if (H[k][j] < epsilon) {
                            H[k][j] = 0.0;
                        }
                    }
                // }
            }
        }
        double end_time_for_H = get_time();
        time_for_H_accum += end_time_for_H - start_time_for_H;

        // update X_hat
        double start_time_for_X_hat_2 = get_time();
        for(j = 0;j < n_cols;j++){
          // for (ii = 0; ii < n_rows; ii+=blk_size) {
          //   for(i = ii; i < min(ii+blk_size, n_rows); i++){
              for(i = 0;i < n_rows;i++){
                  X_hat[i][j] = 0.0;
                  for(k = 0;k < n_class;k++){
                      X_hat[i][j] += W[i][k] * H[k][j];
                      flops+=2; // 2 flops - 1 multiply and 1 add
                  }
              }
          // }
        }
        double end_time_for_X_hat_2 = get_time();
        time_for_X_hat_accum += end_time_for_X_hat_2 - start_time_for_X_hat_2;

        double end_time_for_ISD = get_time();
        double time_for_ISD = end_time_for_ISD - start_time_for_ISD;
        // accumulate time for IS divergence calculation
        time_for_ISD_accum += time_for_ISD;
    }
    // print the accumulated time for IS divergence calculation
    printf("IS divergence calculation = %f\n",time_for_ISD_accum);
    printf("W calculation = %f\n",time_for_W_accum);
    printf("X_hat calculation = %f\n",time_for_X_hat_accum);
    printf("H calculation = %f\n",time_for_H_accum);

    fclose(ofp);

    // get a stop time for gflop calculation
    double stop_time = get_time();

    // calculate the number of gflops
    gflops = flops / (stop_time - start_time) / 1.0e9;

    // ouput flops in a print statement
    printf("GFLOPS = %f \n",gflops);
}
