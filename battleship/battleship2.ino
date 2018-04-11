#include <LedControl.h>
#include "../globals.cpp"
/* Led controller for two MAX7219
 * Change 4th parameter to 2 !!!!!!!!!!!!!!
 * pin 12 -> DataIn
 * pin 11 -> CS
 * pin 10 -> CLK
 */
LedControl lc = LedControl(12, 10, 11, 2);
Player p1 = Player();
Player p2 = Player();
Game game = Game();
ButtonListener listener = ButtonListener();

static const struct actionType {
  int enter = 0;
  int moveCursorLeft = 3;
  int moveCursorRight = 1;
  int moveCursorDown = 2;
  int moveCursorUp = 6;
  int switchPageLeft = 7;
  int switchPageRight = 5;
  int retreat = 4;  
} action;

void testSetLed(int row, int col, int maxnr)
{
  lc.setLed(maxnr, row, col, true);
  //delay(delaytime);
  //lc.setLed(0,row,col,false);
  //delay(delaytime);
}

void testAllLeds(int board[][columns], int maxnr)
{
  int i, j;
  for (i = 0; i < rows; i++)
    for (j = 0; j < columns; j++)
    {
      if (board[i][j])
      {
        testSetLed(i, j, maxnr);
      }
    }
  //delay(delaytime*5);
  //lc.clearDisplay(0);
  //delay(delaytime);
}

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
  game.startGame();
}

void loop()
{
  if (listener.checkPressedButton())
  {
    game.buttonPressed(listener.getPressedButton());
  }
}

class Game
{
  Player players[];
  Player currentPlayer;
  int startGame()
  {
    return 0;
  }
  void moveCursorLeft() {

  }
  void moveCursorRight() {

  }
  void moveCursorUp() {

  }
  void moveCursorDown() {

  }
  void switchPageLeft() {

  }
  void switchPageRight() {

  }
  void retreat() {

  }
  void enter() {

  }
public:
  void buttonPressed(int pressedButton)
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
      this.retreat()
      break;
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
}
}

class Player {

}