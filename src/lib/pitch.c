#include "pitch.h"

int getPitchName(float frequency){

int index=0;

while(frequency>frequencies[index]){index++;}

float midpointFrequency = ( (frequencies[index] - frequencies[index-1]) / 2 ) + frequencies[index-1];

if( frequency > midpointFrequency ){ return index; }

return (index-1);

}


int flatOrSharp(float frequency, int index){

float pitchTolerance;

if(frequency >= frequencies[index]){ 

pitchTolerance = (frequencies[index+1] - frequencies[index]) * tolerance;

pitchTolerance = frequencies[index] + pitchTolerance;

if(frequency > pitchTolerance){ return 1; }

else{return 0;}

}

else{ 
pitchTolerance = (frequencies[index] - frequencies[index-1]) * tolerance;

pitchTolerance = frequencies[index] - pitchTolerance;

if(frequency < pitchTolerance){ return 1; }

else{return 0;}
}

return 0;

}
