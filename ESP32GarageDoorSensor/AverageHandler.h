#ifndef _AVERAGEHANDLER_H_
#define _AVERAGEHANDLER_H_

/*
 * File: AverageHandler.h
 * Description: Calculate average.
 * 
 */

template <int SIZE, class T>
class AverageHandler
{
private:
  T sampleArray[SIZE];
  int length;
  int startPos;

public:
  AverageHandler() : sampleArray{0}, length(0), startPos(0)
  {
    
  }
  int getLength();
  void addSample(T sample);
  T calculateAverage();
};

template <int SIZE, class T>
int AverageHandler<SIZE, T>::getLength()
{
  return length;
}

template <int SIZE, class T>
void AverageHandler<SIZE, T>::addSample(T sample)
{
  if(length < SIZE)
  {
    sampleArray[length]=sample;
    length++;
  }
  else
  {
    sampleArray[startPos]=sample;
    startPos=(startPos+1)%length;
  }
}

template <int SIZE, class T> T AverageHandler<SIZE, T>::calculateAverage()
{
  int curPos=startPos;
  T curValue=0;
  for(int i=startPos; i < startPos+length; i++)
  {
    curPos=i%length;
    curValue+=sampleArray[curPos];
  }
  return curValue/((T)length);
}



 #endif
