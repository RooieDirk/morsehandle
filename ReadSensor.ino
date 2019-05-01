
byte SensorValueCounter = 0;

byte SensorValCounterNext()
{
  if ( ++SensorValueCounter >= SENSOR_AVG_NO )
    SensorValueCounter = 0;
  return SensorValueCounter;
}

void FillAvgArray(int value)
{
  for ( int i=0; i < SENSOR_AVG_NO; i++ )
        SensorValue[i] = value;
}
int GetAvgArray()
{
  int t=0;
  for ( int i=0; i< SENSOR_AVG_NO; i++)
  {
    t = t + SensorValue[i];
  }
  return (int) t/SENSOR_AVG_NO;
}

int analogReadAvg()
{
  // read the value from the sensor 0 - 1024
  int sens = analogReadDirect();
  if ( abs(sens - GetAvgArray() ) >= DEADBAND ){
    SensorValue[ SensorValCounterNext() ] = analogRead(SensorPin);
  }
  return GetAvgArray();
}

int analogReadDirect()
{
  // read the value from the sensor 0 - 1024
  return analogRead(SensorPin);
}
