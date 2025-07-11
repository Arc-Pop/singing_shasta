#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "btstack.h"
#include "../bluetooth/bt_audio.h"

#include "../sd_card/tf_card.h"
#include "../sd_card/fatfs/ff.h"
#include "sd_audio.h"

// Set PRE_ALLOCATE true to pre-allocate file clusters.
const bool PRE_ALLOCATE = true;

// Set SKIP_FIRST_LATENCY true if the first read/write to the SD can
// be avoid by writing a file header or reading the first record.
const bool SKIP_FIRST_LATENCY = true;

// Size of read/write.
const size_t BUF_SIZE = 1024;

// Size of read/write.
const size_t TAG_BUF_SIZE = 44;

// File size in MB where MB = 1,000,000 bytes.
const uint32_t FILE_SIZE_MB =25;

// Write pass count.
const uint8_t WRITE_COUNT = 2;

// Read pass count.
const uint8_t READ_COUNT = 2;
//==============================================================================
// End of configuration constants.
//------------------------------------------------------------------------------

int16_t byteConvert(const uint8_t data[2]) {
    uint32_t val = (((uint32_t)data[0]) << 0) | 
                   (((uint32_t)data[1]) << 8);
    return ((int32_t) val) - 0x10000u;
}



DIR dir;                    // Directory
FILINFO fno;                // File Info
int16_t audPlace=0;
FATFS fs;
FIL fil;
FRESULT fr;     
UINT br;
UINT bw;
void sd_init(int8_t compile_mode){

    if(compile_mode==0){
    pico_fatfs_spi_config_t config = {
        spi1,
        CLK_SLOW_DEFAULT,
        CLK_FAST_DEFAULT,
        PIN_SPI1_MISO_DEFAULT,
        PIN_SPI1_CS_DEFAULT,
        PIN_SPI1_SCK_DEFAULT,
        PIN_SPI1_MOSI_DEFAULT,
        true  // use internal pullup
    };
    pico_fatfs_set_config(&config);
    }
    else{
    pico_fatfs_spi_config_t config = {
        spi0,
        CLK_SLOW_DEFAULT,
        CLK_FAST_DEFAULT,
        PIN_SPI0_MISO_DEFAULT,
        PIN_SPI0_CS_DEFAULT,
        PIN_SPI0_SCK_DEFAULT,
        PIN_SPI0_MOSI_DEFAULT,
        true  // use internal pullup
    };
    pico_fatfs_set_config(&config);
    }


    for (int i = 0; i < 5; i++) {
        fr = f_mount(&fs, "", 1);
        if (fr == FR_OK) { break; }
        printf("mount error %d -> retry %d\n", fr, i);
        pico_fatfs_reboot_spi();
    }
//    f_lseek(&fil , 5);
    printf("SD Card Initialized\n");

    f_opendir(&dir, "/");   // Open Root
do
{
    f_readdir(&dir, &fno);
    if(fno.fname[0] != 0)
        printf("File found: %s\n", fno.fname); // Print File Name
} while(fno.fname[0] != 0);

f_closedir(&dir);

}


long lendian32(int firstIndex, uint8_t array[]){

    long converted = ((array[firstIndex] & 0xFF) << 0) |
    ((array[firstIndex+1] & 0xFF) << 8) |
    ((array[firstIndex+2] & 0xFF) << 16) |
    ((array[firstIndex+3] & 0xFF) << 24);
    return converted;
}

short lendian16(int firstIndex, uint8_t array[]){
    short converted = ((array[firstIndex] & 0xFF) << 0) | ((array[firstIndex+1] & 0xFF) << 8);
    return converted;
}


    unsigned long fileSize=0;

int8_t trackNum;
char* Directory;
void setTrack(int8_t trackN, char* Dir){ trackNum= trackN; Directory=Dir;}
    

void Open_Intro(void){
    switch(trackNum){
        case 0:    
        fr = f_open(&fil, "Intro/Silence3.wav", FA_READ); // Open file.
        break;
    
        case 1:    
        fr = f_open(&fil, "Intro/Intro.wav", FA_READ); // Open file.
        break;
    
        case 2:    
        fr = f_open(&fil, "Intro/ShastaIntro.wav", FA_READ); // Open file.
        break;
    }
}


void Open_Feedback(void){
    switch(trackNum){
        case Flat: 
        fr = f_open(&fil, "Feedback/Flat.wav", FA_READ); // Open file.
        break;  

        case Sharp: 
        fr = f_open(&fil, "Feedback/Sharp.wav", FA_READ); // Open file.
        break;  

        case Great: 
        fr = f_open(&fil, "Feedback/Great.wav", FA_READ); // Open file.
        break;  

        case SingLow: 
        fr = f_open(&fil, "Feedback/SingLow.wav", FA_READ); // Open file.
        break;  

        case SingLower: 
        fr = f_open(&fil, "Feedback/SingLower.wav", FA_READ); // Open file.
        break;  

        case SingHigh: 
        fr = f_open(&fil, "Feedback/SingHigh.wav", FA_READ); // Open file.
        break;  

        case SingHigher: 
        fr = f_open(&fil, "Feedback/SingHigher.wav", FA_READ); // Open file.
        break;  

        case PressButton: 
        fr = f_open(&fil, "Feedback/PressButton.wav", FA_READ); // Open file.
        break;  

        case TooLow: 
        fr = f_open(&fil, "Feedback/TooLow.wav", FA_READ); // Open file.
        break;  

        case TooHigh: 
        fr = f_open(&fil, "Feedback/TooHigh.wav", FA_READ); // Open file.
        break;  

        case TryAgain: 
        fr = f_open(&fil, "Feedback/TryAgain.wav", FA_READ); // Open file.
        break;  

        case ThisNote: 
        fr = f_open(&fil, "Feedback/ThisNote.wav", FA_READ); // Open file.
        break;  

        case DidIt: 
        fr = f_open(&fil, "Feedback/DidIt.wav", FA_READ); // Open file.
        break;  

        case LowNote: 
        fr = f_open(&fil, "Feedback/LowNote.wav", FA_READ); // Open file.
        break;  

        case HighNote: 
        fr = f_open(&fil, "Feedback/HighNote.wav", FA_READ); // Open file.
        break;  

        case PressYes: 
        fr = f_open(&fil, "Feedback/PressYes.wav", FA_READ); // Open file.
        break;  

        case Baritone: 
        fr = f_open(&fil, "Feedback/Baritone.wav", FA_READ); // Open file.
        break;  

        case Bass: 
        fr = f_open(&fil, "Feedback/Bass.wav", FA_READ); // Open file.
        break;  

        case Chord: 
        fr = f_open(&fil, "Feedback/Chord.wav", FA_READ); // Open file.
        break;  

        case Contralto: 
        fr = f_open(&fil, "Feedback/Contralto.wav", FA_READ); // Open file.
        break;  

        case CounterTenor: 
        fr = f_open(&fil, "Feedback/CounterTenor.wav", FA_READ); // Open file.
        break;  

        case DYKFlat: 
        fr = f_open(&fil, "Feedback/DYKFlat.wav", FA_READ); // Open file.
        break;  

        case DYKLH: 
        fr = f_open(&fil, "Feedback/DYKLH.wav", FA_READ); // Open file.
        break;  

        case DYKSharp: 
        fr = f_open(&fil, "Feedback/DYKSharp.wav", FA_READ); // Open file.
        break;  

        case Fifth: 
        fr = f_open(&fil, "Feedback/Fifth.wav", FA_READ); // Open file.
        break;  

        case FiveNotes: 
        fr = f_open(&fil, "Feedback/FiveNotes.wav", FA_READ); // Open file.
        break;  

        case FlatMeans: 
        fr = f_open(&fil, "Feedback/FlatMeans.wav", FA_READ); // Open file.
        break;  

        case FourNotes: 
        fr = f_open(&fil, "Feedback/FourNotes.wav", FA_READ); // Open file.
        break;  

        case Fourth: 
        fr = f_open(&fil, "Feedback/Fourth.wav", FA_READ); // Open file.
        break;  

        case GoodScale: 
        fr = f_open(&fil, "Feedback/GoodScale.wav", FA_READ); // Open file.
        break;  

        case HighestNote: 
        fr = f_open(&fil, "Feedback/HighestNote.wav", FA_READ); // Open file.
        break;  

        case LowerNote: 
        fr = f_open(&fil, "Feedback/LowerNote.wav", FA_READ); // Open file.
        break;  

        case MajorScale: 
        fr = f_open(&fil, "Feedback/MajorScale.wav", FA_READ); // Open file.
        break;  

        case MezzSoprano: 
        fr = f_open(&fil, "Feedback/MezzSoprano.wav", FA_READ); // Open file.
        break;  

        case Octave: 
        fr = f_open(&fil, "Feedback/Octave.wav", FA_READ); // Open file.
        break;  

        case OrA: 
        fr = f_open(&fil, "Feedback/OrA.wav", FA_READ); // Open file.
        break;  

        case PlayAgain: 
        fr = f_open(&fil, "Feedback/PlayAgain.wav", FA_READ); // Open file.
        break;  

        case SevenNotes: 
        fr = f_open(&fil, "Feedback/SevenNotes.wav", FA_READ); // Open file.
        break;  

        case SharpMeans: 
        fr = f_open(&fil, "Feedback/SharpMeans.wav", FA_READ); // Open file.
        break;  

        case SingChord: 
        fr = f_open(&fil, "Feedback/SingChord.wav", FA_READ); // Open file.
        break;  

        case SingFifth: 
        fr = f_open(&fil, "Feedback/SingFifth.wav", FA_READ); // Open file.
        break;  

        case SingFourth: 
        fr = f_open(&fil, "Feedback/SingFourth.wav", FA_READ); // Open file.
        break;  

        case SingOctave: 
        fr = f_open(&fil, "Feedback/SingOctave.wav", FA_READ); // Open file.
        break;  

        case SingThird: 
        fr = f_open(&fil, "Feedback/SingThird.wav", FA_READ); // Open file.
        break;  

        case SixNotes: 
        fr = f_open(&fil, "Feedback/SixNotes.wav", FA_READ); // Open file.
        break;  

        case Soprano: 
        fr = f_open(&fil, "Feedback/Soprano.wav", FA_READ); // Open file.
        break;  

        case SoundLike: 
        fr = f_open(&fil, "Feedback/SoundLike.wav", FA_READ); // Open file.
        break;  

        case Tenor: 
        fr = f_open(&fil, "Feedback/Tenor.wav", FA_READ); // Open file.
        break;  

        case Third: 
        fr = f_open(&fil, "Feedback/Third.wav", FA_READ); // Open file.
        break;   

        case ThreeNotes: 
        fr = f_open(&fil, "Feedback/ThreeNotes.wav", FA_READ); // Open file.
        break;  

        case TwoNotes: 
        fr = f_open(&fil, "Feedback/TwoNotes.wav", FA_READ); // Open file.
        break;  

        case WholeScale: 
        fr = f_open(&fil, "Feedback/WholeScale.wav", FA_READ); // Open file.
        break;
        
        case CanYou: 
        fr = f_open(&fil, "Feedback/CanYou.wav", FA_READ); // Open file.
        break;
        
    }
}


void Open_NoteNames(void){
    switch(trackNum){
        case 0: 
        fr = f_open(&fil, "NoteNames/Zero.wav", FA_READ); // Open file.
        break;  

        case 1: 
        fr = f_open(&fil, "NoteNames/One.wav", FA_READ); // Open file.
        break;  

        case 2: 
        fr = f_open(&fil, "NoteNames/Two.wav", FA_READ); // Open file.
        break;  

        case 3: 
        fr = f_open(&fil, "NoteNames/Three.wav", FA_READ); // Open file.
        break;  

        case 4: 
        fr = f_open(&fil, "NoteNames/Four.wav", FA_READ); // Open file.
        break;  

        case 5: 
        fr = f_open(&fil, "NoteNames/Five.wav", FA_READ); // Open file.
        break;  

        case 6: 
        fr = f_open(&fil, "NoteNames/Six.wav", FA_READ); // Open file.
        break;  

        case 7: 
        fr = f_open(&fil, "NoteNames/Seven.wav", FA_READ); // Open file.
        break;  

        case 8: 
        fr = f_open(&fil, "NoteNames/Eight.wav", FA_READ); // Open file.
        break;  

        case 9: 
        fr = f_open(&fil, "NoteNames/A.wav", FA_READ); // Open file.
        break;  

        case 10: 
        fr = f_open(&fil, "NoteNames/B.wav", FA_READ); // Open file.
        break;  

        case 11: 
        fr = f_open(&fil, "NoteNames/C.wav", FA_READ); // Open file.
        break;  

        case 12: 
        fr = f_open(&fil, "NoteNames/D.wav", FA_READ); // Open file.
        break;  

        case 13: 
        fr = f_open(&fil, "NoteNames/E.wav", FA_READ); // Open file.
        break;  

        case 14: 
        fr = f_open(&fil, "NoteNames/F.wav", FA_READ); // Open file.
        break;  

        case 15: 
        fr = f_open(&fil, "NoteNames/G.wav", FA_READ); // Open file.
        break;  

        case 16: 
        fr = f_open(&fil, "NoteNames/NoteFlat.wav", FA_READ); // Open file.
        break;  

        case 17: 
        fr = f_open(&fil, "NoteNames/NoteSharp.wav", FA_READ); // Open file.
        break;  
    }
}


void Open_Piano(void){
    switch(trackNum){
        case 0: 
        fr = f_open(&fil, "Piano Notes/01A0.wav", FA_READ); // Open file.
        break;
        
        case 1: 
        fr = f_open(&fil, "Piano Notes/02B0b.wav", FA_READ); // Open file.
        break;  

        case 2: 
        fr = f_open(&fil, "Piano Notes/03B0.wav", FA_READ); // Open file.
        break;  

        case 3: 
        fr = f_open(&fil, "Piano Notes/04C1.wav", FA_READ); // Open file.
        break;  

        case 4: 
        fr = f_open(&fil, "Piano Notes/05D1b.wav", FA_READ); // Open file.
        break;  

        case 5: 
        fr = f_open(&fil, "Piano Notes/06D1.wav", FA_READ); // Open file.
        break;  

        case 6: 
        fr = f_open(&fil, "Piano Notes/07E1b.wav", FA_READ); // Open file.
        break;  

        case 7: 
        fr = f_open(&fil, "Piano Notes/08E1.wav", FA_READ); // Open file.
        break;  

        case 8: 
        fr = f_open(&fil, "Piano Notes/09F1.wav", FA_READ); // Open file.
        break;  

        case 9: 
        fr = f_open(&fil, "Piano Notes/10G1b.wav", FA_READ); // Open file.
        break;  

        case 10: 
        fr = f_open(&fil, "Piano Notes/11G1.wav", FA_READ); // Open file.
        break;  

        case 11: 
        fr = f_open(&fil, "Piano Notes/12A1b.wav", FA_READ); // Open file.
        break;  

        case 12: 
        fr = f_open(&fil, "Piano Notes/13A1.wav", FA_READ); // Open file.
        break;  

        case 13: 
        fr = f_open(&fil, "Piano Notes/14B1b.wav", FA_READ); // Open file.
        break;  

        case 14: 
        fr = f_open(&fil, "Piano Notes/15B1.wav", FA_READ); // Open file.
        break;  

        case 15: 
        fr = f_open(&fil, "Piano Notes/16C2.wav", FA_READ); // Open file.
        break;  

        case 16: 
        fr = f_open(&fil, "Piano Notes/17D2b.wav", FA_READ); // Open file.
        break;  

        case 17: 
        fr = f_open(&fil, "Piano Notes/18D2.wav", FA_READ); // Open file.
        break;  

        case 18: 
        fr = f_open(&fil, "Piano Notes/19E2b.wav", FA_READ); // Open file.
        break;  

        case 19: 
        fr = f_open(&fil, "Piano Notes/20E2.wav", FA_READ); // Open file.
        break;  

        case 20: 
        fr = f_open(&fil, "Piano Notes/21F2.wav", FA_READ); // Open file.
        break;  

        case 21: 
        fr = f_open(&fil, "Piano Notes/22G2b.wav", FA_READ); // Open file.
        break;  

        case 22: 
        fr = f_open(&fil, "Piano Notes/23G2.wav", FA_READ); // Open file.
        break;  

        case 23: 
        fr = f_open(&fil, "Piano Notes/24A2b.wav", FA_READ); // Open file.
        break;  

        case 24: 
        fr = f_open(&fil, "Piano Notes/25A2.wav", FA_READ); // Open file.
        break;  

        case 25: 
        fr = f_open(&fil, "Piano Notes/26B2b.wav", FA_READ); // Open file.
        break;  

        case 26: 
        fr = f_open(&fil, "Piano Notes/27B2.wav", FA_READ); // Open file.
        break;  

        case 27: 
        fr = f_open(&fil, "Piano Notes/28C3.wav", FA_READ); // Open file.
        break;  

        case 28: 
        fr = f_open(&fil, "Piano Notes/29D3b.wav", FA_READ); // Open file.
        break;  

        case 29: 
        fr = f_open(&fil, "Piano Notes/30D3.wav", FA_READ); // Open file.
        break;  

        case 30: 
        fr = f_open(&fil, "Piano Notes/31E3b.wav", FA_READ); // Open file.
        break;  

        case 31: 
        fr = f_open(&fil, "Piano Notes/32E3.wav", FA_READ); // Open file.
        break;  

        case 32: 
        fr = f_open(&fil, "Piano Notes/33F3.wav", FA_READ); // Open file.
        break;  

        case 33: 
        fr = f_open(&fil, "Piano Notes/34G3b.wav", FA_READ); // Open file.
        break;  

        case 34: 
        fr = f_open(&fil, "Piano Notes/35G3.wav", FA_READ); // Open file.
        break;  

        case 35: 
        fr = f_open(&fil, "Piano Notes/36A3b.wav", FA_READ); // Open file.
        break;  

        case 36: 
        fr = f_open(&fil, "Piano Notes/37A3.wav", FA_READ); // Open file.
        break;  

        case 37: 
        fr = f_open(&fil, "Piano Notes/38B3b.wav", FA_READ); // Open file.
        break;  

        case 38: 
        fr = f_open(&fil, "Piano Notes/39B3.wav", FA_READ); // Open file.
        break;  

        case 39: 
        fr = f_open(&fil, "Piano Notes/40C4.wav", FA_READ); // Open file.
        break;  

        case 40: 
        fr = f_open(&fil, "Piano Notes/41D4b.wav", FA_READ); // Open file.
        break;  

        case 41: 
        fr = f_open(&fil, "Piano Notes/42D4.wav", FA_READ); // Open file.
        break;  

        case 42: 
        fr = f_open(&fil, "Piano Notes/43E4b.wav", FA_READ); // Open file.
        break;  

        case 43: 
        fr = f_open(&fil, "Piano Notes/44E4.wav", FA_READ); // Open file.
        break;  

        case 44: 
        fr = f_open(&fil, "Piano Notes/45F4.wav", FA_READ); // Open file.
        break;  

        case 45: 
        fr = f_open(&fil, "Piano Notes/46G4b.wav", FA_READ); // Open file.
        break;  

        case 46: 
        fr = f_open(&fil, "Piano Notes/47G4.wav", FA_READ); // Open file.
        break;  

        case 47: 
        fr = f_open(&fil, "Piano Notes/48A4b.wav", FA_READ); // Open file.
        break;  

        case 48: 
        fr = f_open(&fil, "Piano Notes/49A4.wav", FA_READ); // Open file.
        break;  

        case 49: 
        fr = f_open(&fil, "Piano Notes/50B4b.wav", FA_READ); // Open file.
        break;  

        case 50: 
        fr = f_open(&fil, "Piano Notes/51B4.wav", FA_READ); // Open file.
        break;  

        case 51: 
        fr = f_open(&fil, "Piano Notes/52C5.wav", FA_READ); // Open file.
        break;
        
        case 52: 
        fr = f_open(&fil, "Piano Notes/53D5b.wav", FA_READ); // Open file.
        break;  

        case 53: 
        fr = f_open(&fil, "Piano Notes/54D5.wav", FA_READ); // Open file.
        break;  

        case 54: 
        fr = f_open(&fil, "Piano Notes/55E5b.wav", FA_READ); // Open file.
        break;  

        case 55: 
        fr = f_open(&fil, "Piano Notes/56E5.wav", FA_READ); // Open file.
        break;  

        case 56: 
        fr = f_open(&fil, "Piano Notes/57F5.wav", FA_READ); // Open file.
        break;  

        case 57: 
        fr = f_open(&fil, "Piano Notes/58G5b.wav", FA_READ); // Open file.
        break;  

        case 58: 
        fr = f_open(&fil, "Piano Notes/59G5.wav", FA_READ); // Open file.
        break;  

        case 59: 
        fr = f_open(&fil, "Piano Notes/60A5b.wav", FA_READ); // Open file.
        break;  

        case 60: 
        fr = f_open(&fil, "Piano Notes/61A5.wav", FA_READ); // Open file.
        break;  

        case 61: 
        fr = f_open(&fil, "Piano Notes/62B5b.wav", FA_READ); // Open file.
        break;  

        case 62: 
        fr = f_open(&fil, "Piano Notes/63B5.wav", FA_READ); // Open file.
        break;  

        case 63: 
        fr = f_open(&fil, "Piano Notes/64C6.wav", FA_READ); // Open file.
        break;  

        case 64: 
        fr = f_open(&fil, "Piano Notes/65D6b.wav", FA_READ); // Open file.
        break;  

        case 65: 
        fr = f_open(&fil, "Piano Notes/66D6.wav", FA_READ); // Open file.
        break;  

        case 66: 
        fr = f_open(&fil, "Piano Notes/67E6b.wav", FA_READ); // Open file.
        break;  

        case 67: 
        fr = f_open(&fil, "Piano Notes/68E6.wav", FA_READ); // Open file.
        break;  

        case 68: 
        fr = f_open(&fil, "Piano Notes/69F6.wav", FA_READ); // Open file.
        break;  

        case 69: 
        fr = f_open(&fil, "Piano Notes/70G6b.wav", FA_READ); // Open file.
        break;  

        case 70: 
        fr = f_open(&fil, "Piano Notes/71G6.wav", FA_READ); // Open file.
        break;  

        case 71: 
        fr = f_open(&fil, "Piano Notes/72A6b.wav", FA_READ); // Open file.
        break;  

        case 72: 
        fr = f_open(&fil, "Piano Notes/73A6.wav", FA_READ); // Open file.
        break;  

        case 73: 
        fr = f_open(&fil, "Piano Notes/74B6b.wav", FA_READ); // Open file.
        break;  

        case 74: 
        fr = f_open(&fil, "Piano Notes/75B6.wav", FA_READ); // Open file.
        break;  

        case 75: 
        fr = f_open(&fil, "Piano Notes/76C7.wav", FA_READ); // Open file.
        break;  

        case 76: 
        fr = f_open(&fil, "Piano Notes/77D7b.wav", FA_READ); // Open file.
        break;  

        case 77: 
        fr = f_open(&fil, "Piano Notes/78D7.wav", FA_READ); // Open file.
        break;  

        case 78: 
        fr = f_open(&fil, "Piano Notes/79E7b.wav", FA_READ); // Open file.
        break;  

        case 79: 
        fr = f_open(&fil, "Piano Notes/80E7.wav", FA_READ); // Open file.
        break;  

        case 80: 
        fr = f_open(&fil, "Piano Notes/81F7.wav", FA_READ); // Open file.
        break;  

        case 81: 
        fr = f_open(&fil, "Piano Notes/82G7b.wav", FA_READ); // Open file.
        break;  

        case 82: 
        fr = f_open(&fil, "Piano Notes/83G7.wav", FA_READ); // Open file.
        break;  

        case 83: 
        fr = f_open(&fil, "Piano Notes/84A7b.wav", FA_READ); // Open file.
        break;  

        case 84: 
        fr = f_open(&fil, "Piano Notes/85A7.wav", FA_READ); // Open file.
        break;  

        case 85: 
        fr = f_open(&fil, "Piano Notes/86B7b.wav", FA_READ); // Open file.
        break;  

        case 86: 
        fr = f_open(&fil, "Piano Notes/87B7.wav", FA_READ); // Open file.
        break;  

        case 87: 
        fr = f_open(&fil, "Piano Notes/88C8.wav", FA_READ); // Open file.
        break;  
    }
}


void sd_open(void){

    if(Directory=="Intro"){ Open_Intro(); }

    else if(Directory=="Feedback"){ Open_Feedback();}

    else if(Directory=="NoteNames"){ Open_NoteNames(); }

    else if(Directory=="Piano Notes"){ Open_Piano(); }

}  



void sd_read_tag(int trackNum){

    // File size in bytes.
 //   const uint32_t FILE_SIZE = 1000000UL*FILE_SIZE_MB;

    // Insure 4-byte alignment.
    uint32_t buf32[(BUF_SIZE + 3)/4];
    uint8_t* buf = (uint8_t*)buf32;

    //fr = f_open(&fil, "Great.wav", FA_READ);

    sd_open();

    uint32_t tagSize = 44;          // Number of bytes in wav file header.
    uint8_t  tagBuffer[tagSize];    // Array to read wav header tags into.

    int byteCount=0;

    fr = f_read(&fil, tagBuffer, TAG_BUF_SIZE, &br);

    printf("Tag data on file...\n");

    printf("Chunk ID:\t\t");
    for(int i=0; i<4; i++){printf("%c", tagBuffer[i]);}
    printf("\n");

    printf("Chunk Size:\t\t");
    printf("%d ", lendian32(4, tagBuffer));   
    printf("\n");

    printf("WAVE ID:\t\t");
    for(int i=8; i<12; i++){printf("%c", tagBuffer[i]);}
    printf("\n");

    printf("Chunk ID:\t\t");
    for(int i=12; i<16; i++){printf("%c", tagBuffer[i]);}
    printf("\n");

    printf("Subchunk1 Size:\t\t");
    printf("%d ", lendian32(16, tagBuffer));   
    printf("\n");

    printf("Audio Format:\t\t");
    if(lendian16(20, tagBuffer)==1){printf("PCM");}
    else{printf("%d ", lendian16(20, tagBuffer)); }
    printf("\n");

    printf("Number of Channels:\t");
    if(lendian16(22, tagBuffer)==1){ printf("Mono"); }
    else{ printf("Stereo"); }
    printf("\n");

    printf("Sample Rate:\t\t");
    printf("%d ", lendian32(24, tagBuffer));   
    printf("\n");

    printf("Byte Rate:\t\t");
    printf("%d ", lendian32(28, tagBuffer));   
    printf("\n");

    printf("Data Block Size:\t");
    printf("%d ", lendian16(32, tagBuffer));   
    printf("\n");

    printf("Bits Per Sample:\t");
    printf("%d ", lendian16(32, tagBuffer));   
    printf("\n");

    printf("Subchunk2ID:\t\t");
    for(int i=36; i<40; i++){printf("%c", tagBuffer[i]);}
    printf("\n");

    printf("Data Bytes:\t\t");
    printf("%d ", lendian32(40, tagBuffer));   
    printf("\n\n");

    fileSize = lendian32(40, tagBuffer);

    f_close(&fil);
}

 void sd_read_file_size(int trackNum){
    
        sd_open();

        uint32_t buf32[(BUF_SIZE + 3)/4];
        uint8_t* buf = (uint8_t*)buf32;
        uint32_t tagSize = 44;          // Number of bytes in wav file header.
        uint8_t  tagBuffer[tagSize];    // Array to read wav header tags into.
    
        int byteCount=0;
    
        fr = f_read(&fil, tagBuffer, TAG_BUF_SIZE, &br);
    
        fileSize = lendian32(40, tagBuffer);

        f_close(&fil);
 }



int wavLocation=44;
void setWavLoc(void){wavLocation=44;}


int16_t audioBuffer[8192]; // Buffer to return audio data from the SD card.
int16_t* sd_read(){
    int sdReadSize=sizeof(audioBuffer)/sizeof(audioBuffer[0]); // Number of elements to be read is 2*elements in audioBuffer
    uint32_t buf16[((sdReadSize) + 1)/2]; // 16-bit alignment...2048 bytes
    uint8_t* buf = (uint8_t*)buf16; // Buffer for reading data off sd card.

    sd_read_file_size(trackNum);

    sd_open();

    if((wavLocation+sdReadSize)>fileSize){
        sdReadSize=fileSize-wavLocation;
        for(int i=sdReadSize; i<sizeof(audioBuffer)/sizeof(audioBuffer[0]); i++){
            buf[i]=0;
        }
    }

    f_lseek(&fil, wavLocation); // Seek current location.

    uint8_t conBuffer[2]; // Conversion buffer for hold 2 bytes to be converted into a 16 bit signed sample in PCM
    int conIndex=0; // Index for the conversion buffer.

        fr = f_read(&fil, buf, sdReadSize, &br); // Read 2048 buffer into buf to

        int bufIndex=0;
        for(int c=0; c<sdReadSize; c++){
        conBuffer[conIndex]=buf[c];
        if(conIndex==1){            
        audioBuffer[bufIndex]=byteConvert(conBuffer);
//        pcm_buf[bufIndex]=byteConvert(conBuffer);
        bufIndex++;
        audioBuffer[bufIndex]=audioBuffer[bufIndex-1];
        bufIndex++;
        conIndex=0;
        }
        else{conIndex++;}
    }

    if((wavLocation+sdReadSize)<fileSize){ wavLocation+=sdReadSize; }  

    if((wavLocation+sdReadSize)==fileSize){
       setCommand('p'); 
       wavLocation=44;
    }

    f_close(&fil);
    return audioBuffer;
}




