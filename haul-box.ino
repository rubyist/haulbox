#include "outputter.h"
#include "button.h"

#define LED_PIN 18
int ledValue = 0;

byte rows[] = {8, 7, 6, 5, 4};
const int rowCount = sizeof(rows)/sizeof(rows[0]);

byte cols[] = {9, 10, 11, 12, 13};
const int colCount = sizeof(cols)/sizeof(cols[0]);

byte keys[colCount][rowCount];

Button *buttons[colCount][rowCount] = {
  { new Toggle(0), new Toggle(5), new PushButton(10), new PushButton(15), new Toggle(20) },
  { new Toggle(1), new Toggle(6), new PushButton(11), new Latch(16), new Toggle(21) },
  { new Toggle(2), new Toggle(7), new PushButton(12), new Toggle(17), new Toggle(22) },
  { new PushButton(3), new Toggle(8), new Toggle(13), new Toggle(18), new Toggle(23) },
  { new PushButton(4), new Toggle(9), new Toggle(14), new Toggle(19), new Toggle(24) },
};

// Rotary encoder
#define E1_CLK 2
#define E1_DATA 3
#define E1_UP_BTN 25
#define E1_DN_BTN 26

#define E2_CLK 0
#define E2_DATA 1
#define E2_UP_BTN 27
#define E2_DN_BTN 28

static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

volatile uint8_t prevNextCode1 = 0;
volatile uint16_t store1 = 0;
volatile uint8_t prevNextCode2 = 0;
volatile uint16_t store2 = 0;

// Rotary encoder directions are mapped to joystick button presses
Toggle *e1Up = new Toggle(E1_UP_BTN);
Toggle *e1Dn = new Toggle(E1_DN_BTN);
Toggle *e2Up = new Toggle(E2_UP_BTN);
Toggle *e2Dn = new Toggle(E2_DN_BTN);

// Change OUTPUT_JOYSTICK to OUTPUT_SERIAL for testing
Outputter output(OUTPUT_JOYSTICK, rowCount*colCount+4); // Account for rotary encoders acting as two buttons each

void setup() {
  pinMode(LED_PIN, OUTPUT);

  // Matrix setup
  for (int x=0; x < rowCount; x++) {
    pinMode(rows[x], INPUT);
  }

  for (int x = 0; x < colCount; x++) {
    pinMode(cols[x], INPUT_PULLUP);
  }

  // Rotary encoder setup
  pinMode(E1_CLK, INPUT_PULLUP);
  pinMode(E1_DATA, INPUT_PULLUP);
  pinMode(E2_CLK, INPUT_PULLUP);
  pinMode(E2_DATA, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(E1_CLK), fire_rotary1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(E1_DATA), fire_rotary1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(E2_CLK), fire_rotary2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(E2_DATA), fire_rotary2, CHANGE);

  output.begin();
}

// matrix reading code from https://www.baldengineer.com/arduino-keyboard-matrix-tutorial.html
void read_matrix() {
  // iterate the columns
  for (int colIndex = 0; colIndex < colCount; colIndex++) {
    // col: set to output to low
    byte curCol = cols[colIndex];
    pinMode(curCol, OUTPUT);
    digitalWrite(curCol, LOW);

    // row: iterate through the rows
    for (int rowIndex = 0; rowIndex < rowCount; rowIndex++) {
      byte rowCol = rows[rowIndex];
      pinMode(rowCol, INPUT_PULLUP);
      int v = !digitalRead(rowCol);
      if (v != keys[colIndex][rowIndex]) {
        // This mess is for the rotary switch. Only send events for the position
        // it's switching into, not the position it's leaving.
        int bId = buttons[colIndex][rowIndex]->id();
        if ((bId == 0 || bId == 1 || bId == 2 || bId == 5 || bId == 6 | bId == 7) && !v) {
          // special case rotary switch, only send events when they go high
        } else {
          buttons[colIndex][rowIndex]->activate(v, output);
        }
  


        // This mess is for disabling output if both rotary encoders are pressed
        // at the same time.
        if (rowIndex == 0 && (colIndex == 3 || colIndex == 4) && buttons[3][0]->state() && buttons[4][0]->state()) {
          // toggle output if both buttons 3 and 4 are pressed
          digitalWrite(LED_PIN, !ledValue);
          ledValue = !ledValue;
          output.toggle();
        }

        keys[colIndex][rowIndex] = v;
      }
      pinMode(rowCol, INPUT);
    }

    // disable the column
    pinMode(curCol, INPUT);
  }
}

void loop() {
  read_matrix();
  delay(5);
}

// Rotary debounce code from https://www.best-microcontroller-projects.com/rotary-encoder.html
void fire_rotary1() {
   if(read_rotary1()) {
      if ( prevNextCode1==0x0b) {
        e1Dn->activate(1, output);
      }

      if ( prevNextCode1==0x07) {
        e1Up->activate(1, output);
      }
   }
}

// A vald CW or CCW move returns 1, invalid returns 0.
int8_t read_rotary1() {
  prevNextCode1 <<= 2;
  if (digitalRead(E1_DATA)) prevNextCode1 |= 0x02;
  if (digitalRead(E1_CLK)) prevNextCode1 |= 0x01;
  prevNextCode1 &= 0x0f;

   // If valid then store as 16 bit data.
   if  (rot_enc_table[prevNextCode1] ) {
      store1 <<= 4;
      store1 |= prevNextCode1;
      if ((store1&0xff)==0x2b) return -1;
      if ((store1&0xff)==0x17) return 1;
   }
   return 0;
}

void fire_rotary2() {
   if(read_rotary2()) {
      if ( prevNextCode2==0x0b) {
        e2Dn->activate(1, output);
      }

      if ( prevNextCode2==0x07) {
         e2Up->activate(1, output);
      }
   }
}

// A vald CW or CCW move returns 1, invalid returns 0.
int8_t read_rotary2() {

  prevNextCode2 <<= 2;
  if (digitalRead(E2_DATA)) prevNextCode2 |= 0x02;
  if (digitalRead(E2_CLK)) prevNextCode2 |= 0x01;
  prevNextCode2 &= 0x0f;

   // If valid then store as 16 bit data.
   if  (rot_enc_table[prevNextCode2] ) {
      store2 <<= 4;
      store2 |= prevNextCode2;
      if ((store2&0xff)==0x2b) return -1;
      if ((store2&0xff)==0x17) return 1;
   }
   return 0;
}