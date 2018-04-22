//Set this depending on environment
#define arduino true
#if !arduino
#include <iostream>
#include "LedControl.h"
#else
#include <LedControl.h>
#endif
#define delayTime = 1000

#if !arduino
using namespace std;
int main()
{
  cout << "Hello world!" << endl;
  return 0;
}
#endif

enum LedState
{
  LED_ON = 1,
  LED_OFF = 0,
  LED_INTERMITENT = 2
};
static const char row1 = 2;
static const char row2 = 3;
static const char row3 = 4;
static const char row4 = 5;
static const char colA = 6;
static const char colB = 7;
static const char colC = 8;
static const char colD = 9;
char ships[] = {4, 3, 3, 2, 2};
class ButtonListener
{
  char pressedButtonValue = -1;
  char noKey = 0;
  boolean readKey = false;
  boolean buttonPressed = false;

public:
  bool listen()
  {
    Serial.println("ButtonListener:listen; ");
    if (this->noKey == 16)
    {                       // no keys were pressed
      this->readKey = true; // keyboard is ready to accept a new keypress
    }
    this->noKey = 0;
    char rowCounter = 0;
    char columnCounter = 0; // column counter
    for (rowCounter = row1; rowCounter < (row4 + 1); rowCounter++)
    {
      scanRow(rowCounter); // switch on one row at a time
      for (columnCounter = colA; columnCounter < colD + 1; columnCounter++)
      {
        if (readColumn(columnCounter)) // read the switch pressed
        {                              //the button has been pressed
          this->pressedButtonValue = (rowCounter - row1) + 4 * (columnCounter - colA);
        }
        else
        {
          this->noKey++; // counter for number of empty columns
        }
      }
    }
    if (this->readKey == true && this->noKey == 15)
    {
      // a key has been pressed
      Serial.println("ButtonListener:listen; a button has been pressed:");
      Serial.println(this->pressedButtonValue);
      this->buttonPressed = true;
      this->readKey = false; // rest the flag
    }
  }

  boolean buttonHasBeenPressed()
  {
    Serial.println("ButtonListener:buttonHasBeenPressed; returning ");
    Serial.println(this->buttonPressed);
    if (this->buttonPressed)
    {
      this->buttonPressed = false;
      return true;
    }
    else
      return false;
  }

  char getPressedButton()
  {
    Serial.println("ButtonListener:getPressedButton ");
    return this->pressedButtonValue;
  }

  void setup()
  {
    Serial.println("ButtonListener:setup");
    Serial.begin(9600);
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
};

class GameHelper
{
public:
  void initializeMatrix(char **matrix)
  {
    Serial.println("GameHelper:initializeMatrix;");
    char i, j;
    for (i = 0; i < 8; i++)
      for (j = 0; j < 8; j++)
        matrix[i][j] = 0;
  }
  void generateShips(char **matrix)
  {
    Serial.println("GameHelper:generateShips;");
    char x = 0, y = 0, flag, j, v;
    for (char i = 0; i < 5; i++)
    {
      flag = 0;
      v = random() % 2;
      /* 0 - horizontal
        * 1 - vertical
        */
      if (v)
      {
        while (!flag)
        {
          x = random() % (8 - ships[i]);
          y = random() % 8;
          if (matrix[x][y])
            /* (x,y) position already taken */
            continue;
          else
          {
            /* fill the column*/
            char temp = x;
            flag = 1;
            for (j = 0; j < ships[i]; j++, temp++)
            {
              if (matrix[temp][y])
                flag = 0;
            }
            if (!flag)
              continue;
            for (j = 0; j < ships[i]; j++, x++)
              matrix[x][y] = LED_ON;
          }
        }
      }
      else
      {
        while (!flag)
        {
          x = random() % 8;
          y = random() % (8 - ships[i]);
          if (matrix[x][y])
            /* (x,y) position already taken */
            continue;
          else
          {
            /* fill the row*/
            char temp = y;
            flag = 1;
            for (j = 0; j < ships[i]; j++, temp++)
            {
              if (matrix[x][temp])
                flag = 0;
            }
            if (!flag)
              continue;
            for (j = 0; j < ships[i]; j++, y++)
              matrix[x][y] = LED_ON;
          }
        }
      }
    }
  }
  char **create2DArray(char height, char width)
  {
    Serial.println("Player:create2DArray; h, w = ");
    Serial.print(height);
    Serial.print(", ");
    Serial.println(width);
    char **array2D = 0;
    array2D = new char *[height];

    for (char h = 0; h < height; h++)
    {
      array2D[h] = new char[width];

      for (char w = 0; w < width; w++)
      {
        // fill in some initial values
        // (filling in zeros would be more logic, but this is just for the example)
        array2D[h][w] = w + width * h;
      }
    }

    return array2D;
  }
};

class Cursor
{
public:
  char row = 0;
  char column = 0;
  char oldRow = 0;
  char oldColumn = 0;
  bool cursorHasMoved = 0;
  Cursor()
  {
    Serial.println("Cursor:Cursor;");
  }
  Cursor(char rowN, char colN)
  {
    Serial.println("Cursor:Cursor;");
    this->row = rowN;
    this->column = colN;
    this->oldRow = rowN;
    this->oldColumn = rowN;
  }
  //MODE = 0 => cursor at border will not move farther
  //MODE = 1 => cursor at border will move circularry
  bool moveLeft(char mode)
  {
    Serial.println("Cursor:moveLeft;");
    if (mode == 0)
    {
      if (this->column > 0)
      {
        this->oldColumn = this->column;
        this->column--;
      }
      else
        return false;
    }
    else
    {
      if (this->column > 0)
      {
        this->oldColumn = this->column;
        this->column--;
      }
      else
      {
        this->oldColumn = this->column;
        this->column = 7;
      }
    }
    this->cursorHasMoved = true;
    return true;
  }
  bool moveRight(char mode)
  {
    Serial.println("Cursor:moveRight;");
    if (mode == 0)
    {
      if (this->column < 7)
      {
        this->oldColumn = this->column;
        this->column++;
      }
      else
        return false;
    }
    else
    {
      if (this->column < 7)
      {
        this->oldColumn = this->column;
        this->column++;
      }
      else
      {
        this->oldColumn = this->column;
        this->column = 0;
      }
    }
    this->cursorHasMoved = true;
    return true;
  }
  bool moveUp(char mode)
  {
    Serial.println("Cursor:moveUp;");
    if (mode == 0)
    {
      if (this->row < 7)
      {
        this->oldRow = this->row;
        this->row++;
      }
      else
        return false;
    }
    else
    {
      if (this->row < 7)
      {
        this->oldRow = this->row;
        this->row++;
      }
      else
      {
        this->oldRow = this->row;
        this->row = 0;
      }
    }
    this->cursorHasMoved = true;
    return true;
  }
  bool moveDown(char mode)
  {
    Serial.println("Cursor:moveDown;");
    if (mode == 0)
    {
      if (this->row > 0)
      {
        this->oldRow = this->row;
        this->row--;
      }
      else
        return false;
    }
    else
    {
      if (this->row > 0)
      {
        this->oldRow = this->row;
        this->row--;
      }
      else
      {
        this->oldRow = this->row;
        this->row = 7;
      }
    }
    this->cursorHasMoved = true;
    return true;
  }
};

class Player
{
public:
  char **myShipsMatrix;
  char **allMyShots;
  char **allMyHits;
  char **allEnemyShots;
  char **allEnemyHits;
  Cursor cursor;
  char currentPageNumber;
  bool currentPageHasChanged;
  bool isAHit(char rowNumber, char columnNumber)
  {
    Serial.println("Player:isAHit; row, column = ");
    Serial.print(rowNumber);
    Serial.print(", ");
    Serial.println(columnNumber);
    return (this->myShipsMatrix[rowNumber][columnNumber] == LED_ON);
  }
  bool beingAttacked(char rowNumber, char columnNumber)
  {
    Serial.println("Player:beingAttacked; row, column = ");
    Serial.print(rowNumber);
    Serial.print(", ");
    Serial.println(columnNumber);
    //TODO instead of returning <bool>, return a custom struct
    //with another info, like: a ship was destroyed, and ship's coords
    this->allEnemyShots[rowNumber][columnNumber] = LED_ON;
    if (isAHit(rowNumber, columnNumber))
    {
      this->allEnemyHits[rowNumber][columnNumber] = LED_ON;
      return true;
    }
    return false;
  }
  //Returns if a ship was completly destroied (UNIMPLEMENTED)
  bool hasJustAttacked(char rowNumber, char columnNumber, bool isHit)
  {
    Serial.println("Player:beingAttacked; row, column, isHit = ");
    Serial.print(rowNumber);
    Serial.print(", ");
    Serial.print(columnNumber);
    Serial.print(", ");
    Serial.println(isHit);
    //TODO save ships coordonates in generateShips method and check if this shot destroyed
    //a ship or just damaged it. And change <isHit> arg with a custom struct
    this->allMyShots[rowNumber][columnNumber] = LED_ON;
    this->allMyHits[rowNumber][columnNumber] = LED_ON;
    return false;
  }
  Player(GameHelper gameHelper) : myShipsMatrix(gameHelper.create2DArray(8, 8)), allMyShots(gameHelper.create2DArray(8, 8)),
                                  allMyHits(gameHelper.create2DArray(8, 8)), allEnemyShots(gameHelper.create2DArray(8, 8)), 
                                  allEnemyHits(gameHelper.create2DArray(8, 8)),
                                  cursor(0, 0)
  {
    Serial.println("Player:Player");
    this->currentPageNumber = 0;
    gameHelper.initializeMatrix(this->myShipsMatrix);
    gameHelper.generateShips(this->myShipsMatrix);
    gameHelper.initializeMatrix(this->allMyShots);
    gameHelper.initializeMatrix(this->allMyHits);
    gameHelper.initializeMatrix(this->allEnemyShots);
    gameHelper.initializeMatrix(this->allEnemyHits);
    //this->cursor = new Cursor(0, 0);
  }

  char **getPage(char pageNumber)
  {
    Serial.println("Player:getPage");
    pageNumber = pageNumber % 5;
    switch (pageNumber)
    {
    case 0:
      return this->myShipsMatrix;
    case 1:
      return this->allMyShots;
    case 2:
      return this->allMyHits;
    case 3:
      return this->allEnemyShots;
    case 4:
      return this->allEnemyHits;
    }
    return NULL;
  }
  char **getCurrentPage()
  {
    Serial.println("Player:getCurrentPage");
    return this->getPage(currentPageNumber);
  }
  void switchPageLeft()
  {
    Serial.println("Player:switchPageLeft");
    this->currentPageNumber = (this->currentPageNumber - 1) % 5;
    this->currentPageHasChanged = true;
  }
  void switchPageRight()
  {
    Serial.println("Player:switchPageRight");
    this->currentPageNumber = (this->currentPageNumber + 1) % 5;
    this->currentPageHasChanged = true;
  }
  void changePage(char newPageNumber)
  {
    Serial.println("Player:changePage");
    this->currentPageNumber = newPageNumber % 5;
    this->currentPageHasChanged = true;
  }
};

/* Led controller for two MAX7219
 * Change 4th parameter to 2 !!!!!!!!!!!!!!
 * pin 12 -> DataIn
 * pin 11 -> CS
 * pin 10 -> CLK
 */
LedControl lc = LedControl(12, 10, 11, 2);
ButtonListener listener = ButtonListener();
GameHelper gameHelper = GameHelper();
Player players[2] = {Player(gameHelper), Player(gameHelper)};
Cursor currentCursor;
char currentPlayer = 0;
char **currentActiveMatrix;
char loserPlayer = -1;
bool gameStarted = false;
bool isCursorVisible = false;
char winnerCount = 0; //used for winner animation
char count;

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

void startGame(char firstPlayer);
void buttonPressed(char pressedButton);
void displayWinnerMessage(char winner);
void displayLoserMessage(char loser);
void displayStaticMatrix(char **matrixToDisplay, char deviceId);
bool isCursorActiveOnPage(char pageNumber);
void renderIntermitents(char **matrixToDisplay, char deviceId);
void displayDynamicMatrix(char **matrixToDisplay, char deviceId, bool value);
void makeHitAnimation(char deviceId, char rowNumber, char columnNumber);
void changeCurrentPlayer(char newPlayerNumber);
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
  listener.setup();
  startGame(-1);
}
void loop()
{
  delay(200);
  Serial.println("Main:loop");
  //The following check could be used to delay the game start until one player presses a button
  //and to support successive games
  if (!gameStarted)
  {
    if (loserPlayer > -1)
    {
      displayWinnerMessage((loserPlayer + 1) % 2);
      displayLoserMessage(loserPlayer);
      if (winnerCount > 1000)
        startGame((loserPlayer + 1) % 2);
    }
    else
    {
      Serial.println("Main:Loop; game is not started and there is no animation");
      return;
    }
  }
  listener.listen();
  if (listener.buttonHasBeenPressed())
  {
    buttonPressed(listener.getPressedButton());
  }
  if (players[currentPlayer].currentPageHasChanged)
  {
    players[currentPlayer].currentPageHasChanged = false;
    currentActiveMatrix = players[currentPlayer].getCurrentPage();
    displayStaticMatrix(currentActiveMatrix, currentPlayer);
    isCursorVisible = isCursorActiveOnPage(players[currentPlayer].currentPageNumber);
  }
  if (currentCursor.cursorHasMoved)
  {
    //After the cursor has moved, restore the led from the previous cursor position to it's initial state
    //Otherwise, it's state will be lost
    currentCursor.cursorHasMoved = false;
    char r = currentCursor.oldRow;
    char c = currentCursor.oldColumn;
    if (currentActiveMatrix[r][c] == LED_ON)
      lc.setLed(currentPlayer, r, c, true);
    if (currentActiveMatrix[r][c] == LED_OFF)
      lc.setLed(currentPlayer, r, c, false);
    //We don't need to set a led with state euqal to LED_INTERMITENT, since it will be activated/deactivatet
    //in renderIntermitents() method
  }
  renderIntermitents(currentActiveMatrix, currentPlayer);
}

void displayWinnerMessage(char winner)
{
  Serial.println("Main:displayWinnerMessage; winner = ");
  Serial.println(winner);
  winnerCount++;
  for (char i = 0; i < 8; i++)
    lc.setRow(winner, i, true);
}
void displayLoserMessage(char loser)
{
  Serial.println("Main:displayLoserMessage");
  Serial.println(loser);
  for (char i = 0; i < 8; i++)
    lc.setRow(loser, i, false);
}
void renderIntermitents(char **matrixToDisplay, char deviceId)
{
  Serial.println("Main:renderIntermitents; deviceId = ");
  Serial.println(deviceId);
  count++;
  if (count == 50)
  {
    displayDynamicMatrix(matrixToDisplay, deviceId, true);
  }
  else if (count == 100)
  {
    count = 0;
    displayDynamicMatrix(matrixToDisplay, deviceId, false);
  }
}
void displayDynamicMatrix(char **matrixToDisplay, char deviceId, bool value)
{
  Serial.println("Main:displayDynamicMatrix; deviceId, value = ");
  Serial.print(deviceId);
  Serial.print(", ");
      Serial.println(value);
  char i, j;
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
    {
      if (matrixToDisplay[i][j] == LED_INTERMITENT)
        lc.setLed(deviceId, i, j, value);
    }
  }
  //display cursor
  if (isCursorVisible)
    lc.setLed(deviceId, currentCursor.row, currentCursor.column, value);
}

void displayStaticMatrix(char **matrixToDisplay, char deviceId)
{
  Serial.println("Main:displayStaticMatrix");
  char i, j;
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
    {
      if (matrixToDisplay[i][j] == LED_ON)
        lc.setLed(deviceId, i, j, true);
      if (matrixToDisplay[i][j] == LED_OFF)
        lc.setLed(deviceId, i, j, false);
    }
  }
}
bool isCursorActiveOnPage(char pageNumber)
{
  Serial.println("Main:isCursorActiveOnPage; pageNumber = ");
  Serial.println(pageNumber);
  Serial.print("Main:isCursorActiveOnPage; returning ");
  Serial.println(pageNumber == 1);
  if (pageNumber == 1)
    return true;
  return false;
}
void playerHasRetreated(char loser)
{
  Serial.println("Main:playerHasRetreated; loser = ");
  Serial.println(loser);
  //TODO display something to show that the game has ended
  gameStarted = false;
  loserPlayer = loser;
}
void startGame(char firstPlayer)
{
  Serial.println("Main:startGame; firstPlayer = ");
  Serial.println(firstPlayer);
  players[0] = Player(gameHelper);
  players[1] = Player(gameHelper);
  if (firstPlayer != 0 && firstPlayer != 1)
    currentPlayer = random() % 2;
  else
    currentPlayer = firstPlayer;
  currentCursor = players[currentPlayer].cursor;
  isCursorVisible = false;
  //Show ships to both players
  displayStaticMatrix(players[0].myShipsMatrix, 0);
  displayStaticMatrix(players[1].myShipsMatrix, 1);
  count = 0;
  winnerCount = 0;
  loserPlayer = -1;
  currentActiveMatrix = players[currentPlayer].getPage(0);
  //Maybe display smth to show who is the first player
  gameStarted = true;
}
void moveCursorLeft()
{
  Serial.println("Main:moveCursorLeft;");
  if (isCursorVisible)
    currentCursor.moveLeft(1);
}
void moveCursorRight()
{
  Serial.println("Main:moveCursorRight;");
  if (isCursorVisible)
    currentCursor.moveRight(1);
}
void moveCursorUp()
{
  Serial.println("Main:moveCursorUp;");
  if (isCursorVisible)
    currentCursor.moveUp(1);
}
void moveCursorDown()
{
  Serial.println("Main:moveCursorDown;");
  if (isCursorVisible)
    currentCursor.moveDown(1);
}
void switchPageLeft()
{
  Serial.println("Main:switchPageLeft;");
  players[currentPlayer].switchPageLeft();
}
void switchPageRight()
{
  Serial.println("Main:switchPageRight;");
  players[currentPlayer].switchPageLeft();
}
void retreat()
{
  Serial.println("Main:retreat;");
  playerHasRetreated(currentPlayer);
}
void enter()
{
  Serial.println("Main:enter;rowNumber, columnNumber =");
  if (!isCursorVisible)
    return;
  char rowNumber = currentCursor.row;
  char columnNumber = currentCursor.column;
  Serial.print(rowNumber);
  Serial.print(", ");
  Serial.println(columnNumber);
  Player enemy = players[(currentPlayer + 1) % 2];
  bool isAHit = enemy.beingAttacked(rowNumber, columnNumber);
  if (isAHit)
  {
    Serial.println("Main:enter;rowNumber, isAHit = ");
    Serial.println(isAHit);
    //There is a hit
    makeHitAnimation(currentPlayer, rowNumber, columnNumber);
    players[currentPlayer].hasJustAttacked(rowNumber, columnNumber, true);
    //Since it's a hit, we don't change the player
    return;
  }
  players[currentPlayer].hasJustAttacked(rowNumber, columnNumber, false);
  //Change the player
  changeCurrentPlayer((currentPlayer + 1) % 2);
}

void changeCurrentPlayer(char newPlayerNumber)
{
  Serial.println("Main:changeCurrentPlayer; currentPlayerNumber =");
  Serial.println(newPlayerNumber);
  currentPlayer = newPlayerNumber;
  currentCursor = players[currentPlayer].cursor;
  currentActiveMatrix = players[currentPlayer].getCurrentPage();
  isCursorVisible = isCursorActiveOnPage(players[currentPlayer].currentPageNumber);
}

void makeHitAnimation(char deviceId, char rowNumber, char columnNumber)
{
  Serial.println("Main:makeHitAnimation;");
  bool value = true;
  char i;
  for (i = 0; i < 9; i++)
  {
    lc.setLed(deviceId, rowNumber, columnNumber, value);
    value = !value;
    delay(300);
  }
}
void buttonPressed(char pressedBttn)
{
  Serial.println("Main:buttonPressed;");
  char pressedButton = pressedBttn;
  switch (pressedButton)
  {
  case moveCursorLeftAction:
  {
    moveCursorLeft();
    break;
  }
  case moveCursorRightAction:
  {
    moveCursorRight();
    break;
  }
  case moveCursorUpAction:
  {
    moveCursorUp();
    break;
  }
  case moveCursorDownAction:
  {
    moveCursorDown();
    break;
  }
  case switchPageLeftAction:
  {
    switchPageLeft();
    break;
  }
  case switchPageRightAction:
  {
    switchPageRight();
    break;
  }
  case retreatAction:
  {
    retreat();
    break;
  }
  case enterAction:
  {
    enter();
    break;
  }
  default:
  {
    break;
  }
  }
}
