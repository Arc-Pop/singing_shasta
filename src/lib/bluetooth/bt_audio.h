#ifndef BT_AUDIO_H
#define BT_AUDIO_H

// AUDIO_BUF_POOL_LEN is the len of pool that share usb audio to bt
//#define AUDIO_BUF_POOL_LEN 3072
#define AUDIO_BUF_POOL_LEN 512



// enable to show bt debug info
//#define HAVE_BTSTACK_STDIN

int btstack_main(int argc, const char * argv[]);

static void stdin_process(char cmd);

bool get_a2dp_connected_flag();

static bool a2dp_is_connected_flag = false;

void setCommand(char com);

char getCommand(void);

void playSine( double frequency );

int setSine( float frequency);

int getBTFlag(void);

void setAudioFlag(int flagSetting);

void setSineTime(int16_t time);

int getAudioFlag(void);

bool get_bt_audio_steam_ready(void);  // Just checks if it is ready

void set_shared_audio_buffer(int16_t *data); // Important, sets the audio buffer for playing audio

void bt_disconnect_and_scan(); // Disconnects the bluetooth, resets the counter

void bt_usb_resync_counter();

bool get_a2dp_connected_flag();

void a2dp_source_reconnect();

static void get_first_link_key(void);

void set_usb_buf_counter(uint16_t counter);

int get_bt_buf_counter(void);

void setFade(void);

static bool bt_audio_ready;
#endif 