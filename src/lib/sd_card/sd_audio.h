#ifndef SD_AUDIO_H
#define SD_AUDIO_H

void sd_init(int8_t compile_mode);

long lendian32(int firstIndex, uint8_t array[]);

short lendian16(int firstIndex, uint8_t array[]);

void sd_read_tag(int trackNum);

void setTrack(int8_t trackN, char* Dir);

int16_t* sd_read(void);

void sd_read_file_size(int trackNum);

void setWavLoc(void);

#define Flat 0
#define Sharp 1
#define Great 2
#define SingLow 3
#define SingLower 4
#define SingHigh 5
#define SingHigher 6
#define PressButton 7
#define TooLow 8
#define TooHigh 9
#define TryAgain 10
#define ThisNote 11
#define DidIt 12
#define LowNote 13
#define HighNote 14
#define PressYes 15
#define Baritone 16
#define Bass 17
#define Chord 18
#define Contralto 19
#define CounterTenor 20
#define DYKFlat 21
#define DYKLH 22
#define DYKSharp 23
#define Fifth 24
#define FiveNotes 25
#define FlatMeans 26
#define FourNotes 27
#define Fourth 28
#define GoodScale 29
#define HighestNote 30
#define LowerNote 31
#define MajorScale 32
#define MezzSoprano 33
#define Octave 34
#define OrA 35
#define PlayAgain 36
#define SevenNotes 37
#define SharpMeans 38
#define SingChord 39
#define SingFifth 40
#define SingFourth 41
#define SingOctave 42
#define SingThird 43
#define SixNotes 44
#define Soprano 45
#define SoundLike 46
#define Tenor 47
#define Third 48
#define ThreeNotes 49
#define TwoNotes 50
#define WholeScale 51
#define CanYou 52


#endif 