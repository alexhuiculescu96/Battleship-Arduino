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
#include <LedControl.h>
LedControl lc = LedControl(12, 10, 11, 2);

//Method to generate ships for a single
void generateShipsForPlayer(byte mask);
//Method to generate ships at the start of the game
void generateShips();
//Method to start game
void startGame();
//Listen to key events
void listen();
//Setup the key event listener
void setupListener();
//Reads a column, return true if a button on that collons has been pressed
//Otherwise, returns false
bool readColumn(char columnNumber);
//Check enable one row to be checked for button presses
void scanRow(char rowNumber);
//Displays the matrix corresponding to <playerNumber/> on it's display
void displayView(byte playerNumber);
//Renders the ships for <playerNumber/> on it's display
void displayShips(byte playerNumber);
//Display the shots corresponding to <playerNumber/>
void displayPlayerShots(byte playerNumber);
//Display the hits corresponding to <playerNumber/>
void displayPlayerHits(byte playerNumber);
//Display  <playerNumber/> 's opponent shots
void displayOpponentShots(byte playerNumber);
//Generally displaying a view on a device
//Mask is used to retreive the wanted data from matrix
//For example: ships(P1 or P2), hits(P1 or P2),player shots, enemy shots
void displayMatrix(byte deviceId, byte mask);
//Does an animation: square for winner and cross for loser
void doEndGameAnimation();
//Called when an attack is initiated
void doAttack();

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

//Holds the data
byte matrix[8][8];
//Cursor position
byte cursorX;
byte cursorY;
//Cursor is visible only on some pages, depending on this field
byte cursorIsVisible;
//If cursor has moved, we will rerender the displays
bool cursorHasMoved;
byte currentPlayer;
//Holds the value of the button: 0-15
byte pressedButtonValue = -1;
//Used to check if any button has been pressed
byte noKey = 0;
bool readKey = false;
bool buttonPressed = false;
byte currentPageNumber[2]; // current page number for both players
byte cursorCount = -1;
bool pageHasChanged;
bool playerHasChanged;
byte endgame = 0;
byte playerHits[2];

void generateShipsForPlayer(byte mask)
{
  //Ships dimmensions
  byte ships[] = {4, 3, 3, 2, 2};
  Serial.print("\ngenerateShipsForPlayer: mask = ");
  Serial.println(mask);
  byte x = 0, y = 0, flag, j, orientation;
  //Iterate for each ship
  for (byte i = 0; i < 5; i++)
  {
    flag = 0;
    orientation = random() % 2; // 1 for horizontal, 0 for vertical
    if (orientation)
    {
      while (!flag)
      {
        //Start position for the ship
        x = random() % (8 - ships[i]);
        y = random() % 8;
        if (matrix[x][y] & mask)
          { 
            /* (x,y) position for player already taken */
            continue;
          }
        else
        {
          /* fill the column*/
          char temp = x;
          flag = 1;
          for (j = 0; j < ships[i]; j++, temp++)
          {
            if (matrix[temp][y] & mask)
              {
                //position is taken
                flag = 0;
              }
          }
          if (!flag)
            {
              continue;
            }
          for (j = 0; j < ships[i]; j++, x++)
            {
              matrix[x][y] = matrix[x][y] | mask; //add 1 on the corresponding bit
            }
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
            {
              Serial.print("*Position already taken");
              /* (x,y) position for player already taken */
              continue;
            }
        else
        {
          /* fill the row*/
          char temp = y;
          flag = 1;
          for (j = 0; j < ships[i]; j++, temp++)
          {
            if (matrix[x][temp] & mask)
              {
                flag = 0;
              }
          }
          if (!flag)
            {
              continue;
            }
          for (j = 0; j < ships[i]; j++, y++)
            {
              matrix[x][y] = matrix[x][y] | mask;
            }
        }
      }
    }
  }
  byte i;
  Serial.println("\tMatrix = "); //Print the matrix on serial
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
void setup()
{
  Serial.begin(9600);
  setupListener();
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
  randomSeed(analogRead(0));
  //Start the game with no default first player
  startGame(-1);
}
void displayViews() {
  if(!pageHasChanged && !cursorHasMoved && !playerHasChanged) 
    return;
  Serial.println("\ndisplayViews: displaying the views");
  //Display view for player 1
  displayView(1);
  //Display view for player 0
  displayView(0);
}

void doEndGameAnimation() {
  static byte alreadyDone; //we wil display the animation only once. No unnecesarry calls will be made
  if(alreadyDone) return;
  alreadyDone = 1;
  Serial.print("doEndGameAnimation;");
  byte loserPlayer = endgame - 1;
  byte winner = (loserPlayer + 1) % 2;
  lc.clearDisplay(loserPlayer);
  lc.setRow(loserPlayer, 3, B11111111);
  lc.setRow(loserPlayer, 4, B11111111);
  lc.setColumn(loserPlayer, 3, B11111111);
  lc.setColumn(loserPlayer, 4, B11111111);
  lc.clearDisplay((loserPlayer+1)%2);
  lc.setRow((loserPlayer+1)%2,0,B11111111);
  lc.setRow((loserPlayer+1)%2,1,B11111111);
  lc.setRow((loserPlayer+1)%2,6,B11111111);
  lc.setRow((loserPlayer+1)%2,7,B11111111);
  lc.setColumn((loserPlayer+1)%2, 0, B11111111);
  lc.setColumn((loserPlayer+1)%2, 1, B11111111);
  lc.setColumn((loserPlayer+1)%2, 6, B11111111);
  lc.setColumn((loserPlayer+1)%2, 7, B11111111);
}

void loop() {
  delay(10);
  if(endgame) {
    //Check if the game has ended
    doEndGameAnimation();
    return;
  }
  listen(); //Check for button presses
  if(buttonPressed) {
    buttonHasBeenPressed(); //Callback for button pressed
  }
  //First display view, after that display cursor
  //Otherwise, the view will override the cursor
  displayViews(); 
  displayCursor();
  //Set flags to default
  cursorHasMoved = false;
  buttonPressed = false;
  pageHasChanged = false;
  playerHasChanged = false;
}

void doAttack() {
  //do attack only on player's shots page (page 1 if starting at 0)
  if(currentPageNumber[currentPlayer] != 1 || !cursorIsVisible) 
    return;
  Serial.println("\ndoAttack");
  byte enemyPlayer = (currentPlayer + 1) % 2; //get enemy player number
  byte playerShotsMask = 4<<currentPlayer; // get player mask for shots
  byte enemyShipsMask = 1<< enemyPlayer; // get enemy mask for ships
  if(matrix[cursorX][cursorY] & playerShotsMask) {
    Serial.println("\n !!! Player already attacked at this position");
    //This position has already been attackef
    return;
  }
  matrix[cursorX][cursorY] = matrix[cursorX][cursorY] | playerShotsMask; // save the attack in the matrix
  if(matrix[cursorX][cursorY] & enemyShipsMask) { //The attack is a hit
  Serial.println("It's a HIT");
    playerHits[currentPlayer]++; //Increment the number of hits
    if(playerHits[currentPlayer] == 14) 
      endgame = enemyPlayer+1; //if the player has 14 hits, the enemy has lost the game
  } else {
  Serial.println("It's a MISS");
    //the attack is a miss
    currentPlayer = enemyPlayer;
    playerHasChanged = true;
  }
}

void buttonHasBeenPressed() {
  Serial.print("\nbuttonHasBeenPressed: buttonValue = ");
  Serial.println(pressedButtonValue);
  if (pressedButtonValue == enterAction) {//On enter action, attack
    doAttack();
  } else if (pressedButtonValue == moveCursorLeftAction) {
    cursorHasMoved = true;
    cursorX = cursorX == 0 ? 7 : cursorX - 1; //compute new cursor position
  } else if (pressedButtonValue == moveCursorDownAction) {
    cursorHasMoved = true;
    cursorY = cursorY == 0 ? 7 : cursorY - 1;//compute new cursor position
  } else if (pressedButtonValue == moveCursorRightAction) {
    cursorHasMoved = true;
    cursorX = (cursorX + 1) % 8;//compute new cursor position
  } else if (pressedButtonValue == moveCursorUpAction) {
    cursorHasMoved = true;
    cursorY = (cursorY + 1) % 8;//compute new cursor position
  } else if (pressedButtonValue == switchPageLeftAction) {
    pageHasChanged = true;
    //compute new page number
    currentPageNumber[currentPlayer] = currentPageNumber[currentPlayer] == 0 ? 3 : currentPageNumber[currentPlayer] - 1;
  } else if (pressedButtonValue == switchPageRightAction) {
    pageHasChanged = true;
    //compute new page number
    currentPageNumber[currentPlayer] = (currentPageNumber[currentPlayer] + 1) % 4; // 4 = number of pages
  } else if (pressedButtonValue == retreatAction) {
    endgame = 1 + currentPlayer; // automatically end the game
    cursorCount = 0;
    cursorIsVisible = false;
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
  if (cursorCount == 20) {
    lc.setLed(currentPlayer, cursorX, cursorY, false);
    return;
  }
  //When count > max, reset the counter
  if(cursorCount > 40 ) {
    cursorCount = -1;
    return;
  }
  //IF cursor has moved, display the cursor in it's new position with it's current state
  if(cursorHasMoved) {
    Serial.println("displayCursor: cursorHasMoved");
    if(cursorCount < 50) {
      //Cursor's state was ON, and will remain so
      lc.setLed(currentPlayer,cursorX,cursorY,true);
    } else {
      //Cursor's state was ON, and will remain so
      lc.setLed(currentPlayer,cursorX,cursorY,false);
    }

  }
}
//playerNumber = 0 or 1
void displayView(byte playerNumber) {
  byte pageNumber = currentPageNumber[playerNumber];
  byte willDisplay = 0;
  Serial.print("\n\tdisplayView: pageNumber = ");
  Serial.print(pageNumber);
  Serial.print(";\n\tplayerNumber = ");
  Serial.print(playerNumber);
  Serial.print("\n\tplayerHasChanged = ");
  Serial.print(playerHasChanged);
  Serial.print("\n\tcursorHasMoved = ");
  Serial.print(cursorHasMoved);
  Serial.print("\n\tpageHasChanged = ");
  Serial.print(pageHasChanged);
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
  Serial.print("\ndisplayShips; playerNumber = ");
  Serial.print(playerNumber);
  Serial.print("; mask = ");
  Serial.print(mask);
  displayMatrix(playerNumber, mask); //call displayMatrix with player's ship mask
}

void displayPlayerShots(byte playerNumber) {

  byte mask = 4 << playerNumber;
  Serial.print("\ndisplayPlayerShots; playerNumber = ");
  Serial.print(playerNumber);
  Serial.print("; player shots mask = ");
  Serial.print(mask);
  displayMatrix(playerNumber, mask); //call displayMatrix with player's shots mask
}

void displayPlayerHits(byte playerNumber) {
  byte i, j, playerShotsMask, opponentShipsMask;
  playerShotsMask = 4 << playerNumber; // get player's shots mask
  opponentShipsMask = 1 << ((1 + playerNumber) % 2); // get opponent's ship mask
  Serial.print("\ndisplayPlayerHits; playerNumber = ");
  Serial.print(playerNumber);
  Serial.print("; player shots mask = ");
  Serial.print(playerShotsMask);
  Serial.print("; opponentShipMask = ");
  Serial.println(opponentShipsMask);
  Serial.println("*Mask");
  for (i = 0; i < 8; i++) //iterate through the matrix
  {
    Serial.println("");
    for (j = 0; j < 8; j++)
    {
      //If player shot (i,j) position and oponent has a ship on that position
      if ((matrix[i][j] & playerShotsMask) && (matrix[i][j] & opponentShipsMask)) 
        {
          //player has a shot on this position and opponent has a ship
          Serial.print("1-");
          Serial.print(matrix[i][j]);
          Serial.print(' ');
          setLed(playerNumber, i, j, true);
        }
      else
        {
          Serial.print("1-");
          Serial.print(matrix[i][j]);
          Serial.print(' ');
          setLed(playerNumber, i, j, false);
        }
    }
  }
}

void displayOpponentShots(byte playerNumber) {
  //get corresponding masks
  byte opponentPlayer = (currentPlayer + 1) % 2;
  byte opponentMask = 4 << opponentPlayer;
  Serial.print("\ndisplayOpponentShots; playerNumber = ");
  Serial.print(playerNumber);
  Serial.print("; opponentMask = ");
  Serial.print(opponentMask);
  displayMatrix(playerNumber, opponentMask); // call displayMatrix with corresponding masks
}

void displayMatrix(byte deviceId, byte mask) {
  Serial.print("\ndisplayMatrix: deviceId = ");
  Serial.print(deviceId);
  Serial.print("; mask = ");
  Serial.print(mask);
  Serial.print("; \n*Matrix = ");
  
  byte i, j;
  for (i = 0; i < 8; i++)
  {
      Serial.println("");
    for (j = 0; j < 8; j++)
    {
      if (matrix[i][j] & mask) 
        {
          //Masking is true
          setLed(deviceId, i, j, true);
          Serial.print("1 ");
        }
      else
        { //Masking is false
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
  cursorIsVisible = true;
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
