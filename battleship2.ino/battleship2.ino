#include <LedControl.h>
#define columnsNumber 8
#define rowsNumber 8
#define delayTime = 1000
enum LedState
{
  LED_ON = 1,
  LED_OFF = 0,
  LED_INTERMITENT = 2
}
/* Led controller for two MAX7219
 * Change 4th parameter to 2 !!!!!!!!!!!!!!
 * pin 12 -> DataIn
 * pin 11 -> CS
 * pin 10 -> CLK
 */
LedControl lc = LedControl(12, 10, 11, 2);
ButtonListener listener = ButtonListener();
Player players[2];
Cursor currentCursor;
char currentPlayer = 0;
char currentActiveMatrix[8][8];
char loserPlayer = -1;
const char numberOfShips = 5;
char ships[] = {4, 3, 3, 2, 2};
GameHelper gameHelper = new GameHelper();
bool gameStarted = false;
bool isCursorVisible = false;
int winnerCount=0;
char count;

static const struct actionType
{
  char enter = 0;
  char moveCursorLeft = 3;
  char moveCursorRight = 1;
  char moveCursorDown = 2;
  char moveCursorUp = 6;
  char switchPageLeft = 7;
  char switchPageRight = 5;
  char retreat = 4;
} action;

void setup()
{
  Serial.begin(9600);
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
displayWinnerMessage(char winner);
displayLoserMessage(char loser);
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
      if(winnerCount >1000) {
        startGame((loserPlayer+1)%2);
    } else return;
  }
  if(players[currentPlayer].currentPageHasChanged) {
    players[currentPlayer].currentPageHasChanged = false;
    currentActiveMatrix = players[currentPlayer].getCurrentPage();
    displayStaticMatrix(currentActiveMatrix, currentPlayer);
    isCursorVisible = isCursorActiveOnPage(players[currentPlayer].currentPage);
  }
  if(currentCursor.cursorHasMoved) {
    //After the cursor has moved, restore the previous led to it's initial state
    //Otherwise, bacause cursor is intermitent, it's state would be lost
    currentCursor.cursorHasMoved = false;
    char r = currentCursor.oldRow;
    char c = currentCursor.oldColumnl
    if(currentActiveMatrix[r][c] == LED_ON ) lc.setLed(deviceId, r, c, true);
    if(currentActiveMatrix[r][c] == LED_OFF) lc.setLed(deviceId, r, c, false);
    //We don't need to set a led with state euqal to LED_INTERMITENT, since it will be activated/deactivatet
    //in renderIntermitents() method
  }
  renderIntermitents(currentActiveMatrix,currentPlayer)
}
void displayWinnerMessage(char winner) {
  winnerCount++;
  for(i=0;i<8;i++)
  lc.setRow(winner,i,true);
}
void displayLoserMessage(char loser) {
  for(i=0;i<8;i++)
  lc.setRow(loser,i,false);
}
void renderIntermitents(char matrixToDisplay[8][8],char deviceId)
{
  count++;
  if(count == 50) {
    displayDynamicMatrix( matrixToDisplay[8][8], deviceId, true);
  } else if(count == 100) {
    count = 0;
    displayDynamicMatrix( matrixToDisplay[8][8], deviceId, false);
  }
}
void displayDynamicMatrix(char matrixToDisplay[8][8], char deviceId,bool value) {
  char i,j;
  for(i=0;i<8;i++) {
    for(j=0;j<8;j++)
    {
      if(matrixToDisplay[i][j] == LED_INTERMITENT) lc.set(deviceId, i, j, value);
    }
  }
  //display cursor
  if(isCursorVisible) lc.set(deviceId, currentCursor.row,currentCursor.column, value);
}

void displayStaticMatrix(char matrixToDisplay[rowsNumber][columnsNumber],char deviceId)
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
  players[0] = new Player();
  players[1] = new Player();
  if(firstPlayer != 0 && firstPlayer != 1) currentPlayer = rand() % 2;
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
  if(isCursorVisible) currentCursor.moveUp(1)
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
  char columnNumber = currentCursor.line;
  Player enemy = players[(currentPlayer+1)%2];
  bool isAHit = enemy.beingAttacked(rowNumber, columnNumber)
  if(isAHit) {
    //There is a hit
    makeHitAnimation(currentPlayer,rowNumber, columnNumber);
    players[currentPlayer].hasJustAttacked(rowNumber, columnNumber, true);
    //Since it's a hit, we don't change the player
    return;
  }
  players[currentPlayer].hasJustAttacked(rowNumber, columnNumber, true);
  //Change the player
  changeCurrentPlayer((currentPlayer+1)%2);

}

changeCurrentPlayer(char newPlayerNumber) {
  char oldPlayer = newPlayerNumber;
  currentPlayer = newPlayerNumber;
  currentCursor = currentPlayer.cursor;
  currentActiveMatrix = currentPlayer.getCurrentPage();
}

void makeHitAnimation(char deviceId, rowNumber, columnNumber) {
  bool value = true;
  char i;
  for(i=0;i<9;i++)
  {
    lc.setLed(deviceId,rowNumber,columnNumber,value);
    value = !value;
    delay(300);
  }
}
void buttonPressed(char pressedButton)
{
  switch (pressedButton)
  {
  case action.moveCursorLeft:
  {
    this.moveCursorLeft();
    break;
  }
  case action.moveCursorRight:
  {
    this.moveCursorRight();
    break;
  }
  case action.moveCursorUp:
  {
    this.moveCursorUp();
    break;
  }
  case action.moveCursorDown:
  {
    this.moveCursorDown();
    break;
  }
  case action.switchPageLeft:
  {
    this.switchPageLeft();
    break;
  }
  case action.switchPageRight:
  {
    this.switchPageRight();
    break;
  }
  case action.retreat:
  {
    this.retreat() break;
  }
  case action.enter:
  {
    this.enter();
    break;
  }
  default:
  {
    break;
  }
  }
}

class Player
{
public:
  char myShipsMatrix[rowsNumber][columnsNumber];
  char allMyShots[rowsNumber][columnsNumber];
  char allMyHits[rowsNumber][columnsNumber];
  char allEnemyShots[rowsNumber][columnsNumber];
  char allEnemyHits[rowsNumber][columnsNumber];
  Cursor cursor;
  char currentPage;
  bool currentPageHasChanged;
  bool beingAttacked(char rowNumber, char columnNumber) {
    //TODO instead of returning <bool>, return a custom struct
    //with another info, like: a ship was destroyed, and ship's coords
    this.allEnemyShots[rowNumber][columnNumber] = LED_ON;
    if(this.myShipsMatrix[rowsNumber][columnsNumber] = LED_ON) {
    this.allEnemyHits[rowNumber][columnNumber] = LED_ON;
    return true;
    }
    return false;
  }
  bool hasJustAttacked(char rowNumber, char columnNumber, bool isHit) {
    //TODO save ships coordonates in generateShips method and check if this shot destroyed
    //a ship or just damaged it. And change <isHit> arg with a custom struct
    players[currentPlayer].allMyShots[rowNumber][columnNumber]= LED_ON;
    players[currentPlayer].allMyHits[rowNumber][columnNumber] = LED_ON;
  }
  Player()
  {
    this.currentPage = 0;
    gameHelper.initializeMatrix(this.myShipsMatrix);
    gameHelper.generateShips(this.myShipsMatrix);
    gameHelper.initializeMatrix(this.allMyShots);
    gameHelper.initializeMatrix(this.allMyHits);
    gameHelper.initializeMatrix(this.allEnemyShots);
    gameHelper.initializeMatrix(this.allEnemyHits);
    cursor = new Cursor(0, 0);
  }
  char** getPage(char pageNumber) {
    pageNumber = pageNumber%5;
    switch(pageNumber) {
      case 0: return this.myShipsMatrix;
      case 1: return this.allMyShots;
      case 2: return this.allMyHits;
      case 3: return this.allEnemyShots;
      case 4: return this.allEnemyHits;
    }
    return NULL;
  }
  char** getCurrentPage() {
    return this.getPage(currentPage)
  }
  void switchPageLeft() {
    this.currentPage = (this.currentPage - 1) % 5;
    this.currentPageHasChanged = true;
  }
  void switchPageRight() {
    this.currentPage = (this.currentPage + 1) % 5;
    this.currentPageHasChanged = true;
  }
  void changePage(char newPageNumber) {
    this.curentPage = newPageNumber % 5;
    this.currentPageHasChanged;
  }
} 
class Cursor
{
public:
  Cursor(char rowN, char colN)
  {
    this.row = rowN;
    this.column = colN;
    this.oldRow = rowN;
    this.oldColumn = rowN;
  }
  char row;
  char column;
  char oldRow;
  char oldColumn;
  bool cursorHasMoved;
  //MODE = 0 => cursor at border will not move farther
  //MODE = 1 => cursor at border will move circularry
  bool moveLeft(char mode)
  {
    if (mode == 0)
    {
      if (this.column > 0)
      {
        this.oldColumn = this.column;
        this.column--;
      }
      else
        return false;
    }
    else
    {
      if (this.column > 0)
      {
        this.oldColumn = this.column;
        this.column--;
      }
      else
      {
        this.oldColumn = this.column;
        this.column = 7;
      }
    }
    this.cursorHasMoved = true;
    return true;
  }
  bool deleteMoveState()
  {
    this.cursorHasMoved = false;
  }
  bool moveRight(char mode)
  {
    if (mode == 0)
    {
      if (this.column < 7)
      {
        this.oldColumn = this.column;
        this.column++;
      }
      else
        return false;
    }
    else
    {
      if (this.column < 7)
      {
        this.oldColumn = this.column;
        this.column++;
      }
      else
      {
        this.oldColumn = this.column;
        this.column = 0;
      }
    }
    this.cursorHasMoved = true;
    return true;
  }
  bool moveUp(char mode)
  {
    if (mode == 0)
    {
      if (this.row < 7)
      {
        this.oldRow = this.row;
        this.row++;
      }
      else
        return false;
    }
    else
    {
      if (this.row < 7)
      {
        this.oldRow = this.row;
        this.row++;
      }
      else
      {
        this.oldRow = this.row;
        this.row = 0;
      }
    }
    this.cursorHasMoved = true;
    return true;
  }
  bool moveDown(char mode)
  {
    if (mode == 0)
    {
      if (this.row > 0)
      {
        this.oldRow = this.row;
        this.row--;
      }
      else
        return false;
    }
    else
    {
      if (this.row > 0)
      {
        this.oldRow = this.row;
        this.row--;
      }
      else
      {
        this.oldRow = this.row;
        this.row = 7;
      }
    }
    this.cursorHasMoved = true;
    return true;
  }
} 
class GameHelper
{
public:
  void initializeMatrix(char matrix[rowsNumber][columnsNumber])
  {
    char i, j;
    for (i = 0; i < rowsNumber; i++)
      for (j = 0; j < columnsNumber; j++)
        matrix[i][j] = 0;
  }
  void generateShips(char matrix[rowsNumber][columnsNumber])
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
}