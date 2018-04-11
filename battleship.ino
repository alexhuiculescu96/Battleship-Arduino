#include <LedControl.h>

/* Led controller for two MAX7219
 * Change 4th parameter to 2 !!!!!!!!!!!!!!
 * pin 12 -> DataIn
 * pin 11 -> CS
 * pin 10 -> CLK
 */
LedControl lc = LedControl(12,10,11,1);
unsigned long delaytime = 1000;

const int rows = 8;
const int columns = 8;

int playerOneBoard[rows][columns];
int playerTwoBoard[rows][columns];

void initializeBoard(int board[][columns])
{
  int i,j;
  for(i=0; i<rows; i++)
    for(j=0; j<columns; j++)
    {
      board[i][j] = random() % 2;
    }
}

void testSetLed(int row, int col)
{
  lc.setLed(0,row,col,true);
  //delay(delaytime);
  //lc.setLed(0,row,col,false);
  //delay(delaytime);
}

void testAllLeds(int board[][columns])
{
  int i,j;
  for(i=0; i<rows; i++)
    for(j=0; j<columns; j++)
    {
      if(board[i][j])
      {
        testSetLed(i,j);     
      }
    }
  delay(delaytime*5);
  lc.clearDisplay(0);
  delay(delaytime);
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
  /* Set the brightness to a medium values */
  lc.setIntensity(0,1);
  /* and clear the display */
  lc.clearDisplay(0);
  /* Initialize the boards */
  initializeBoard(playerOneBoard);
  initializeBoard(playerTwoBoard);
}

void loop() 
{
  testAllLeds(playerOneBoard);
  testAllLeds(playerTwoBoard);
}
