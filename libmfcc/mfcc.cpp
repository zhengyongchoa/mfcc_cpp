//
// Created by 郑勇超 on 2022/9/23.
//

#include "mfcc.h"



void Discrete_Cosine_Transform(int direction, int length, double X[]){
    double pi = 3.14159265358979323846;

    double *x = new double[length];

    for (int i = 0; i < length; i++){
        x[i] = X[i];
    }
    for (int k = 0; k < length; k++){
        double sum = 0;

        if (direction == 1){
            for (int n = 0; n < length; n++){
                sum += ((k == 0) ? (sqrt(0.5)) : (1)) * x[n] * cos(pi * (n + 0.5) * k / length);
            }
        }
        else
        if (direction == -1){
            for (int n = 0; n < length; n++){
                sum += ((n == 0) ? (sqrt(0.5)) : (1)) * x[n] * cos(pi * n * (k + 0.5) / length);
            }
        }
        X[k] = sum * sqrt(2.0 / length);
    }
    delete[] x;

}
void DCT(int direction, int length, double X[]){
    if (direction == 1 || direction == -1){
        Discrete_Cosine_Transform(direction, length, X);
        return;
    }
    fprintf(stderr, "[DCT], direction = {-1 (inversed transform), 1 (forward transform)}\n");
}
void Fast_Fourier_Transform(int direction, int length, double Xr[], double Xi[]){
    int log_length = (int)log2((double)length);

    double pi = 3.14159265358979323846;

    for (int i = 0, j = 0; i < length; i++, j = 0){
        for (int k = 0; k < log_length; k++){
            j = (j << 1) | (1 & (i >> k));
        }
        if (j < i){
            double t;

            t = Xr[i];
            Xr[i] = Xr[j];
            Xr[j] = t;

            t = Xi[i];
            Xi[i] = Xi[j];
            Xi[j] = t;
        }
    }
    for (int i = 0; i < log_length; i++){
        int L = (int)pow(2.0, i);

        for (int j = 0; j < length - 1; j += 2 * L){
            for (int k = 0; k < L; k++){
                double argument = direction * -pi * k / L;

                double xr = Xr[j + k + L] * cos(argument) - Xi[j + k + L] * sin(argument);
                double xi = Xr[j + k + L] * sin(argument) + Xi[j + k + L] * cos(argument);

                Xr[j + k + L] = Xr[j + k] - xr;
                Xi[j + k + L] = Xi[j + k] - xi;
                Xr[j + k] = Xr[j + k] + xr;
                Xi[j + k] = Xi[j + k] + xi;
            }
        }
    }
    if (direction == -1){
        for (int k = 0; k < length; k++){
            Xr[k] /= length;
            Xi[k] /= length;
        }
    }
}
void FFT(int direction, int length, double Xr[], double Xi[]){
    int log_length = (int)log2((double)length);

    if (direction != 1 && direction != -1){
        fprintf(stderr, "[FFT], direction = {-1 (inversed transform), 1 (forward transform)}\n");
        return;
    }
    if (1 << log_length != length){
        fprintf(stderr, "[FFT], length must be a power of 2\n");
        return;
    }
    Fast_Fourier_Transform(direction, length, Xr, Xi);
}

double Mel_Scale(int direction, double x){
    switch (direction){
        case -1:
            return 700 * (exp(x / 1125.0) - 1);
        case 1:
            return 1125 * log(1 + x / 700.0);
    }
    fprintf(stderr, "[Mel_Scale], direction = {-1 (inversed transform), 1 (forward transform)}\n");
    return 0;
}

void MFCC(int length_frame, int length_DFT, int number_coefficients, int number_filterbanks, int sample_rate, double frame[], double feature_vector[]){
    double max_Mels_frequency = Mel_Scale(1, sample_rate / 2);
    double min_Mels_frequency = Mel_Scale(1, 300);
    double interval = (max_Mels_frequency - min_Mels_frequency) / (number_filterbanks + 1);

    double *filterbank = new double[number_filterbanks];
    double *Xr = new double[length_DFT];
    double *Xi = new double[length_DFT];

    for (int i = 0; i < number_filterbanks; i++){
        filterbank[i] = 0;
    }
    for (int i = 0; i < length_DFT; i++){
        Xr[i] = (i < length_frame) ? (frame[i]) : (0);
        Xi[i] = 0;
    }
    FFT(1, length_DFT, Xr, Xi);

    for (int i = 0; i < length_DFT / 2 + 1; i++){
        double frequency = (sample_rate / 2) * i / (length_DFT / 2);
        double Mel_frequency = Mel_Scale(1, frequency);
        double power = (Xr[i] * Xr[i] + Xi[i] * Xi[i]) / length_frame;

        for (int j = 0; j < number_filterbanks; j++){
            double frequency_boundary[] = { min_Mels_frequency + interval * (j + 0), min_Mels_frequency + interval * (j + 1), min_Mels_frequency + interval * (j + 2) };

            if (frequency_boundary[0] <= Mel_frequency && Mel_frequency <= frequency_boundary[1]){
                double lower_frequency = Mel_Scale(-1, frequency_boundary[0]);
                double upper_frequency = Mel_Scale(-1, frequency_boundary[1]);

                filterbank[j] += power * (frequency - lower_frequency) / (upper_frequency - lower_frequency);
            }
            else
            if (frequency_boundary[1] <= Mel_frequency && Mel_frequency <= frequency_boundary[2]){
                double lower_frequency = Mel_Scale(-1, frequency_boundary[1]);
                double upper_frequency = Mel_Scale(-1, frequency_boundary[2]);

                filterbank[j] += power * (upper_frequency - frequency) / (upper_frequency - lower_frequency);
            }
        }
    }

    for (int i = 0; i < number_filterbanks; i++){
        filterbank[i] = log(filterbank[i]);
    }
    DCT(1, number_filterbanks, filterbank);

    for (int i = 0; i < number_coefficients; i++){
        feature_vector[i] = filterbank[i];
    }

    delete[] filterbank;
    delete[] Xr;
    delete[] Xi;
}
