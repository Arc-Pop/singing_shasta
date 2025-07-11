#include <stdlib.h>
#include <math.h>
#include "yinShasta.h"

// Initialize Yin pitch detection
Yin* Yin_init(float sampleRate, int bufferSize) {
    Yin* yin = (Yin*) malloc(sizeof(Yin));                                   // Allocate memory for Yin structure
   yin->sampleRate = (sampleRate > 16000.0) ? 16000.0 : sampleRate;         // Set sample rate - I recommend using 5000 Hz
//    yin->sampleRate = sampleRate;         // Set sample rate - I recommend using 5000 Hz
    yin->bufferSize = bufferSize;                                            // Set buffer size - Need a buffer size of at minimum 1334, set minimum to 2000 for small freq resolution
    yin->halfBuffer = bufferSize / 2;                                        // Compute half buffer size - this is for more efficient computing
//    yin->thresVal = 0.5;                                                     // Set threshold value - can probably set higher now with better sampling rate and buffer size
    yin->thresVal = 0.25;                                                     // Set threshold value - can probably set higher now with better sampling rate and buffer size
//    yin->pitchConfidence = 0.85;                                             // Initialize pitch confidence
    yin->pitchConfidence = 0.85;                                             // Initialize pitch confidence
    
    // Allocate memory for difference function buffer
    yin->yinDiffBuffer = (float*) malloc(sizeof(float) * yin->halfBuffer);
    for (int i = 0; i < yin->halfBuffer; i++) {
        yin->yinDiffBuffer[i] = 0.0;
    }
    return yin;
}

// Main function to detect pitch
float Yin_detectPitch(Yin* yin, float* audioBuffer) {
    Yin_DF(yin, audioBuffer); // Compute difference function
    Yin_CMND(yin); // Compute CMND function
    int tauEstimate = Yin_findThresholdCrossing(yin); // Find thresVal crossing
    
    if (tauEstimate != -1) {
        return yin->sampleRate / Yin_refineTauEstimate(yin, tauEstimate); // Compute pitch frequency
    }
    return -1; // No valid pitch detected
}


//////////////// CALCULATIONS FOR THE DETECT PITCH FUNCTION - PRETTY STANDARD

// Compute the difference function
void Yin_DF(Yin* yin, float* audioBuffer) {
    for (int tau = 0; tau < yin->halfBuffer; tau++) {
        yin->yinDiffBuffer[tau] = 0.0; // Reset buffer value
        for (int i = 0; i < yin->halfBuffer; i++) {
            float difference = audioBuffer[i] - audioBuffer[i + tau]; // Compute difference
            yin->yinDiffBuffer[tau] += difference * difference; // Sum squared differences
        }
    }
}

// Compute the cumulative mean normalized difference function
void Yin_CMND(Yin* yin) {
    yin->yinDiffBuffer[0] = 1.0; // First value is always 1

    float cumulativeSum = 0.0;

    for (int tau = 1; tau < yin->halfBuffer; tau++) {
        cumulativeSum += yin->yinDiffBuffer[tau]; // Compute cumulative sum
        yin->yinDiffBuffer[tau] *= tau / cumulativeSum; // Normalize values
    }
}

// Find the first minimum in CMND function below thresVal
int Yin_findThresholdCrossing(Yin* yin) {
    for (int tau = 2; tau < yin->halfBuffer; tau++) {
        if (yin->yinDiffBuffer[tau] < yin->thresVal) { // Check if below thresVal
            while (tau + 1 < yin->halfBuffer && yin->yinDiffBuffer[tau + 1] < yin->yinDiffBuffer[tau]) {
                tau++; // Move to local minimum
            }
            yin->pitchConfidence = 1 - yin->yinDiffBuffer[tau]; // Compute confidence
            return tau; // Return tau estimate
        }
    }
    yin->pitchConfidence = 0.0; // No valid pitch found
    return -1;
}

// Refine tau estimate using parabolic interpolation - short hand for cleaner reading basically same as one we're using
float Yin_refineTauEstimate(Yin* yin, int tauEstimate) {
    int prevIndex = (tauEstimate > 0) ? tauEstimate - 1 : tauEstimate;
    int nextIndex = (tauEstimate + 1 < yin->halfBuffer) ? tauEstimate + 1 : tauEstimate;
    
    if (prevIndex == tauEstimate || nextIndex == tauEstimate) {
        return tauEstimate; // Return original if at boundary
    }
    
    float s0 = yin->yinDiffBuffer[prevIndex];
    float s1 = yin->yinDiffBuffer[tauEstimate];
    float s2 = yin->yinDiffBuffer[nextIndex];
    
    return tauEstimate + (s2 - s0) / (2 * (2 * s1 - s2 - s0)); // Perform interpolation
}

// Get pitch confidence value
float Yin_getPitchConfidence(Yin* yin) {
    return yin->pitchConfidence;
}