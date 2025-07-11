Singing Shasta is a toy designed to teach children to sing by analyzing the frequency of their voice and giving them audio feedback responses.
The software is written in C for the Raspberry Pi Pico W which uses the BlueKitchen Bluetooth Libary to transmit the audio responses from the
microcontroller that is embedded in a toy microphone to a voice box embedded in a stuffed toy. The pico_fatfs_SD_Card library was used to read
the .wav audio files which are stored on a micro-SD card.

The Singing Shasta software was written by Jonathan Arc. Jonathan Nguyen wrote the implementation of the yin algorithm for pitch detection. 

Demo video: https://youtu.be/xY1jvm_y0n8
