#include <LedControl.h>
<<<<<<< HEAD
=======
#include "../globals.cpp"
>>>>>>> 1b68b84962f52a00fa7db05bcdf9771715767bdf
/* Led controller for two MAX7219
 * pin 12 -> DataIn
 * pin 11 -> CS
 * pin 10 -> CLK
 */
LedControl lc = LedControl(12,10,11,2);

const int rows = 8;
const int columns = 8; 
const int numberOfShips = 5;
int playerOneBoard[rows][columns];
int playerTwoBoard[rows][columns];
int ships[] = {4,3,3,2,2};

void clearBoard(int board[][columns])
{
  int i,j;
  for(i=0; i<rows; i++)
    for(j=0; j<columns; j++)
      board[i][j] = 0;
}

void initializeBoard(int board[][columns])
{
  int x = 0, y = 0, flag, j, v;
 
  for(int i=0; i<numberOfShips; i++)
  {
    flag = 0;
    v = random() % 2;
    /* 0 - horizontal
     * 1 - vertical
     */
    if(v)
    {
      while(!flag)
      {
        x = random() % (rows - ships[i]);
        y = random() % columns;
        if(board[x][y])
          /* (x,y) position already taken */
          continue;
        else
        {
          /* fill the column*/
          int temp = x;
          flag = 1;
          for(j=0; j<ships[i]; j++,temp++)
          {
            if(board[temp][y])
              flag = 0;
          }
          if(!flag)
            continue;
          for(j=0; j<ships[i]; j++,x++)
            board[x][y] = 1;
        }
      }
    }
    else
    {
      while(!flag)
      {
        x = random() % rows;
        y = random() % (columns - ships[i]);
        if(board[x][y])
          /* (x,y) position already taken */
          continue;
        else
        {
          /* fill the row*/
          int temp = y;
          flag = 1;
          for(j=0; j<ships[i]; j++,temp++)
          {
            if(board[x][temp])
              flag = 0;
          }
          if(!flag)
            continue;
          for(j=0; j<ships[i]; j++,y++)
            board[x][y] = 1;
        }
      }
    }
  }
}

void showBoard(int board[][columns], int addr)
{
  int i,j;
  for(i=0; i<rows; i++)
    for(j=0; j<columns; j++)
    {
      if(board[i][j])
      {
        lc.setLed(addr,i,j,true);
      }
    }
}

void setup() 
{
  Serial.begin(9600);
  randomSeed(analogRead(0));
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  lc.shutdown(1,false);
  /* Set the brightness to lowest values */
  lc.setIntensity(0,1);
  lc.setIntensity(1,1);
  /* and clear the display */
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  /* Initialize the boards */
  initializeBoard(playerOneBoard);
  initializeBoard(playerTwoBoard);
}
long int count=0;
boolean currentValue=false;
void loop() 
{ 
 // showBoard(playerOneBoard, 0);
 // showBoard(playerTwoBoard, 1);
  //delay(5);
  count++;
  if(count == 2)
  { 
    currentValue = !currentValue; 
    lc.setLed(0,1,1,false);
  } else if(count == 20)
    {
    lc.setLed(0,1,1,true);
      count = 0;
    }
  lc.setLed(0,1,2,true);
  lc.setLed(1,1,1,true);
}
