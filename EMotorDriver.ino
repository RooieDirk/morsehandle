int ConvertSensorToPWM(int sensorval)
{
  if ( abs(sensorval-CONFIG.Neutral) <= DEADBAND ) return 0;
  int DeadSlow_abs, Full_abs, pwmSlow, pwmFull;
  bool GoingForward;
  if (sensorval < CONFIG.Neutral ){
    if (CONFIG.ForwardDeadSlow < CONFIG.Neutral)
      GoingForward = true;
    else
      GoingForward = false;
  }
  else{  //sensorval > CONFIG.Neutral
    if (CONFIG.ForwardDeadSlow < CONFIG.Neutral)
      GoingForward = false;
    else
      GoingForward = true;
  }
  
  int sensorval_abs = abs(sensorval - CONFIG.Neutral);
  if ( GoingForward)
  {
    DeadSlow_abs = abs(CONFIG.ForwardDeadSlow - CONFIG.Neutral);
    Full_abs = abs(CONFIG.ForwardFull - CONFIG.Neutral);
    pwmSlow = CONFIG.PwmForwardDeadSlow;
    pwmFull = CONFIG.PwmForwardFull;
  }
  else // In astern position
  {
    DeadSlow_abs = abs(CONFIG.AsternDeadSlow - CONFIG.Neutral);
    Full_abs = abs(CONFIG.AsternFull - CONFIG.Neutral);
    pwmSlow = CONFIG.PwmAsternDeadSlow;
    pwmFull = CONFIG.PwmAsternFull;
  }

  if (sensorval_abs < (int)DeadSlow_abs * 0.20){
    OldValue = 0;
    return 0;
  }
  ChangeMotorDirection(GoingForward);
  if (sensorval_abs < (int)DeadSlow_abs * 0.80){
    OldValue = 0;
    return 0;
  }
  if (sensorval_abs < (int)DeadSlow_abs) sensorval_abs = DeadSlow_abs;
  if (OldValue = 0){
    sensorval_abs = (int) sensorval_abs * (1 + CLUTCHIN_OVERSHOOT / 100);
    FillAvgArray(sensorval_abs);
  }    
  
  OldValue = sensorval_abs;
  return map( sensorval_abs, DeadSlow_abs, Full_abs, pwmSlow, pwmFull);  
}

void ChangeMotorDirection(bool r)
{  
  bool fr;
  if (r) fr = CONFIG.AsternSwitch;
  else fr = !CONFIG.AsternSwitch;
  if( digitalRead(ReversePin) != fr){
    if( MotorState != STOPPED )
      BreakMotor(500); //stop motor and wait 500 ms
    digitalWrite(ReversePin, fr );
  }
}

void StopMotor(int delaytime)
{
  OCR2A = 0; //set output pwm to nul
  FillAvgArray(0);
  delay( delaytime );
  MotorState = STOPPED;  
}

void BreakMotor(int delaytime)
{ //brake for a given periode of time (ms)
  OCR2A = 0; //set output pwm to nul
  digitalWrite(BrakePin, 1);
  MotorState = BRAKE;
  FillAvgArray(0);
  delay( delaytime );
  FillAvgArray(0);
  digitalWrite(BrakePin, 0);
  MotorState = STOPPED;  
}

void SetMotorForward( int pwm_val)
{
  digitalWrite(ReversePin, !CONFIG.AsternSwitch );
  OCR2A = pwm_val;
}
