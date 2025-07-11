#ifndef Yin_Shasta_H
#define Yin_Shasta_H

// Yin pitch detection structure
typedef struct {
    float sampleRate;        // Sampling rate (Hz)
    int bufferSize;          // Total buffer size
    int halfBuffer;          // Half of buffer size
    float thresVal;          // Threshold value for pitch detection
    float pitchConfidence;   // Confidence of detected pitch
    float *yinDiffBuffer;    // Buffer for difference function
} Yin;

// Function declarations
Yin* Yin_init(float sampleRate, int bufferSize);
float Yin_getPitchConfidence(Yin* yin);
float Yin_detectPitch(Yin* yin, float* audioBuffer);
void Yin_DF(Yin* yin, float* audioBuffer);
void Yin_CMND(Yin* yin);
int Yin_findThresholdCrossing(Yin* yin);
float Yin_refineTauEstimate(Yin* yin, int tauEstimate);

#endif 