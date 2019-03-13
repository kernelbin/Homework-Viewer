#pragma once



void __inline CalGaussianCoeff(float sigma, float* a0, float* a1, float* a2, float* a3, float* b1, float* b2, float* cprev, float* cnext);
void __inline gaussianHorizontal(unsigned char* bufferPerLine, unsigned char* lpRowInitial, unsigned char* lpColumn, int width, int height, int Channels, int Nwidth, float a0a1, float a2a3, float b1b2, float  cprev, float cnext);
void __inline gaussianVertical(unsigned char* bufferPerLine, unsigned char* lpRowInitial, unsigned char* lpColInitial, int height, int width, int Channels, float a0a1, float a2a3, float b1b2, float  cprev, float  cnext);
void  GaussianBlurFilter(unsigned char* input, unsigned char* output, int Width, int Height, int Stride, float GaussianSigma);
void  AlphaOnlyGaussianBlurFilter(unsigned char* input, unsigned char* output, int Width, int Height, int Stride, float GaussianSigma);