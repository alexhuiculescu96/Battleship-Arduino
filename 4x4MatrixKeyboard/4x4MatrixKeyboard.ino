/****************************************************
4x4 keyboard matrix

Connect:

Row 1 to Arduino pin 2
Row 2 to Arduino pin 3
Row 3 to Arduino pin 4
Row 4 to Arduino pin 5
Column A to Arduino pin 6
Column B to Arduino pin 7
Column C to Arduino pin 8
Column D to Arduino pin 9

Key identification:
(with connector at the top)
0 1 2 3
4 5 6 7
8 9 10 11
12 13 14 15

Connector:
Rows     Columns

X X X X  X X X X
1 2 3 4  A B C D
(Rows link keys from left to right,
columns link keys from top to bottom)
****************************************************/
#include <LedControl.h>

int debounce = 1; // set this to the lowest value that gives the best result
const int row1 = 2;
const int row2 = 3;
const int row3 = 4;
const int row4 = 5;
const int colA = 6;
const int colB = 7;
const int colC = 8;
const int colD = 9;
const int ledPin = 13; // onboard LED

LedControl lc = LedControl(12, 10, 11, 2);

void setup()
{
  Serial.begin(9600);
  pinMode(row1, OUTPUT);
  pinMode(row2, OUTPUT);
  pinMode(row3, OUTPUT);
  pinMode(row4, OUTPUT);
  pinMode(colA, INPUT_PULLUP);
  pinMode(colB, INPUT_PULLUP);
  pinMode(colC, INPUT_PULLUP);
  pinMode(colD, INPUT_PULLUP);
  //
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // turn LED off

  lc.shutdown(0, false);
  lc.shutdown(1, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 1);
  lc.setIntensity(1, 1);
  /* and clear the display */
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  lc.setLed(0, 0, 0, true);
  lc.setLed(1, 0, 0, true);
}

int foundColumn = 0;
boolean foundCol = false;
int keyValue = 0;
int noKey = 0;
boolean readKey = false;
int x = 0;
int y = 0;

void loop()
{
  if (noKey == 16)
  {                 // no keys were pressed
    readKey = true; // keyboard is ready to accept a new keypress
  }
  noKey = 0;
  int rowCounter = 0;
  int result;
  int columnCounter = 0; // column counter
  for (rowCounter = row1; rowCounter < (row4 + 1); rowCounter++)
  {
    scanRow(rowCounter); // switch on one row at a time
    for (columnCounter = colA; columnCounter < colD + 1; columnCounter++)
    {
      result = readColumn(columnCounter); // read the switch pressed
      if (result == true)
      {
        keyValue = (rowCounter - row1) + 4 * (columnCounter - colA);
      }
      else
      {
        noKey++; // counter for number of empty columns
      }
    }
  }
  if (readKey == true && noKey == 15)
  {                           // a key has been pressed
    Serial.println(keyValue); // used for debug
    if (keyValue == 1 && y < 7)
    {
        lc.setLed(0, x, y, false);
        lc.setLed(1, x, y, false);
        y++;
        lc.setLed(0, x, y, true);
        lc.setLed(1, x, y, true);
    }
    else if (keyValue == 2 && x < 7)
    {
        lc.setLed(0, x, y, false);
        lc.setLed(1, x, y, false);
        x++;
        lc.setLed(0, x, y, true);
        lc.setLed(1, x, y, true);
    }
    else if (keyValue == 3 && y > 0)
    {
        lc.setLed(0, x, y, false);
        lc.setLed(1, x, y, false);
        y--;
        lc.setLed(0, x, y, true);
        lc.setLed(1, x, y, true);
    }
    else if (keyValue == 6 && x > 0)
    {
        lc.setLed(0, x, y, false);
        lc.setLed(1, x, y, false);
        x--;
        lc.setLed(0, x, y, true);
        lc.setLed(1, x, y, true);
    }
    //*******************************************************
    readKey = false; // rest the flag
    delay(debounce); // debounce
  }
}

void scanRow(int rowNumber)
{
  for (int j = row1; j < (row4 + 1); j++)
  {
    digitalWrite(j, HIGH);
  }
  digitalWrite(rowNumber, LOW); // switch on one row
}

bool readColumn(int columnNumber)
{
  foundColumn = digitalRead(columnNumber);
  int response = foundColumn;
  if (response == 0)
  {
    foundCol = true;
    return true;
  }
  else
  {
    foundCol = false;
    return false;
  }
}
