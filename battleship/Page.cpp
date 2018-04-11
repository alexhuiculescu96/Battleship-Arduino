#include <iostream>
#include "battleship.ino"
#include <LedControl.h>
using namespace std;

class Page
{
  int ledMatrix[rowNumber][columnNumber];


public:
  Page(LedControl ledControl)
  {
    for(i=0; i< rowNumber; i++) 
    {
      for(j=0;j<columnNumber;j++)
      {
        this.ledMatrix[i][j] = LED_OFF;
      }
    }
  }
  Page(LedControl ledControl, int initialBoard[rowNumber][columnNumber])
  {
    int i,j;
    for(i=0; i< rowNumber; i++) 
    {
      for(j=0;j<columnNumber;j++)
      {
        this.ledMatrix[i][j] = initialBoard[i][j];
      }
    }
  }
};
