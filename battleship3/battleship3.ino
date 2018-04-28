/**
 *Matricile sunt compactate intr-o singura matrice, pentru 
 *eficienta memoriei 
 *  Un element din matrice are urmatoarea semnificatie
 *  [_,_,_,_,L2,L1,N2,N1] unde:
 *      N1 - pe aceasta pozitie, jucatorul 1 are nava
 *      L1 - pe aceasta pozitie, jucatorul 1 a lovit
 *      N2 - pe aceasta pozitie, jucatorul 2 are nava
 *      L2 - pe aceasta pozitie, jucatorul 2 a lovit
 * 
 * 
 **/
//#define byte char
#include <LedControl.h>
LedControl lc = LedControl(12, 10, 11, 2);

void generateShipsForPlayer(byte mask);
void generateShips();
void startGame();
void listen();
void setupListener();
bool readColumn(char columnNumber);
void scanRow(char rowNumber);
void displayView(byte playerNumber);
void displayShips(byte playerNumber);
void displayPlayerShots(byte playerNumber);
void displayPlayerHits(byte playerNumber);
void displayOpponentShots(byte playerNumber);
void displayMatrix(byte deviceId, byte mask);

enum action
{
  enterAction = 0,
  moveCursorLeftAction = 3,
  moveCursorRightAction = 1,
  moveCursorDownAction = 2,
  moveCursorUpAction = 6,
  switchPageLeftAction = 7,
  switchPageRightAction = 5,
  retreatAction = 4
};
static const char row1 = 2;
static const char row2 = 3;
static const char row3 = 4;
static const char row4 = 5;
static const char colA = 6;
static const char colB = 7;
static const char colC = 8;
static const char colD = 9;

byte matrix[8][8];
byte cursorX;
byte cursorY;
byte cursorIsVisible;
bool cursorHasMoved;
byte currentPlayer;
byte pressedButtonValue = -1;
byte noKey = 0;
bool readKey = false;
bool buttonPressed = false;
byte currentPageNumber[2]; // current page number for both players
byte cursorCount = -1;
bool pageHasChanged;

void generateShipsForPlayer(byte mask)
{
  byte ships[] = {4, 3, 3, 2, 2};
  Serial.println("generateShips");
  byte x = 0, y = 0, flag, j, orientation;
  for (byte i = 0; i < 5; i++)
  {
    flag = 0;
    orientation = random() % 2;
    if (orientation)
    {
      while (!flag)
      {
        x = random() % (8 - ships[i]);
        y = random() % 8;
        if (matrix[x][y] & mask)
          /* (x,y) position for player already taken */
          continue;
        else
        {
          /* fill the column*/
          char temp = x;
          flag = 1;
          for (j = 0; j < ships[i]; j++, temp++)
          {
            if (matrix[temp][y] & mask)
              flag = 0;
          }
          if (!flag)
            continue;
          for (j = 0; j < ships[i]; j++, x++)
            matrix[x][y] = matrix[x][y] | mask; //add 1 on first bit(MSB)
        }
      }
    }
    else
    {
      while (!flag)
      {
        x = random() % 8;
        y = random() % (8 - ships[i]);
        if (matrix[x][y] & mask)
          /* (x,y) position for player already taken */
          continue;
        else
        {
          /* fill the row*/
          char temp = y;
          flag = 1;
          for (j = 0; j < ships[i]; j++, temp++)
          {
            if (matrix[x][temp] & mask)
              flag = 0;
          }
          if (!flag)
            continue;
          for (j = 0; j < ships[i]; j++, y++)
            matrix[x][y] = matrix[x][y];
        }
      }
    }
  }
}

void generateShips()
{
  //mask = 1 for the first player
  generateShipsForPlayer(1);
  //mask = 2 for the second player
  generateShipsForPlayer(2);
}
#define arduino true
void setup()
{
  Serial.println("Main:setup");
  //ON ARDUINO: change "::" with '.'
  #if arduino
  Serial.begin(9600);
  #else
  Serial::begin(9600);
  #endif
  randomSeed(analogRead(0));
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0, false);
  lc.shutdown(1, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 1);
  lc.setIntensity(1, 1);
  /* and clear the display */
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  setupListener();
  startGame(-1);
}
void displayViews() {
  if(!pageHasChanged && !cursorHasMoved) 
    return;
  //Display view for player 0
  displayView(0);
  //Display view for player 1
  displayView(1);
}

void loop() {
  delay(300);
  listen();
  if(buttonPressed) {
    buttonHasBeenPressed();
  }
  displayCursor();
  displayViews();
  cursorHasMoved = false;
  buttonPressed = false;
  pageHasChanged = false;
}

void buttonHasBeenPressed() {
  if (pressedButtonValue == enterAction) {
    //TODO
  } else if (pressedButtonValue == moveCursorLeftAction) {
    cursorHasMoved = true;
    cursorX = cursorX ? 7 : cursorX - 1;
  } else if (pressedButtonValue == moveCursorDownAction) {
    cursorHasMoved = true;
    cursorY = cursorY ? 7 : cursorY - 1;
  } else if (pressedButtonValue == moveCursorRightAction) {
    cursorHasMoved = true;
    cursorX = (cursorX + 1) % 8;
  } else if (pressedButtonValue == moveCursorUpAction) {
    cursorHasMoved = true;
    cursorY = (cursorY + 1) % 8;
  } else if (pressedButtonValue == switchPageLeftAction) {
    pageHasChanged = true;
    currentPageNumber[currentPlayer] = currentPageNumber[currentPlayer] ? 3 : currentPageNumber[currentPlayer] - 1;
  } else if (pressedButtonValue == switchPageRightAction) {
    pageHasChanged = true;
    currentPageNumber[currentPlayer] = (currentPageNumber[currentPlayer] + 1) % 4; // 4 = number of pages
  } else if (pressedButtonValue == retreatAction) {
    //TODO
  }
}

void displayCursor() {
  cursorCount++;
  //Display only if cursor should be visible
  if( !cursorIsVisible ) return;
  //Set led to ON only when count == 0, to avoid unnecessary calls
  if(cursorCount == 0) {
    lc.setLed(currentPlayer,cursorX,cursorY,true);
    return;
  } 
  //Set led to OFF only when count == 50, to avoid unnecessary calls
  if (cursorCount == 50) {
    lc.setLed(currentPlayer, cursorX, cursorY, false);
    return;
  }
  //When count > 99, reset the counter
  if(cursorCount > 99 ) {
    cursorCount = 0;
    return;
  }
  //IF cursor has moved, display the cursor in it's new position with it's current value
  if(cursorHasMoved) {
    if(cursorCount < 50) {
      lc.setLed(currentPlayer,cursorX,cursorY,true);
    } else {
      lc.setLed(currentPlayer,cursorX,cursorY,false);
    }

  }
}
//playerNumber = 0 or 1
void displayView(byte playerNumber) {
  byte pageNumber = currentPageNumber[playerNumber];
  if (pageNumber == 0) {
    displayShips(playerNumber);
  } else if (pageNumber == 1) {
    displayPlayerShots(playerNumber);
  } else if (pageNumber == 2) {
    displayPlayerHits(playerNumber);
  } else if (pageNumber == 3) {
    displayOpponentShots(playerNumber);
  }
}

void displayShips(byte playerNumber) {
  Serial.println("displayShips");
  
  byte mask = 1 << playerNumber;
  displayMatrix(playerNumber, mask);
}

void displayPlayerShots(byte playerNumber) {
  Serial.println("displayPlayerShots");

  byte mask = 4 << playerNumber;
  displayMatrix(playerNumber, mask);
}

void displayPlayerHits(byte playerNumber) {
  Serial.println("displayPlayerHits");
  
  byte i, j, playerShotsMask, opponentShipsMask;
  playerShotsMask = 4 << playerNumber;
  opponentShipsMask = 1 << ((1 + playerNumber) % 2);
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
    {
      //If player shot (i,j) position and oponent has a ship on that position
      if ((matrix[i][j] & playerShotsMask) && (matrix[i][j] & opponentShipsMask))
        setLed(playerNumber, i, j, true);
      else
        setLed(playerNumber, i, j, false);
    }
  }
}

void displayOpponentShots(byte playerNumber) {
  Serial.println("displayOpponentShots");

  byte opponentPlayer = (currentPlayer + 1) % 2;
  byte opponentMask = 4 << opponentPlayer;
  displayMatrix(playerNumber, opponentMask);
}

void displayMatrix(byte deviceId, byte mask) {
  Serial.println("displayMatrix");
  
  byte i, j;
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
    {
      if (matrix[i][j] & mask)
        setLed(deviceId, i, j, true);
      else
        setLed(deviceId, i, j, false);
    }
  }
}

//Method used to avoid changing the led where the cursor sits,
// if it's visible, and only for the current player
void setLed(byte deviceId, byte row, byte column, bool value) {
  if(cursorIsVisible && row == cursorX && column == cursorY && deviceId == currentPlayer)
   return;
  lc.setLed(deviceId, row, column, value);
}


void startGame(byte firstPlayer)
{
  if(firstPlayer == 1 ) currentPlayer = 1;
  else currentPlayer == 0;
  currentPageNumber[0] = 0;
  currentPageNumber[1] = 0;
  generateShips();
  cursorIsVisible = false;
  pageHasChanged = true;
}
void listen()
{
  Serial.println("ButtonListener:listen; ");
  if (noKey == 16)
  {                 // no keys were pressed
    readKey = true; // keyboard is ready to accept a new keypress
  }
  noKey = 0;
  char rowCounter = 0;
  char columnCounter = 0; // column counter
  for (rowCounter = row1; rowCounter < (row4 + 1); rowCounter++)
  {
    scanRow(rowCounter); // switch on one row at a time
    for (columnCounter = colA; columnCounter < colD + 1; columnCounter++)
    {
      if (readColumn(columnCounter)) // read the switch pressed
      {                              //the button has been pressed
        pressedButtonValue = (rowCounter - row1) + 4 * (columnCounter - colA);
      }
      else
      {
        noKey++; // counter for number of empty columns
      }
    }
  }
  if (readKey == true && noKey == 15)
  {
    // a key has been pressed
    Serial.println("ButtonListener:listen; a button has been pressed:");
    Serial.println(pressedButtonValue);
    buttonPressed = true;
    readKey = false; // reset the flag
  }
}


void setupListener()
{
  Serial.println("ButtonListener:setup");
  pinMode(row1, OUTPUT);
  pinMode(row2, OUTPUT);
  pinMode(row3, OUTPUT);
  pinMode(row4, OUTPUT);
  pinMode(colA, INPUT_PULLUP);
  pinMode(colB, INPUT_PULLUP);
  pinMode(colC, INPUT_PULLUP);
  pinMode(colD, INPUT_PULLUP);
}

bool readColumn(char columnNumber)
{
  Serial.println("ButtonListener:readColumn; columnNumber = ");
  Serial.println(columnNumber);
  //digitalRead returns 0 when button was pressed
  char foundColumn = digitalRead(columnNumber) ? false : true;
  return foundColumn;
}

void scanRow(char rowNumber)
{
  Serial.println("ButtonListener:scanRow; rowNumber = ");
  Serial.println(rowNumber);
  for (char j = row1; j < (row4 + 1); j++)
  {
    digitalWrite(j, HIGH);
  }
  digitalWrite(rowNumber, LOW); // switch on one row
}