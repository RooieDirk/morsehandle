// BOF preprocessor bug prevent - insert me on top of your arduino-code
// From: http://www.a-control.de/arduino-fehler/?lang=en
#if 1
__asm volatile ("nop");
#endif
//
int pwm = 11;      // select the pin for pwm
int reverse = 10;  // select the pin for direction control
int brake = 12;    // select the pin for braking on motor
int SensorPin = A0;    // select the input pin for the potentiometer
// ID of the settings block
#define CONFIG_VERSION "0.1"
#define SENSOR_AVG_NO 10
#define DEADBAND 4
#define CLUTCHIN_OVERSHOOT 10 //in percents of deadslow
#define SENSORREAD_FREQ_DIV 10 // Gives a reading frequency of 10Hz. (100Hz / 10)
//enum workaround
#define REVERSE 0
#define NEUTRAL 1
#define FORWARD 2
#define BRAKE 3
#define REGENERATING 4
#define STOPPED 5
//end enum workaround
#define DEBUG 1

int SensorValue[SENSOR_AVG_NO]{0};  // array to store the value coming from the sensor
byte S_ArrayIndex = 0;
int OldValue=0;
int StartupTimer = 0;
int HandleNeutral = 0;
int divcounter = 0;
byte MotorState = STOPPED;

bool SetupMode = false;
bool ShowSensorValue = false;

typedef struct {
  // This is for mere detection if they are your settings
  char version[4];
  // The variables of your settings
  int AsternFull, AsternDeadSlow, Neutral, ForwardDeadSlow, ForwardFull;
  int PwmAsternFull, PwmAsternDeadSlow, PwmForwardDeadSlow, PwmForwardFull, ClutchinOvershoot;
  bool AsternSwitch;  
} config_struct;

config_struct CONFIG = {
  CONFIG_VERSION,
  // The default values
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 10,
  true
};
config_struct SET;


void setup() {
  SetupTimers();
  // initialize serial:
  Serial.begin(4800);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Type 'setup' within 10 seconds for setup.");
  if (!loadConfig() ){
    Serial.println("No valid config found, going to setup");
    SetupParameters();
  }
  else 
  {
    FillAvgArray( CONFIG.Neutral );
    Serial.println("Configuration loaded");
  }
    pinMode(pwm,OUTPUT) ;
    pinMode(reverse,OUTPUT) ;
    pinMode(brake,OUTPUT) ;
    
    TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(CS22);
    OCR2A = 180;
    OCR2B = 50;
}
void SetupTimers()
{
  cli();//stop interrupts

  //set timer2 interrupt at 100Hz
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 100hz increments
  OCR0A = 155;                           // = (16*10^6) / (100*1024) - 1 (must be <256)
  TCCR0A |= (1 << WGM01);                // turn on CTC mode
  TCCR0B |= (1 << CS02)| (1 << CS00);    // Set CS22 and CS20 bits for 1024 prescaler   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
  sei();//allow interrupts
  
}

ISR(TIMER0_COMPA_vect){
  // This timer is set at a frequency of 100Hz
  if (StartupTimer >= 0 ){
    if ( ++StartupTimer < 1000 ){
      if( StartupTimer % 100 == 1 ) Serial.print(".");
    }
    if ( StartupTimer > 1000 ){
      StartupTimer=-1;
      Serial.println("");
      Serial.println("Sorry your time to enter setup is over ;-)");
      Serial.flush();
      #ifndef DEBUG
        Serial.println("Closing the serial port");
        Serial.flush();
        Serial.end();
      #endif
    }
  }
  if ( divcounter++ == SENSORREAD_FREQ_DIV ){
    divcounter=0;
    
    if ( !SetupMode )
      OCR2A = analogReadDirect()/4;
  }
}

void loop() {
  if( (Serial.available()) && (StartupTimer) )
    SetupParameters();
}
