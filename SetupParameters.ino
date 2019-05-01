 void SetupParameters()
{
  if ( toupper(Serial.read()) == 'S')
    if ( GetInputFromSerial() == 'E')
      if ( GetInputFromSerial() == 'T')
        if ( GetInputFromSerial() == 'U')
          if ( GetInputFromSerial() == 'P')
          {
            StartupTimer = -1;
            SetupMode = true;
            SET = CONFIG;  
        
            if ( SetupHandlePos() )
                saveConfig();
            if ( SetupMotorSpeed() ){
                saveConfig();
                Serial.println("Configuration saved"); 
            }   
            loadConfig();
            SetupMode = false;
          }
          return;
}

int SetupHandlePos()
{
  bool loop = true;
  do {
    Serial.print("A set handle pos for Neutral [");Serial.print(SET.Neutral);Serial.println("]");
    Serial.print("B set handle pos for DeadSlow Forward [");Serial.print(SET.ForwardDeadSlow);Serial.println("]");
    Serial.print("C set handle pos for Full Forward [");Serial.print(SET.ForwardFull);Serial.println("]");
    Serial.print("D set handle pos for DeadSlow Astern [");Serial.print(SET.AsternDeadSlow);Serial.println("]");
    Serial.print("E set handle pos for Full Astern [");Serial.print(SET.AsternFull);Serial.println("]");
    Serial.println("Enter, for next");    
    Serial.println("Press a key");
    ShowSensorValue = true;

    char inChar = GetInputFromSerial();
      switch(inChar){
        case 'A':
          SET.Neutral = analogReadDirect();
          break;
        case 'B':
          SET.ForwardDeadSlow = analogReadDirect();
          break;
        case 'C':
          SET.ForwardFull = analogReadDirect();
          break;
        case 'D':
          SET.AsternDeadSlow = analogReadDirect();
          break;
        case 'E':
          SET.AsternFull = analogReadDirect();
          break;
      }
      
    if (inChar == char(27) ) return 0;
    if (inChar == char(13) ) loop = false;
    ShowSensorValue = false;
  }while (loop);
  
  return 1;
}

int SetupMotorSpeed()
{  
  Serial.println(); Serial.println(); Serial.println();

  bool  loop = true;
  do {
    Serial.print("F to set Dead-slow forward speed [");Serial.print(SET.PwmForwardDeadSlow);Serial.println("]");
    Serial.print("G to set Dead-slow astern speed [");Serial.print(SET.PwmAsternDeadSlow);Serial.println("]");
    Serial.print("H to set Full-forward speed [");Serial.print(SET.PwmForwardFull);Serial.println("]");
    Serial.print("I to set Full-astern speed [");Serial.print(SET.PwmAsternFull);Serial.println("]");
    Serial.println("Enter, for next");    
    Serial.println("Press a key");
    
    char inChar = GetInputFromSerial();
 
      switch(inChar){
        case 'F':
          SET.PwmForwardDeadSlow = DoSpeedSetting( "Dead-Slow Forward", SET.PwmForwardDeadSlow);
          break;
        case 'G':
          SET.PwmAsternDeadSlow = DoSpeedSetting( "Dead-Slow Astern", SET.PwmAsternDeadSlow);
          break;
        case 'H':
          SET.PwmAsternDeadSlow = DoSpeedSetting( "Full ahead", SET.PwmAsternDeadSlow);
          break;
        case 'I':
          SET.PwmAsternDeadSlow = DoSpeedSetting( "Full Astern", SET.PwmAsternDeadSlow);
          break;
      }
      if (inChar == char(27) ) return 0;
      if (inChar == char(13) ) loop = false; 
    }while (loop);
    return 1;
}

int DoSpeedSetting( String text, int startsetting)
{
  Serial.println(); Serial.println(); Serial.println();
  Serial.print("Set motor speed:");
  Serial.print("'F' for faster, 'S' for slower.");
  Serial.print("'D' for changing turning direction.");Serial.println(text);
  Serial.println("'Enter' when satisfied");

  bool loop = true;
  do {  
      char inChar = GetInputFromSerial(); 
      switch(inChar){
        case 'F':
          if ( startsetting < 255 ) startsetting++;
          break;
        case 'S':
          if ( startsetting > 0 ) startsetting--;
          break;
        case 'D':        
          SetMotorspeed( 0, SET.AsternSwitch); //stop motor
          SET.AsternSwitch = !SET.AsternSwitch; //togle direction bit
          delay(2000);                  
          break;          
      }
      if (inChar == char(13) ) loop = false; 
      SetMotorspeed( startsetting, SET.AsternSwitch);
        Serial.print( char(13) );
        Serial.print( "[" );
        Serial.print( startsetting );
        Serial.print( "]   " ); 
   }while (loop);
  return startsetting;
}

void SetMotorspeed( int pwm_val, bool direction)
{
  digitalWrite(reverse, direction );
  OCR2A = pwm_val;
}

char GetInputFromSerial()
{
  while (Serial.available()) { //clear the input buffer
    Serial.read();}
  while ( Serial.available()<= 0 ) {
//    if (ShowSensorValue){
//        Serial.print( char(13) );
//        Serial.print( "[" );
//        Serial.print( analogReadDirect() );
//        Serial.print( "]   " );
//        delay(50);
//      }
      ; } //wait for input
    return (char)toupper(Serial.read());
}
