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
bool playerHasChanged;

void generateShipsForPlayer(byte mask)
{
  byte ships[] = {4, 3, 3, 2, 2};
  Serial.print("\ngenerateShipsForPlayer: mask = ");
  Serial.println(mask);
  byte x = 0, y = 0, flag, j, orientation;
  for (byte i = 0; i < 5; i++)
  {
    Serial.print("ship number = ");
    Serial.print(i);
    flag = 0;
    orientation = random() % 2;
    Serial.print("; orientation = ");
    Serial.println(orientation);
    if (orientation)
    {
      while (!flag)
      {
        Serial.print("Starting while; flag is not set. Orientation is vertical");
        x = random() % (8 - ships[i]);
        y = random() % 8;
        Serial.print("*(x,y) = (");
        Serial.print(x);
        Serial.print(',');
        Serial.print(y);
        Serial.println(")");
        if (matrix[x][y] & mask)
          { 
            Serial.print("*Position already taken");
            /* (x,y) position for player already taken */
            continue;
          }
        else
        {
          Serial.println("*Position is not taken; filling the collumn");
          /* fill the column*/
          char temp = x;
          flag = 1;
          for (j = 0; j < ships[i]; j++, temp++)
          {
            if (matrix[temp][y] & mask)
              {
                Serial.println("**found a match at ");
                Serial.print(temp);
                Serial.print(",");
                Serial.println(y);
                flag = 0;
              }
          }
          if (!flag)
            {
              Serial.println("*could not fill the collumn");
              continue;
            }
          for (j = 0; j < ships[i]; j++, x++)
            {
              Serial.println("\n set on the following position: x = ");
              Serial.print(x);
              Serial.print("; y = ");
              Serial.println(y);
              matrix[x][y] = matrix[x][y] | mask; //add 1 on the corresponding bit
            }
        }
      }
    }
    else
    {
      while (!flag)
      {
        Serial.print("Starting while; flag is not set. Orientation is horizontal");
        x = random() % 8;
        y = random() % (8 - ships[i]);
        Serial.print("*(x,y) = (");
        Serial.print(x);
        Serial.print(',');
        Serial.print(y);
        Serial.println(")");
        if (matrix[x][y] & mask)
            {
              Serial.print("*Position already taken");
              /* (x,y) position for player already taken */
              continue;
            }
        else
        {
          Serial.println("*Position is not taken; filling the row");
          /* fill the row*/
          char temp = y;
          flag = 1;
          for (j = 0; j < ships[i]; j++, temp++)
          {
            if (matrix[x][temp] & mask)
              {
                Serial.println("**found a match at ");
                Serial.print(temp);
                Serial.print(",");
                Serial.println(y);
                flag = 0;
              }
          }
          if (!flag)
            {
              Serial.println("*could not fill the collumn");
              continue;
            }
          for (j = 0; j < ships[i]; j++, y++)
            {
              Serial.println("\n set on the following position: x = ");
              Serial.print(x);
              Serial.print("; y = ");
              Serial.println(y);
              matrix[x][y] = matrix[x][y] | mask;
            }
        }
      }
    }
  }
  byte i;
  Serial.println("\tMatrix = ");
  for(i = 0; i < 8 ; i ++) {
    Serial.println("");
    for(j = 0; j < 8; j++) {
      if(matrix[i][j] & mask)
        {
          Serial.print("1-");
          Serial.print(matrix[i][j]);
          Serial.print(" ");
        }
      else {
          Serial.print("0-");
          Serial.print(matrix[i][j]);
          Serial.print(" ");
      }
    }
  }
  Serial.println("");
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
  if(!pageHasChanged && !cursorHasMoved && !playerHasChanged) 
    return;
  //Display view for player 0
  displayView(0);
  //Display view for player 1
  displayView(1);
}

void loop() {
  delay(100);
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
  Serial.print("\nbuttonHasBeenPressed: buttonValue = ");
  Serial.println(pressedButtonValue);
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
    Serial.println("displayCursor: set cursor on");
    return;
  } 
  //Set led to OFF only when count == 50, to avoid unnecessary calls
  if (cursorCount == 50) {
    lc.setLed(currentPlayer, cursorX, cursorY, false);
    Serial.println("displayCursor: set cursor off");
    return;
  }
  //When count > 99, reset the counter
  if(cursorCount > 99 ) {
    cursorCount = 0;
    Serial.println("displayCursor: reset the counter");
    return;
  }
  //IF cursor has moved, display the cursor in it's new position with it's current value
  if(cursorHasMoved) {
    Serial.println("displayCursor: cursorHasMoved");
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
  byte willDisplay = 0;
  if(!playerHasChanged && !cursorHasMoved && !pageHasChanged)
    willDisplay = 0;
  if(playerHasChanged) {
    willDisplay = 1;
  } else if(cursorHasMoved && pageHasChanged) {
    willDisplay = (currentPlayer == playerNumber);
  }
  if(!willDisplay)
    return;
    Serial.println("displayView: playerNumber = ");
    Serial.print(playerNumber);
    Serial.print("; pageNumber = ");
    Serial.print(pageNumber);
  
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
  
  byte mask = 1 << playerNumber;
  Serial.println("displayShips; playerNumber = ");
  Serial.print(playerNumber);
  Serial.print("; mask = ");
  Serial.print(mask);
  displayMatrix(playerNumber, mask);
}

void displayPlayerShots(byte playerNumber) {

  byte mask = 4 << playerNumber;
  Serial.println("displayPlayerShots; playerNumber = ");
  Serial.print(playerNumber);
  Serial.print("; player shots mask = ");
  Serial.print(mask);
  displayMatrix(playerNumber, mask);
}

void displayPlayerHits(byte playerNumber) {
  
  byte i, j, playerShotsMask, opponentShipsMask;
  playerShotsMask = 4 << playerNumber;
  opponentShipsMask = 1 << ((1 + playerNumber) % 2);
  Serial.println("displayPlayerHits; playerNumber = ");
  Serial.print(playerNumber);
  Serial.print("; player shots mask = ");
  Serial.print(playerShotsMask);
  Serial.print("; opponentShipMask = ");
  Serial.print(opponentShipsMask);
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

  byte opponentPlayer = (currentPlayer + 1) % 2;
  byte opponentMask = 4 << opponentPlayer;
  Serial.println("displayOpponentShots; playerNumber = ");
  Serial.print(playerNumber);
  Serial.print("; opponentMask = ");
  Serial.print(opponentMask);
  displayMatrix(playerNumber, opponentMask);
}

void displayMatrix(byte deviceId, byte mask) {
  Serial.println("displayMatrix: deviceId = ");
  Serial.print(deviceId);
  Serial.print("; mask = ");
  Serial.print("mask; \n*Matrix = ");
  
  byte i, j;
  for (i = 0; i < 8; i++)
  {
      Serial.println("");
    for (j = 0; j < 8; j++)
    {
      if (matrix[i][j] & mask)
        {
          setLed(deviceId, i, j, true);
          Serial.print("1 ");
        }
      else
        {
          setLed(deviceId, i, j, false);
          Serial.print("0 ");
        }
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
  Serial.println("START game");
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
  //digitalRead returns 0 when button was pressed
  char foundColumn = digitalRead(columnNumber) ? false : true;
  return foundColumn;
}

void scanRow(char rowNumber)
{
  for (char j = row1; j < (row4 + 1); j++)
  {
    digitalWrite(j, HIGH);
  }
  digitalWrite(rowNumber, LOW); // switch on one row
}
