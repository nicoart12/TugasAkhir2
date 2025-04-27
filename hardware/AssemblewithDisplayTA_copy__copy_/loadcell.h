//#include "var.h"
#ifndef LOADCELL.H
#define LOADCELL.H

void updateLoadCell()
{
  static boolean newDataReady = 0;
  if (LoadCell.update()) {
      newDataReady = true;
  if (newDataReady)
  {
    for (int k = 1; k <= 20; k++)
    {
      float i = LoadCell.getData();
      sum += i;
    }
    average = sum / 20;
    newDataReady = false;
    //t = millis();
    sum = 0;
  }
  }
    
}
#endif