//FINAL CODE
#include "simpletools.h"
serial *lcd;// Initializing LCD
const int ON = 22;
const int CLR = 12;
const int CRT = 13;

#define MAX_NOTES 30 // Increased to accommodate more notes
#define BTN_RECORD 8 // Button to start and stop the record
#define BTN_PLAYBACK 9//Button to play, pause and stop the Recorded Notes


void recordNotes(char* recordedNotes, int* noteDurations);// Function to record notes
void playBack(char* recordedNotes, int* noteDurations, int* leds);//Function to play back the recorded notes
void normalPlay(char* recordedNotes);// Function to playnotes
void led(void *par1);//cog for lighting up the LEDs while playnotes and recording them
//void led1(void* par2);
static volatile int leds[6] = {11, 13, 14, 15, 16, 17};// Initializing LED array 
static volatile char notes[8] = {'c','D','E','F','G','A','B','C'};// Initializing Notes Array
int noteFrequencies[8] = {2093,2349,2637,2793,3136,3520,3951,4186};// Initializing Frequency Array
static volatile char recordedNotes[MAX_NOTES];
unsigned int stack1[128 + 128];//Stack variables for cog 1


int main() {
  cogstart(led, NULL, stack1, sizeof(stack1));// Initiating cog 1
  
  lcd = serial_open(12, 12, 0, 9600); // Initialize the LCD
  writeChar(lcd, ON); // Turn on the LCD
  writeChar(lcd, CLR); // Clear the LCD
  pause(10); 
  
  volatile int buttonState_1;// Record Button State
  volatile int buttonState_2;// Playback Button State
    
  int noteDurations[MAX_NOTES]; 
  
  for(int i = 0; i < 6; i++) {
    low(leds[i]); // Initialize LEDs to low
  }
  while(1){
    buttonState_1 = input(BTN_RECORD);
    buttonState_2 = input(BTN_PLAYBACK);
    if (buttonState_1 == 1 && buttonState_2 == 0) {
      writeChar(lcd, CLR); // Clear the LCD
      writeChar(lcd, ON); // Turn on the LCD
      dprint(lcd, "Recording..\n");
      pause(500); // Pause for readability
      writeChar(lcd, CRT); // Move cursor to the second line

      // Clear recorded notes and durations
      memset(recordedNotes, 0, sizeof(char) * MAX_NOTES);
      memset(noteDurations, 0, sizeof(int) * MAX_NOTES);

      // recordNotesfunction Call
      recordNotes(recordedNotes, noteDurations);
      writeChar(lcd, CLR);
      dprint(lcd, "Record Stopped\n");
      pause(500); // Pause for readability
    }
    if (buttonState_2 == 1 && buttonState_1 == 0) {
      writeChar(lcd, CLR); // Clear the LCD
      writeChar(lcd, ON); // Turn on the LCD
      dprint(lcd, "Playing..\n");
      pause(250); // Pause for readability
      writeChar(lcd, CRT); // Move cursor to the second line
      
      // playBack function Call 
      playBack(recordedNotes, noteDurations, leds);
      writeChar(lcd, CLR); // Clear the LCD
      pause(250); // Pause for readability
    }
    if(buttonState_2 == 0 && buttonState_1 == 0){
      writeChar(lcd, CLR); // Clear the LCD
      writeChar(lcd, ON); // Turn on the LCD
      pause(100); // Pause for readability
      writeChar(lcd, CRT); // Move cursor to the second line
      
      //normalPlay function call
      normalPlay(recordedNotes);
      
      writeChar(lcd, CLR); // Clear the LCD
      pause(100); // Pause for readability 
    }
  }    
  serial_close(lcd); // Close the serial connection
  return 0;
}
//function normalPlay
void normalPlay(char* recordedNotes){
  for (int i = 0; i < 8; i++) {
    // to check buttons pressed and playing
    if (input(i) == 1) {
      char note = ' ';
      switch (i) {
        case 0:
          note = 'c';
          break;
        case 1:
          note = 'D';
          break;
        case 2:
          note = 'E';
          break;
        case 3:
          note = 'F';
          break;
        case 4:
          note = 'G';
          break;
        case 5:
          note = 'A';
          break;
        case 6:
          note = 'B';
          break;
        case 7:
          note = 'C';
          break;
        }
      while(input(i)==1){
        // to play
        freqout(10, 100, noteFrequencies[i]);
        dprint(lcd, "Normal Play: %c\n", note);
      }                
    }
  }        
}
//function recordNotes
void recordNotes(char* recordedNotes, int* noteDurations){
  volatile int index = 0;
  volatile int lastButtonState[8] = {0};  // Array to store the last state of each button
  volatile int buttonPressStart[8] = {0}; // Array to store the start time of each button press
  int c;// variable to count number of times button pressed
  while (index < MAX_NOTES && input(BTN_RECORD) == 1) {
    int buttonState[8] = {input(0), input(1), input(2), input(3), input(4), input(5), input(6), input(7)}; // Array to store the current state of each button
    // Check if any button is pressed
    for (int i = 0; i < 8; i++) {
      if (buttonState[i] == 1 && lastButtonState[i] == 0) { // Button transitioned from released to pressed
        c= 0;
        // Display the note being recorded
        char note = ' ';
        switch (i) {
          case 0:
            note = 'c';
            break;
          case 1:
            note = 'D';
            break;
          case 2:
            note = 'E';
            break;
          case 3:
            note = 'F';
            break;
          case 4:
            note = 'G';
            break;
          case 5:
            note = 'A';
            break;
          case 6:
            note = 'B';
            break;
          case 7:
            note = 'C'; // Octave change, so we have another 'C'
            break;
        }
        while (input(i) == 1) { // Keep playing the note as long as the button is pressed
          c++;
          freqout(10, 100, noteFrequencies[i]); // Play the note continuously
        }
        dprint(lcd, "Note:%c\n", note);
        pause(250); // Pause for readability
        writeChar(lcd, CLR); // Clear the second line
        writeChar(lcd, CRT); // Move cursor to the second line
      } 
      else if (buttonState[i] == 0 && lastButtonState[i] == 1) { // Button transitioned from pressed to released
        int duration=c*100;//Duration of button pressed
        // Record the note corresponding to the button and its duration
        switch (i) {
          case 0:
            recordedNotes[index] = 'c';
            break;
          case 1:
            recordedNotes[index] = 'D';
            break;
          case 2:
            recordedNotes[index] = 'E';
            break;
          case 3:
            recordedNotes[index] = 'F';
            break;
          case 4:
            recordedNotes[index] = 'G';
            break;
          case 5:
            recordedNotes[index] = 'A';
            break;
          case 6:
            recordedNotes[index] = 'B';
            break;
          case 7:
            recordedNotes[index] = 'C'; // Octave change, so we have another 'C'
            break;
        }
        noteDurations[index] = duration; // Store the duration of the note
        index++;
        // Pause for a short duration to prevent accidental button presses
        pause(100);
      }
      // Update the last state of the button
      lastButtonState[i] = buttonState[i];
    }
  }
}
//function playBack
void playBack(char* recordedNotes, int* noteDurations, int* leds) {
  volatile int index = 0;
  volatile int playbackButtonState;// playBack button State
  while (index < MAX_NOTES && recordedNotes[index] != '\0') {
    playbackButtonState = input(BTN_PLAYBACK);
    if (playbackButtonState == 0) {
      dprint(lcd, "Paused\n");
      while (input(BTN_PLAYBACK) == 0) {
        pause(100); // Wait until the button is released to resume playback
      }
      dprint(lcd, "Resuming\n");
      pause(500); // Pause for readability
      writeChar(lcd, CLR); // Clear the second line
      writeChar(lcd, CRT); // Move cursor to the second line
     } 
     else {
      // Play note
      int noteDuration = noteDurations[index];
      char note = recordedNotes[index];
      switch (note) {
        case 'c':
          dprint(lcd, "Playing: c\n");
          freqout(10, noteDuration, 2093); // Play C note
          break;
        case 'D':
          dprint(lcd, "Playing: D\n");
          high(leds[0]);
          freqout(10, noteDuration,  2349);//Play D note
          low(leds[0]);
          break;
        case 'E':
          dprint(lcd, "Playing: E\n");
          high(leds[1]);
          freqout(10, noteDuration, 2637); // Play E note 
          low(leds[1]);
          break;
        case 'F':
          dprint(lcd, "Playing: F\n");
          high(leds[2]);
          freqout(10, noteDuration, 2793); // Play F note 
          low(leds[2]);
          break;
        case 'G':
          dprint(lcd, "Playing: G\n");
          high(leds[3]);
          freqout(10, noteDuration,  3136); // Play G note 
          low(leds[3]); 
          break;
        case 'A':
          dprint(lcd, "Playing: A\n");
          high(leds[4]);
          freqout(10, noteDuration, 3520); // Play A note 
          low(leds[4]);
          break;
        case 'B':
          dprint(lcd, "Playing: B\n");
          high(leds[5]);
          freqout(10, noteDuration, 3951); // Play B note
          low(leds[5]);
          break;
        case 'C' : 
          dprint(lcd, "Playing: C\n");//Play C note
          freqout(10, noteDuration, 4186);
          break;
      }
      index++;
      pause(50); 
      writeChar(lcd, CLR); // Clear the second line
      writeChar(lcd, CRT); // Move cursor to the second line
      // Turn off all LEDs after note is played
      for(int i = 0; i < 6; i++) {
        low(leds[i]);
      }
    }
  }
   // Display "Playback Over!" after all recorded notes have been played
    dprint(lcd, "Playback Over!\n");
    pause(5000); 
   //Display "Starting over.." to start playing 
    dprint(lcd, "Starting over..\n");
    pause(5000); // Pause for 500 seconds before starting again
}
// cog for led lighting up
void led(void *par1){
  while(1){
    int buttonState[8] = {input(0), input(1), input(2), input(3), input(4), input(5), input(6), input(7)}; // Array to store the current state of each button
    for(int i = 1; i < 7; i++){
      if(buttonState[i] == 1){
        high(leds[i-1]);
      }
      else{
        low(leds[i-1]);
      }
    }
  }
} 

