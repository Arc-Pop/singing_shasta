#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "pico/binary_info.h"
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "lib/yinShasta.h"
#include "lib/pitch.h"
#include "pico/cyw43_arch.h"
#include <ctype.h>

#include "btstack_run_loop.h"
#include "lib/bluetooth/bt_audio.h"
#include "pico/multicore.h"
#include "btstack_event.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"
#include "pico/flash.h"
#include "pico/bootrom.h"


#include "hardware/uart.h"
#include "pico/stdlib.h"
#include "lib/sd_card/sd_audio.h"


#define ADC_NUM 0
#define ADC_PIN (26 + ADC_NUM)
#define ADC_VREF 3.3
#define ADC_RANGE (1 << 12)
#define ADC_CONVERT (ADC_VREF / (ADC_RANGE - 1))
#define SAMPLES 256                        // number of audio samples to take per FFT computation, must be power of 2
#define SAMPLING_FREQUENCY 4096

#define MIC_ADC 0
#define MIC_GPIO_PIN ADC_PIN
#define TOLERANCE 3.0

#define Yes_Button_PLAT 3 
#define No_Button_PLAT 7
#define Mic_LED_PLAT 9

#define Yes_Button_PROTO 4 
#define No_Button_PROTO 3
#define Mic_LED_PROTO 5

int8_t Yes_Button;
int8_t No_Button;
int8_t Mic_LED;

int buttonPressed=0;

#define USB_DETECT_PIN 28

uint16_t micSamplingPeriod;
uint64_t lastSampleTime;
float vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values
void sampleMic(void);
bool isListening;
uint adc_raw;
float userFreq=0;

int8_t compile_mode=0;
void CompileMode(int8_t type){

    compile_mode=type;

    switch(type){

        case 0:
        Yes_Button = Yes_Button_PLAT;
        No_Button = No_Button_PLAT;
        Mic_LED = Mic_LED_PLAT;
        break;

        case 1:
        Yes_Button = Yes_Button_PROTO;
        No_Button = No_Button_PROTO;
        Mic_LED = Mic_LED_PROTO;
        break;

        default:
        break;
    }

    gpio_init(USB_DETECT_PIN); // Initialize the GPIO pin
    gpio_set_dir(USB_DETECT_PIN, GPIO_IN); // Set as input

}

void LoadProgram(void){ 
    if(buttonPressed==1 && gpio_get(USB_DETECT_PIN)){ reset_usb_boot(0,0);} 
}

//Code from Bluetooth USB project.
static btstack_packet_callback_registration_t hci_event_callback_registration;

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(size);
    UNUSED(channel);
    bd_addr_t local_addr;
    if (packet_type != HCI_EVENT_PACKET) return;
    switch(hci_event_packet_get_type(packet)){
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) return;
            gap_local_bd_addr(local_addr);
            printf("BTstack up and running on %s.\n", bd_addr_to_str(local_addr));
            break;
        default:
            break;
    }
}
/******************************* */

Yin* yin;
void PitchDetectInit(void){
// Initialize ADC
adc_init();
adc_gpio_init(MIC_GPIO_PIN);
adc_select_input(MIC_ADC);
micSamplingPeriod = round(1000000*(1.0/SAMPLING_FREQUENCY));
yin = Yin_init(SAMPLING_FREQUENCY, SAMPLES);
}


float DetectPitch(void){

    float currentFreq = 0;

    while(1){
    float freqArray[3];
    int arrayFullFlag=0;
   
    while(arrayFullFlag<3){
    
    sampleMic();
    
    currentFreq = Yin_detectPitch(yin, vReal);

    if(currentFreq>Db2 && currentFreq<Db6){
    if(arrayFullFlag==0){ freqArray[arrayFullFlag]=currentFreq; arrayFullFlag++;}
    
    else{
        if(currentFreq>(freqArray[0]*1.1) || currentFreq<(freqArray[0]*0.9)){ arrayFullFlag=0; }
        else{ freqArray[arrayFullFlag]=currentFreq; arrayFullFlag++;}
    }
    
    }
    }
    
    currentFreq = (freqArray[0] + freqArray[1] + freqArray[2]) / 3;
    
    if(currentFreq > 0){ break; }
    }
    return currentFreq;
}


int GiveFeedback(float checkFreq, int targetFreq){

    int flatSharp = flatOrSharp(checkFreq, targetFreq);

    
    if((checkFreq > frequencies[targetFreq+1]) && flatSharp == 1){
            printf("but that's too high!\n\n");
            setTrack(9, "Feedback");
            setCommand('s');
            while(getAudioFlag()){}
            return 0;      
    }

    else if((checkFreq > frequencies[targetFreq]) && flatSharp == 1){
        printf("but that sounds sharp!\n\n");
        setTrack(1, "Feedback");
        setCommand('s');
        while(getAudioFlag()){}
        return 0;      
    }

    else if((checkFreq < frequencies[targetFreq-1]) && flatSharp == 1){
        printf("but that's too low!\n\n");    
        setTrack(8, "Feedback");
        setCommand('s');
        while(getAudioFlag()){}
        return 0;       
    }

    else if((checkFreq < frequencies[targetFreq]) && flatSharp == 1){
            printf("but that sounds flat!\n\n");    
            setTrack(0, "Feedback");
            setCommand('s');
            while(getAudioFlag()){}
            return 0;       
    }
    
    else if(flatSharp == 0) { 
            printf("and that sounds great!\n\n"); 
            setTrack(2, "Feedback");
            setCommand('s');
            while(getAudioFlag()){}
            return 1;
    }

    return 0;
}

uint64_t last_time;// = time_us_64();  // Get current time in microseconds
uint64_t this_time;// = 1000000; // 1 second
// Interrupt Service Routine
void button_isr(uint gpio, uint32_t events) {
    this_time = time_us_64();
    if((this_time-last_time)>1000000){
    if(gpio == Yes_Button && events & GPIO_IRQ_EDGE_FALL) { buttonPressed=1;}
    else if(gpio == No_Button && events & GPIO_IRQ_EDGE_FALL) { buttonPressed=2; }
    }
    last_time=this_time;
}


void LED_Init(void){
    gpio_init(Mic_LED);
    gpio_set_dir(Mic_LED, GPIO_OUT);
}

int8_t LED_State=0;
void Blink_LED(void){

    if(LED_State==0){ LED_State=1; }
    else{ LED_State=0; }
    gpio_put(Mic_LED, LED_State);
    sleep_ms(500);
    }


void Buttons_Init(void){
    gpio_init(Yes_Button); // Initialize the GPIO pin
    gpio_set_dir(Yes_Button, GPIO_IN); // Set as input
    gpio_pull_up(Yes_Button); // Enable internal pull-up resistor

    gpio_init(No_Button); // Initialize the GPIO pin
    gpio_set_dir(No_Button, GPIO_IN); // Set as input
    gpio_pull_up(No_Button); // Enable internal pull-up resistor

    // Enable IRQs and register the callback
    gpio_set_irq_enabled_with_callback(Yes_Button, GPIO_IRQ_EDGE_FALL, true, &button_isr);
    gpio_set_irq_enabled(No_Button, GPIO_IRQ_EDGE_FALL, true);
                                                                            
    printf("Buttons initialized\n");                                   
}

int Bluetooth_Init(void){
    if (cyw43_arch_init()) {
        printf("cyw43_arch_init() failed.\n");
        return -1;
    }

    // inform about BTstack state
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    int err = btstack_main(0, NULL);

    while(!get_a2dp_connected_flag()){ LoadProgram(); Blink_LED(); }

    printf("Bluetooth hardware initialized.\n");

    return 0;
}

int8_t vocalRange=0;

int findRange(void){
    float highNote=0;
    float lowNote=0;
    int findNote=0;

    setTrack(DYKLH, "Feedback");
    setCommand('s');
    while(getAudioFlag()){}

    if(buttonPressed==0){
    setTrack(PressButton, "Feedback");
    setCommand('s');
    while(getAudioFlag()){}
    }

    while(buttonPressed==0){sleep_ms(10);}

    if(buttonPressed==2){
    printf("This is a low note.\n");
    setTrack(LowNote, "Feedback");
    setCommand('s');
    while(getAudioFlag()){}

    setTrack(15, "Piano Notes");
    setCommand('s');
    while(getAudioFlag()){}

    printf("This is a high note.\n");
    setTrack(HighNote, "Feedback");
    setCommand('s');
    while(getAudioFlag()){}

    setTrack(63, "Piano Notes");
    setCommand('s');
    while(getAudioFlag()){}
    }
    buttonPressed=0;



    while(buttonPressed!=2){
        buttonPressed=0;

        printf("Sing as low as you can\n");
        setTrack(3, "Feedback");
        setCommand('s');
        while(getAudioFlag()){ lowNote=DetectPitch();}

        if(lowNote==0){ lowNote= DetectPitch(); }

        printf("Can you sing any lower than %f?\n", lowNote);
        setTrack(4, "Feedback");
        setCommand('s');
        while(getAudioFlag()){}

        findNote=0;
        while(lowNote>frequencies[findNote]){ findNote++; }
        findNote--;
        setTrack(findNote, "Piano Notes");
        setCommand('s');
        while(getAudioFlag()){}

        if(buttonPressed==2){break;}

        printf("Press yes or no\n");
        setTrack(7, "Feedback");
        setCommand('s');
        while(getAudioFlag()){}

        while(buttonPressed==0){sleep_ms(100);}
    }
    buttonPressed=0;
 
    if(findNote<24){ vocalRange=Bass;}
    if(findNote>=24 && findNote<27){ vocalRange=Baritone;}
    if(findNote>=27 && findNote<30){ vocalRange=Tenor;}
    if(findNote>=30 && findNote<32){ vocalRange=CounterTenor;}
    if(findNote>=32 && findNote<36){ vocalRange=Contralto;}
    if(findNote>=36 && findNote<39){ vocalRange=MezzSoprano;}
    if(findNote>=39){ vocalRange=Soprano;}


    while(buttonPressed!=2){
        buttonPressed=0;
        printf("Sing as high as you can\n"); 
        setTrack(5, "Feedback");
        setCommand('s');
        while(getAudioFlag()){
            highNote= DetectPitch();
        }
        if(highNote==0 || highNote<(lowNote+(lowNote*.25))){ 
            highNote= DetectPitch();
        }

        printf("Can you sing any higher than %f?\n", highNote);
        setTrack(6, "Feedback");
        setCommand('s');
        while(getAudioFlag()){}

        findNote=0;
        while(highNote>frequencies[findNote]){ findNote++; }
        setTrack(findNote, "Piano Notes");
        setCommand('s');
        while(getAudioFlag()){}

        if(buttonPressed==2){break;}

        printf("Press yes or no\n");
        setTrack(7, "Feedback");
        setCommand('s');
        while(getAudioFlag()){}

        while(buttonPressed==0){sleep_ms(100);}
    }
    buttonPressed=0;

    sleep_ms(1000);

    int keyIndex=0;
    int rangeSpace=0;
    while(frequencies[keyIndex]<lowNote){keyIndex++;}

    while(frequencies[keyIndex+rangeSpace+12]<=highNote){rangeSpace++;}

    rangeSpace= rangeSpace/2;

    keyIndex = keyIndex + rangeSpace;

    return keyIndex;

}


void PlayAllSine(void){
    for(int i=0; i<((sizeof(frequencies)/sizeof(frequencies[0]))-1); i++){
        setSine(frequencies[i]);
        printf("Playing ");
        for(int c=0; c<4; c++){
            if(pitchNames[i][c]=='\0'){break;}
            printf("%c", pitchNames[i][c]);
        }
        printf(" \ti = %i\n", i);
        setCommand('x');
        while(getAudioFlag()){sleep_ms(1);}
       }

}


void sampleMic(void){
    for(uint16_t i=0; i<SAMPLES; i++){
  
          lastSampleTime = time_us_64();
          vReal[i] = adc_read();
          vImag[i] = 0;
          while(time_us_64() < (lastSampleTime + micSamplingPeriod)){
              // wait until next sampling time if necessary
          }
      }
  }

void SineScale(int keyNote){

    setSineTime(1000);

    setSine(frequencies[keyNote]);
    setCommand('x');
    while(getAudioFlag()){}

    setSine(frequencies[keyNote+2]);
    setCommand('x');
    while(getAudioFlag()){}

    setSine(frequencies[keyNote+4]);
    setCommand('x');
    while(getAudioFlag()){}

    setSine(frequencies[keyNote+5]);
    setCommand('x');
    while(getAudioFlag()){}

    setSine(frequencies[keyNote+7]);
    setCommand('x');
    while(getAudioFlag()){}

    setSine(frequencies[keyNote+9]);
    setCommand('x');
    while(getAudioFlag()){}

    setSine(frequencies[keyNote+10]);
    setCommand('x');
    while(getAudioFlag()){}

    setSine(frequencies[keyNote+12]);
    setCommand('x');
    while(getAudioFlag()){}

}


void PianoScale(int keyNote){

    int noteTime=2000;

    setTrack(keyNote, "Piano Notes");
    setCommand('s');
    sleep_ms(noteTime);
    setCommand('p');

    setTrack(keyNote+2, "Piano Notes");
    setCommand('s');
    sleep_ms(noteTime);
    setCommand('p');

    setTrack(keyNote+4, "Piano Notes");
    setCommand('s');
    sleep_ms(noteTime);
    setCommand('p');

    setTrack(keyNote+5, "Piano Notes");
    setCommand('s');
    sleep_ms(noteTime);
    setCommand('p');

    setTrack(keyNote+7, "Piano Notes");
    setCommand('s');
    sleep_ms(noteTime);
    setCommand('p');

    setTrack(keyNote+9, "Piano Notes");
    setCommand('s');
    sleep_ms(noteTime);
    setCommand('p');

    setTrack(keyNote+10, "Piano Notes");
    setCommand('s');
    sleep_ms(noteTime);
    setCommand('p');

    setTrack(keyNote+12, "Piano Notes");
    setCommand('s');
    sleep_ms(noteTime);
    setCommand('p');

}


void SayNoteName(int frequencyIndex){

    int octaveTrack;
    int letterTrack;
    int sharpflatTrack=0;

    switch(pitchNames[frequencyIndex][0]){
        case 'A':
        letterTrack=9;
        break;

        case 'B':
        letterTrack=10;
        break;

        case 'C':
        letterTrack=11;
        break;

        case 'D':
        letterTrack=12;
        break;

        case 'E':
        letterTrack=13;
        break;

        case 'F':
        letterTrack=14;
        break;

        case 'G':
        letterTrack=15;
        break;
    }

    if(pitchNames[frequencyIndex][1]=='#'){sharpflatTrack=1;}

    else{
    switch(pitchNames[frequencyIndex][1]){
        case '0':
        octaveTrack=0;
        break;

        case '1':
        octaveTrack=1;
        break;

        case '2':
        octaveTrack=2;
        break;

        case '3':
        octaveTrack=3;
        break;

        case '4':
        octaveTrack=4;
        break;

        case '5':
        octaveTrack=5;
        break;

        case '6':
        octaveTrack=6;
        break;

        case '7':
        octaveTrack=7;
        break;

        case '8':
        octaveTrack=8;
        break;
    }
}
    if(pitchNames[frequencyIndex][1]=='#'){sharpflatTrack=1;

    switch(pitchNames[frequencyIndex][2]){
        case '0':
        octaveTrack=0;
        break;

        case '1':
        octaveTrack=1;
        break;

        case '2':
        octaveTrack=2;
        break;

        case '3':
        octaveTrack=3;
        break;

        case '4':
        octaveTrack=4;
        break;

        case '5':
        octaveTrack=5;
        break;

        case '6':
        octaveTrack=6;
        break;

        case '7':
        octaveTrack=7;
        break;

        case '8':
        octaveTrack=8;
        break;

        default:
        break;

    }
    }

    setTrack(letterTrack, "NoteNames");
    setCommand('s');
    while(getAudioFlag()){}

    if(sharpflatTrack==1){
    setTrack(17, "NoteNames");
    setCommand('s');
    while(getAudioFlag()){}
    }

    setTrack(octaveTrack, "NoteNames");
    setCommand('s');
    while(getAudioFlag()){}
}


void ShastaSings(void){

    int majorScale[8]={0,2,4,5,7,9,10,12};
    int scaleDegree=0;
    int Play=1;

    while(Play==1){
    int keyNoteIndex = findRange();

    printf("Let's start by singing the ");
    for(int c=0; c<4; c++){
        if(pitchNames[keyNoteIndex][c]=='\0'){break;}
        printf("%c", pitchNames[keyNoteIndex][c]);
    }
    printf(" scale\n");

    setTrack(SoundLike, "Feedback");
    setCommand('s');
    while(getAudioFlag()){}    

    setTrack(vocalRange, "Feedback");
    setCommand('s');
    while(getAudioFlag()){}    

    setTrack(MajorScale, "Feedback");
    setCommand('s');
    while(getAudioFlag()){}    

    SayNoteName(keyNoteIndex);

    setTrack(GoodScale, "Feedback");
    setCommand('s');
    while(getAudioFlag()){}    

    while(scaleDegree<8){
    int result=0;
    int firstTry=0;
    while(!result){
    if(firstTry==0){    
    printf("Try to sing this note.\n");
    setTrack(CanYou, "Feedback");
    setCommand('s');
    while(getAudioFlag()){}   

    SayNoteName(keyNoteIndex+majorScale[scaleDegree]);
    }

    //setFade();
    setTrack((keyNoteIndex+majorScale[scaleDegree]), "Piano Notes");
    setCommand('s');
    while(getAudioFlag()){}
    userFreq = DetectPitch();

    if((userFreq*2)>(frequencies[keyNoteIndex+majorScale[scaleDegree]] *.9) && (userFreq*2)<(frequencies[keyNoteIndex+majorScale[scaleDegree]] *1.1)){
        userFreq=userFreq*2;
    }
    else if((userFreq*.5)>(frequencies[keyNoteIndex+majorScale[scaleDegree]] *.9) && (userFreq*.5)<(frequencies[keyNoteIndex+majorScale[scaleDegree]] *1.1)){
        userFreq=userFreq*.5;
    }    

    result = GiveFeedback(userFreq, keyNoteIndex+majorScale[scaleDegree]);
    printf("Your frequency: %f\n", userFreq);
    while(getAudioFlag()){}
    sleep_ms(1000);
    if(result==0){
        printf("Try again!\n");    
        setTrack(10, "Feedback");
        setCommand('s');
        while(getAudioFlag()){}
        firstTry=1;     
        }           
    }
    firstTry=0;
    scaleDegree++;
}
    printf("Congratualations! You did it!");
    setTrack(12, "Feedback");
    setCommand('s');
    while(getAudioFlag()){}

    setTrack(PlayAgain, "Feedback");
    setCommand('s');
    while(getAudioFlag()){}

    setTrack(PressButton, "Feedback");
    setCommand('s');
    while(getAudioFlag()){}

    while(buttonPressed==0){sleep_ms(10);}
    if(buttonPressed==0){Play=0;}    
}
}


void PlayIntro(void){

    if(compile_mode==0){
    setTrack(0, "Intro");
    setCommand('s');
    while(getAudioFlag()){ Blink_LED();}
    }

    gpio_put(Mic_LED, true);

    setTrack(1, "Intro");
    setCommand('s');
    while(getAudioFlag()){}

    setTrack(2, "Intro");
    setCommand('s');
    while(getAudioFlag()){}

    setTrack(PressYes, "Feedback");
    setCommand('s');
    while(getAudioFlag()){}

    while(buttonPressed!=1){sleep_ms(10);}
    buttonPressed=0;
}


void PlayAllNotes(void){

    for(int i=0; i<sizeof(frequencies)/sizeof(frequencies[0]); i++){
        setTrack(i, "Piano Notes");

        printf("This note is ");
        for(int c=0; c<4; c++){
            if(pitchNames[i][c]=='\0'){break;}
            printf("%c", pitchNames[i][c]);
        }
        printf("\nIts frequency is %f \n", frequencies[i]);

        setCommand('s');
        while(getAudioFlag()){}
    }

}

int main()
{
    CompileMode(1); // 0 For platform, 1 for prototype

    stdio_init_all();

    LED_Init();

    Buttons_Init();

    PitchDetectInit();
    
    sd_init(compile_mode);
    
    Bluetooth_Init();    

    PlayIntro();

    ShastaSings();

    return 0;
}