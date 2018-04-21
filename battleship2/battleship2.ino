//Set this depending on environment
#define arduino true
#if !arduino
    #include <iostream>
    #include "LedControl.h"
#else
    #include <LedControl.h>
#endif
#define columnsNumber 8
#define rowsNumber 8
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
const char numberOfShips = 5;
char ships[] = {4, 3, 3, 2, 2};
char columnNumber = 8;
class ButtonListener {
  char pressedButton;
  public:
  bool checkPressedButton(){
    //TODO get pressed button and save it in pressedButton
    return false;
  }
  char getPressedButton(){
    return this->pressedButton;
  }
};

class GameHelper
{
public:
  void initializeMatrix(char **matrix)
  {
    char i, j;
    for (i = 0; i < 8; i++)
      for (j = 0; j < 8; j++)
        matrix[i][j] = 0;
  }
  void generateShips(char **matrix)
  {
    char x = 0, y = 0, flag, j, v;

    for (char i = 0; i < numberOfShips; i++)
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
          x = random() % (rowsNumber - ships[i]);
          y = random() % columnsNumber;
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
          x = random() % rowsNumber;
          y = random() % (columnsNumber - ships[i]);
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
};

class Cursor
{
public:
  char row;
  char column;
  char oldRow;
  char oldColumn;
  bool cursorHasMoved;
  Cursor() {
      this->row = 0;
      this->column = 0;
  }
  Cursor(char rowN, char colN)
  {
    this->row = rowN;
    this->column = colN;
    this->oldRow = rowN;
    this->oldColumn = rowN;
  }
  //MODE = 0 => cursor at border will not move farther
  //MODE = 1 => cursor at border will move circularry
  bool moveLeft(char mode)
  {
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
  void deleteMoveState()
  {
    this->cursorHasMoved = false;
  }
  bool moveRight(char mode)
  {
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
  bool beingAttacked(char rowNumber, char columnNumber) {
    //TODO instead of returning <bool>, return a custom struct
    //with another info, like: a ship was destroyed, and ship's coords
    this->allEnemyShots[rowNumber][columnNumber] = LED_ON;
    if(this->myShipsMatrix[rowsNumber][columnsNumber] == LED_ON) {
    this->allEnemyHits[rowNumber][columnNumber] = LED_ON;
    return true;
    }
    return false;
  }
  //Returns if a ship was completly destroied (UNIMPLEMENTED)
  bool hasJustAttacked(char rowNumber, char columnNumber, bool isHit) {
    //TODO save ships coordonates in generateShips method and check if this shot destroyed
    //a ship or just damaged it. And change <isHit> arg with a custom struct
    this->allMyShots[rowNumber][columnNumber]= LED_ON;
    this->allMyHits[rowNumber][columnNumber] = LED_ON;
    return false;
  }
  Player(GameHelper gameHelper) :  myShipsMatrix(create2DArray(8,8)), allMyShots(create2DArray(8,8)),
    allMyHits(create2DArray(8,8)), allEnemyShots(create2DArray(8,8)), allEnemyHits(create2DArray(8,8)),
    cursor(0,0)
  {
    this->currentPageNumber = 0;
    gameHelper.initializeMatrix(this->myShipsMatrix);
    gameHelper.generateShips(this->myShipsMatrix);
    gameHelper.initializeMatrix(this->allMyShots);
    gameHelper.initializeMatrix(this->allMyHits);
    gameHelper.initializeMatrix(this->allEnemyShots);
    gameHelper.initializeMatrix(this->allEnemyHits);
    //this->cursor = new Cursor(0, 0);
  }
  char** create2DArray(unsigned height, unsigned width)
    {
      char** array2D = 0;
      array2D = new char*[height];

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

char** getPage(char pageNumber) {
    pageNumber = pageNumber%5;
    switch(pageNumber) {
      case 0: return this->myShipsMatrix;
      case 1: return this->allMyShots;
      case 2: return this->allMyHits;
      case 3: return this->allEnemyShots;
      case 4: return this->allEnemyHits;
    }
    return NULL;
  }
  char** getCurrentPage() {
    return this->getPage(currentPageNumber);
  }
  void switchPageLeft() {
    this->currentPageNumber = (this->currentPageNumber - 1) % 5;
    this->currentPageHasChanged = true;
  }
  void switchPageRight() {
    this->currentPageNumber = (this->currentPageNumber + 1) % 5;
    this->currentPageHasChanged = true;
  }
  void changePage(char newPageNumber) {
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
Player players[2]={Player(gameHelper), Player(gameHelper)};
Cursor currentCursor;
char currentPlayer = 0;
char **currentActiveMatrix;
char loserPlayer = -1;
bool gameStarted = false;
bool isCursorVisible = false;
int winnerCount=0;
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
void displayStaticMatrix(char **matrixToDisplay,char deviceId);
bool isCursorActiveOnPage(char pageNumber);
void renderIntermitents(char **matrixToDisplay,char deviceId);
void displayDynamicMatrix(char **matrixToDisplay, char deviceId,bool value);
void makeHitAnimation(char deviceId, char rowNumber, char columnNumber);
void changeCurrentPlayer(char newPlayerNumber);
void displayStaticMatrix(char **matrixToDisplay,char deviceId);
void setup()
{
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
  startGame(-1);
}
void loop()
{
  delay(10);
  if (listener.checkPressedButton())
  {
    buttonPressed(listener.getPressedButton());
  }
  //The following check could be used to delay the game start until one player presses a button
  //and to support successive games
  if(!gameStarted) {
    if(loserPlayer > -1) {
      displayWinnerMessage((loserPlayer+1)%2);
      displayLoserMessage(loserPlayer);
      if(winnerCount >1000)
        startGame((loserPlayer+1)%2);
    } else return;
  }
  if(players[currentPlayer].currentPageHasChanged) {
    players[currentPlayer].currentPageHasChanged = false;
    currentActiveMatrix = players[currentPlayer].getCurrentPage();
    displayStaticMatrix(currentActiveMatrix, currentPlayer);
    isCursorVisible = isCursorActiveOnPage(players[currentPlayer].currentPageNumber);
  }
  if(currentCursor.cursorHasMoved) {
    //After the cursor has moved, restore the previous led to it's initial state
    //Otherwise, bacause cursor is intermitent, it's state would be lost
    currentCursor.cursorHasMoved = false;
    char r = currentCursor.oldRow;
    char c = currentCursor.oldColumn;
    if(currentActiveMatrix[r][c] == LED_ON ) lc.setLed(currentPlayer, r, c, true);
    if(currentActiveMatrix[r][c] == LED_OFF) lc.setLed(currentPlayer, r, c, false);
    //We don't need to set a led with state euqal to LED_INTERMITENT, since it will be activated/deactivatet
    //in renderIntermitents() method
  }
  renderIntermitents(currentActiveMatrix,currentPlayer);
}

void displayWinnerMessage(char winner) {
  winnerCount++;
  for(int i=0;i<8;i++)
  lc.setRow(winner,i,true);
}
void displayLoserMessage(char loser) {
  for(int i=0;i<8;i++)
  lc.setRow(loser,i,false);
}
void renderIntermitents(char **matrixToDisplay,char deviceId)
{
  count++;
  if(count == 50) {
    displayDynamicMatrix( matrixToDisplay, deviceId, true);
  } else if(count == 100) {
    count = 0;
    displayDynamicMatrix( matrixToDisplay, deviceId, false);
  }
}
void displayDynamicMatrix(char **matrixToDisplay, char deviceId,bool value) {
  char i,j;
  for(i=0;i<8;i++) {
    for(j=0;j<8;j++)
    {
      if(matrixToDisplay[i][j] == LED_INTERMITENT) lc.setLed(deviceId, i, j, value);
    }
  }
  //display cursor
  if(isCursorVisible) lc.setLed(deviceId, currentCursor.row,currentCursor.column, value);
}

void displayStaticMatrix(char **matrixToDisplay,char deviceId)
{
  char i, j;
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
    {
      if(matrixToDisplay[i][j] == LED_ON) lc.setLed(deviceId, i, j, true);
      if(matrixToDisplay[i][j] == LED_OFF) lc.setLed(deviceId,i,j, false);
    }
  }

}
bool isCursorActiveOnPage(char pageNumber) {
  if(pageNumber == 1) return true;
  return false;
}
void playerHasRetreated(char loser) {
  //TODO display something to show that the game has ended
  gameStarted = false;
  loserPlayer = loser;
}
void startGame(char firstPlayer)
{
  players[0] = Player(gameHelper);
  players[1] = Player(gameHelper);
  if(firstPlayer != 0 && firstPlayer != 1) currentPlayer = random() % 2;
  else currentPlayer = firstPlayer;
  currentCursor = players[currentPlayer].cursor;
  isCursorVisible = false;
  //Show ships to both players
  displayStaticMatrix(players[0].myShipsMatrix,0);
  displayStaticMatrix(players[1].myShipsMatrix,1);
  count = 0;
  winnerCount = 0;
  loserPlayer = -1;
  currentActiveMatrix = players[currentPlayer].getPage(0);
  //Maybe display smth to show who is the first player
  gameStarted = true;
}
void moveCursorLeft()
{
  if(isCursorVisible) currentCursor.moveLeft(1);
}
void moveCursorRight()
{
  if(isCursorVisible) currentCursor.moveRight(1);
}
void moveCursorUp()
{
  if(isCursorVisible) currentCursor.moveUp(1);
}
void moveCursorDown()
{
  if(isCursorVisible) currentCursor.moveDown(1);
}
void switchPageLeft()
{
  players[currentPlayer].switchPageLeft();
}
void switchPageRight()
{
  players[currentPlayer].switchPageLeft();
}
void retreat()
{
  playerHasRetreated(currentPlayer);
}
void enter()
{
  if(!isCursorVisible) return;
  char rowNumber = currentCursor.row;
  char columnNumber = currentCursor.column;
  Player enemy = players[(currentPlayer+1)%2];
  bool isAHit = enemy.beingAttacked(rowNumber, columnNumber);
  if(isAHit) {
    //There is a hit
    makeHitAnimation(currentPlayer,rowNumber, columnNumber);
    players[currentPlayer].hasJustAttacked(rowNumber, columnNumber, true);
    //Since it's a hit, we don't change the player
    return;
  }
  players[currentPlayer].hasJustAttacked(rowNumber, columnNumber, false);
  //Change the player
  changeCurrentPlayer((currentPlayer+1)%2);

}

void changeCurrentPlayer(char newPlayerNumber) {
  currentPlayer = newPlayerNumber;
  currentCursor = players[currentPlayer].cursor;
  currentActiveMatrix = players[currentPlayer].getCurrentPage();
}

void makeHitAnimation(char deviceId,char rowNumber,char columnNumber) {
  bool value = true;
  char i;
  for(i=0;i<9;i++)
  {
    lc.setLed(deviceId,rowNumber,columnNumber,value);
    value = !value;
    delay(300);
  }
}
void buttonPressed(char pressedBttn)
{
    int pressedButton = (int) pressedBttn;
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


