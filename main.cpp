#include <math.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include "libmfcc/mfcc_block.h"



int main(){


    // Generate a 440 herz sine wave
    int SAMPLE_RATE = 16000;
    int SAMPLE_COUNT = 32000;
    std::vector<float> samples(SAMPLE_COUNT);
    float freq = 440;
    float angle_speed = 2 * M_PI * (freq / SAMPLE_RATE);
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        samples[i] = sin(angle_speed * i);
    }

    int number_feature_vectors = 61; // 注意： 子函数中重新定义number_feature_vectors的数值
    int number_coefficients = 12;
    double **feature_vector= new double *[number_feature_vectors] ;
    feature_vector[0] = new double[number_feature_vectors * number_coefficients * 3];
    mfcc_block( samples,  SAMPLE_COUNT,  feature_vector);

    // generate data.txt
    FILE *file = fopen("../MFCC.txt", "wt");
    for (int i = 0; i < number_feature_vectors; i++){
        for (int j = 0; j < 3 * number_coefficients; j++){
            fprintf(file, "%lf ", feature_vector[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);

    for (int i = 0; i < number_feature_vectors; i++){
        delete[] feature_vector[i];
    }
    delete[] feature_vector;

	return 0;
}
