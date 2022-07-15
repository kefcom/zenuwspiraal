#include <WS2812FX.h>

#define LED_COUNT 141
#define LED_PIN 2
#define CONE_LEFT 13      //0 in array
#define CONE_RIGHT 14     //1 in array
#define COPPER_TUBE 12    //2 in array
#define BTN_1 27          //3 in array
#define BTN_2 26          //4 in array
#define BTN_3 25          //5 in array
#define BTN_4 33          //6 in array
#define RELAIS 32

int state = 0;

int buttonPins[7] = {CONE_LEFT,CONE_RIGHT,COPPER_TUBE,BTN_1,BTN_2,BTN_3,BTN_4};
unsigned long lastButtonPress[7] = {0,0,0,0,0,0,0};
byte lastButtonState[7];
int debounceTime[7] = {500,1000,500,100,100,100,100};

int segment_0_START = 0;
int segment_0_STOP = 23;
int segment_1_START = 25;
int segment_1_STOP = 110;
int segment_2_START = 112;
int segment_2_STOP = 135;
int segment_3_START = 136;
int segment_3_STOP = 136;
int segment_4_START = 137;
int segment_4_STOP = 137;
int segment_5_START = 138;
int segment_5_STOP = 138;
int segment_6_START = 139;
int segment_6_STOP = 139;

int customEffect1_counter = 0;

int demoSpeed;
int demoIndex;

int SP_defaultSpeed = 30000; //speed in game mode 1 (STATE 7)
int SP_speed; //speed in game mode 1 (STATE 7) (set during the game)
int SP_interval = 15; //speed increase in mode 1 (STATE 7) (in percentage)
int SP_gamesWon = 0;

unsigned long gameStartTime;

// speeds (baby, kid, junior, adult, expert)
//        (40s,  35s, 30s,    20s,   10s)
int SP_speeds[5] = {40000,35000,30000,20000,10000};
uint32_t SP_colors[5] = {GREEN, BLUE, YELLOW, ORANGE, RED};
int SP_index; // index
int SP_penalty[5] = {3000,3000,3000,3000,3000};

//game 3 options
uint32_t MP_colors[8] = {RED,GREEN,BLUE,PURPLE,YELLOW,CYAN,ORANGE,WHITE};
int MP_players = 2;
int MP_defaultSpeed = 30000;
int MP_speed;
unsigned long MP_fastestTime;
int MP_penalty = 3000;
int MP_penalties = 0;
int MP_currentPlayer;
int MP_leader;

unsigned long lastPenalty;
int Penalty_effectDuration = 2000;


WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  ws2812fx.init();
  ws2812fx.setBrightness(255);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(0x007BFF);
  ws2812fx.setMode(12);
  ws2812fx.start();
  

  //set buttons ande set the default state
  for(int i = 0; i<7; i++)
  {     
    pinMode(buttonPins[i], INPUT_PULLUP);   
    lastButtonState[i] = HIGH;
  }

  Serial.begin(9600);

  //define segments on the ledstrip
  ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_RAINBOW_CYCLE, RED, 1000, false); //CONE_LEFT
  ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_RAINBOW_CYCLE, RED, 1000, false); //COPPER_TUBE
  ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_RAINBOW_CYCLE, RED, 1000, false); //CONE_RIGHT
  ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_4
  ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_3
  ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_BLINK, GREEN, 500, false); //BTN_2
  ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_1

}

void loop() {
  ws2812fx.service();
  checkButtons();
  setLeds();
}

void setLeds()
{
  switch(state)
  {
    case 0:
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_RAINBOW_CYCLE, RED, 1000, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_RAINBOW_CYCLE, RED, 1000, false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_RAINBOW_CYCLE, RED, 1000, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_BLINK, GREEN, 500, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_1    
      break;
    case 1:
      ws2812fx.setCustomMode(tube_chase_clear);
      ws2812fx.setSegment(0, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_RAINBOW_CYCLE, RED, 1000, false); //CONE_LEFT       
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_CUSTOM, RED, demoSpeed, false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_STATIC, BLACK, 1000, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_BLINK, GREEN, 500, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLUE, 1000, false); //BTN_1
      ws2812fx.setSegment(7, 0,6, FX_MODE_BLINK, GREEN, 1000, false); //first quadrant of the left cone
      ws2812fx.setSegment(8, 7,23, FX_MODE_STATIC, BLUE, 1000, false); //rest of the left cone
    break;
    case 2:
      ws2812fx.setCustomMode(tube_chase_clear);
      ws2812fx.setSegment(0, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_RAINBOW_CYCLE, RED, 1000, false); //CONE_LEFT       
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_CUSTOM, SP_colors[SP_index], SP_speeds[SP_index] / (segment_1_STOP-segment_1_START), false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_STATIC, BLACK, 1000, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_BLINK, SP_colors[SP_index], 1000, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_BLINK, GREEN, 500, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLUE, 1000, false); //BTN_1
      ws2812fx.setSegment(7, 0,6, FX_MODE_STATIC, BLUE, 1000, false); //first quadrant of the left cone
      ws2812fx.setSegment(8, 7,12 , FX_MODE_BLINK, GREEN, 1000, false); //second quadrant of the left cone
      ws2812fx.setSegment(9, 13,23, FX_MODE_STATIC, BLUE, 1000, false); //rest of the left cone
    break;    
    case 3:
      ws2812fx.setCustomMode(colorRacers);
      ws2812fx.setSegment(0, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_RAINBOW_CYCLE, RED, 1000, false); //CONE_LEFT       
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_CUSTOM, RED, demoSpeed, false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_STATIC, BLACK, 1000, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_BLINK, GREEN, 500, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLUE, 1000, false); //BTN_1
      ws2812fx.setSegment(7, 0,12, FX_MODE_STATIC, BLUE, 1000, false); //first and second quadrant of the left cone
      ws2812fx.setSegment(8, 13, 18, FX_MODE_BLINK, GREEN, 1000, false); //third quadrant of the left cone
      ws2812fx.setSegment(9, 19,23, FX_MODE_STATIC, BLUE, 1000, false); //rest of the left cone
    break;    
    case 4:
      ws2812fx.setCustomMode(countBack);
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, 16, RED, 700, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_CUSTOM, RED, 100, false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_STATIC, BLACK, 1000, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 500, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_1
      //REMOVE SEGMENTS 7,8,8
      ws2812fx.setSegment(7, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_STATIC, BLACK, 1000, false); 
      ws2812fx.setSegment(8, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_STATIC, BLACK, 1000, false); 
      ws2812fx.setSegment(9, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_STATIC, BLACK, 1000, false);
    break;
    case 5:
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_STATIC, GREEN, 100, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_STATIC, BLACK, 100, false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_STATIC, GREEN, 100, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_BLINK, GREEN, 1000, false); //BTN_1    
    break;
    case 6:
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_BLINK, GREEN, 1000, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_STATIC, BLACK, 100, false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_STATIC, GREEN, 100, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_1    
    break;    
    case 7:
      ws2812fx.setCustomMode(effect_game1);
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, 16, GREEN, 1000, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_CUSTOM, BLACK, SP_speed / (segment_1_STOP-segment_1_START), false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, 16, GREEN, 1000, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_1    
    break;        
    case 8:
      //customEffect1_counter
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_BLINK, RED, 500, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, (segment_1_START+ customEffect1_counter), FX_MODE_BLINK, RED, 1000, false); //COPPER_TUBE as far as it's filled
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_BLINK, RED, 500, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_BLINK, GREEN, 1000, false); //BTN_1    
    break;
    case 9:
      //customEffect1_counter
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_BLINK, GREEN, 500, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, (segment_1_START+ customEffect1_counter), FX_MODE_BLINK, GREEN, 1000, false); //COPPER_TUBE as far as it's filled
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_BLINK, GREEN, 500, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_BLINK, GREEN, 1000, false); //BTN_1    
    break;    
    case 10:
      //customEffect1_counter
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, 16, BLUE, 500, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, (segment_1_START+ customEffect1_counter), FX_MODE_BLINK, BLUE, 1000, false); //COPPER_TUBE as far as it's filled
      ws2812fx.setSegment(7, (segment_1_START+ customEffect1_counter),segment_1_STOP , FX_MODE_STATIC, BLACK, 1000, false); //COPPER_TUBE rest
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, 16, BLUE, 500, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_BLINK, GREEN, 1000, false); //BTN_1    
    break;    
    case 11:
      ws2812fx.setCustomMode(countBack);
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, 16, RED, 700, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_CUSTOM, RED, 100, false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_STATIC, BLACK, 1000, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 500, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_1
      //REMOVE SEGMENTS 7,8,8
      ws2812fx.setSegment(7, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_STATIC, BLACK, 1000, false); 
      ws2812fx.setSegment(8, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_STATIC, BLACK, 1000, false); 
      ws2812fx.setSegment(9, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_STATIC, BLACK, 1000, false);
    break;
    case 12:
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_STATIC, GREEN, 100, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_STATIC, BLACK, 100, false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_STATIC, GREEN, 100, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_BLINK, GREEN, 1000, false); //BTN_1    
    break;         
    case 13:
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_BLINK, GREEN, 1000, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_STATIC, BLACK, 100, false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_STATIC, GREEN, 100, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_1          
    break;
    case 14:
      ws2812fx.setCustomMode(effect_game2);
      if(millis() < (lastPenalty + Penalty_effectDuration))
      {
        ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_BLINK, RED, 500, false); //CONE_LEFT
      }else{
        ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, 16, GREEN, 1000, false); //CONE_LEFT
      }
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_CUSTOM, BLACK, SP_speed / (segment_1_STOP-segment_1_START), false); //COPPER_TUBE
      if(millis() < (lastPenalty + Penalty_effectDuration))
      {
        ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_BLINK, RED, 500, false); //CONE_RIGHT
      }else{
        ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, 16, GREEN, 1000, false); //CONE_RIGHT
      }
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_1    
    break;    
    case 15:
      //customEffect1_counter
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_BLINK, RED, 500, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, (segment_1_START+ customEffect1_counter), FX_MODE_BLINK, RED, 1000, false); //COPPER_TUBE as far as it's filled
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_BLINK, RED, 500, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_BLINK, GREEN, 1000, false); //BTN_1    
    break;     
    case 16:
      //customEffect1_counter
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_BLINK, GREEN, 500, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, (segment_1_START+ customEffect1_counter), FX_MODE_BLINK, GREEN, 1000, false); //COPPER_TUBE as far as it's filled
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_BLINK, GREEN, 500, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_BLINK, GREEN, 1000, false); //BTN_1    
    break;    
    case 17:
      ws2812fx.setCustomMode(select_game3);
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_STATIC, BLACK, 500, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_CUSTOM, RED, 100, false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_STATIC, BLACK, 500, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_BLINK, GREEN, 1000, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_BLINK, RED, 1000, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLUE, 1000, false); //BTN_1    
      //REMOVE SEGMENTS 7,8,8
      ws2812fx.setSegment(7, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_STATIC, BLACK, 1000, false); 
      ws2812fx.setSegment(8, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_STATIC, BLACK, 1000, false); 
      ws2812fx.setSegment(9, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_STATIC, BLACK, 1000, false);      
    break;        
    case 18:
      ws2812fx.setCustomMode(countBackGame3);
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, 16, RED, 700, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_CUSTOM, RED, 100, false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_STATIC, BLACK, 1000, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 500, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLACK, 1000, false); //BTN_1
      //REMOVE SEGMENTS 7,8,8
      ws2812fx.setSegment(7, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_STATIC, BLACK, 1000, false); 
      ws2812fx.setSegment(8, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_STATIC, BLACK, 1000, false); 
      ws2812fx.setSegment(9, segment_6_STOP +2, segment_6_STOP +2, FX_MODE_STATIC, BLACK, 1000, false);      
    break;
    case 19:
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_STATIC, GREEN, 100, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_STATIC, BLACK, 100, false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_STATIC, GREEN, 100, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_BLINK, GREEN, 1000, false); //BTN_1        
    break;
    case 20:
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_BLINK, GREEN, 1000, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_STATIC, BLACK, 100, false); //COPPER_TUBE
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_STATIC, GREEN, 100, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_1          
    break;  
    case 21:
      ws2812fx.setCustomMode(effect_game3);
      if(millis() < (lastPenalty + Penalty_effectDuration))
      {
        ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_BLINK, RED, 500, false); //CONE_LEFT
      }else{
        ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, 16, GREEN, 1000, false); //CONE_LEFT
      }
      ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_CUSTOM, BLACK, MP_speed / (segment_1_STOP-segment_1_START), false); //COPPER_TUBE
      if(millis() < (lastPenalty + Penalty_effectDuration))
      {
        ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_BLINK, RED, 500, false); //CONE_RIGHT
      }else{
        ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, 16, GREEN, 1000, false); //CONE_RIGHT
      }
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_1    
    break;    
    case 22:
      //customEffect1_counter
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_BLINK, RED, 500, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, (segment_1_START+ customEffect1_counter), FX_MODE_BLINK, MP_colors[MP_currentPlayer], 1000, false); //COPPER_TUBE as far as it's filled
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_BLINK, RED, 500, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_BLINK, GREEN, 1000, false); //BTN_1    
    break;        
    //MP_leader
    case 23:
      //customEffect1_counter
      if(MP_leader != -1)
      {
        //a player won
        ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_BLINK, MP_colors[MP_leader], 500, false); //CONE_LEFT
        ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, 16, MP_colors[MP_leader], 4000, false); //COPPER_TUBE as far as it's filled
        ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_BLINK, MP_colors[MP_leader], 500, false); //CONE_RIGHT
        ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
        ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
        ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
        ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_BLINK, GREEN, 1000, false); //BTN_1    
      }else{
        //the computer won
        ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_RAINBOW_CYCLE, WHITE, 500, false); //CONE_LEFT
        ws2812fx.setSegment(1, segment_1_START, segment_1_STOP, FX_MODE_RAINBOW_CYCLE, WHITE, 500, false); //COPPER_TUBE as far as it's filled
        ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_RAINBOW_CYCLE, WHITE, 500, false); //CONE_RIGHT
        ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
        ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
        ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
        ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_BLINK, GREEN, 1000, false); //BTN_1            
      }
    break;         
    case 24:
      //customEffect1_counter
      ws2812fx.setSegment(0, segment_0_START, segment_0_STOP, FX_MODE_BLINK, GREEN, 500, false); //CONE_LEFT
      ws2812fx.setSegment(1, segment_1_START, (segment_1_START+ customEffect1_counter), FX_MODE_BLINK, MP_colors[MP_currentPlayer], 1000, false); //COPPER_TUBE as far as it's filled
      ws2812fx.setSegment(2, segment_2_START, segment_2_STOP, FX_MODE_BLINK, GREEN, 500, false); //CONE_RIGHT
      ws2812fx.setSegment(3, segment_3_START, segment_3_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_4
      ws2812fx.setSegment(4, segment_4_START, segment_4_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_3
      ws2812fx.setSegment(5, segment_5_START, segment_5_STOP, FX_MODE_STATIC, BLACK, 100, false); //BTN_2
      ws2812fx.setSegment(6, segment_6_START, segment_6_STOP, FX_MODE_BLINK, GREEN, 1000, false); //BTN_1    
    break;       
  }

}

void checkButtons()
{
  //detect button changes
  for(int i = 0; i<7; i++)
  {
    byte currentValue = digitalRead(buttonPins[i]);
    if(currentValue != lastButtonState[i] && millis() > (lastButtonPress[i] + debounceTime[i]))
    {
      lastButtonPress[i] = millis();
      lastButtonState[i] = currentValue;
      if(currentValue == LOW)
      {
        buttonPressed(i);
      }else{
        buttonReleased(i);
      }
    } 
  }
  //HARD CHECK FOR VULCAN NERVE PINCH
  if(digitalRead(BTN_1) == LOW && digitalRead(BTN_4) == LOW)
  {
    //HARD RESET
    Serial.println("Vulcan Nerve Pinch detected");
    state = 0;
  }
}

void buttonPressed(int btnIndex)
{

  switch(state)
  {
    case 0:
      switch(btnIndex)
      {
        case 4:
          state = 1;
          demoSpeed = 500;
          customEffect1_counter = 0;
          Serial.println("Changed to state 1");
        break;
      }
    break;
    case 1:
      switch(btnIndex)
      {
        case 4:
          customEffect1_counter = 0;
          SP_index = 0;
          state = 2;
          Serial.println("Changed to state 2");
        break;
        case 3:
          //START GAME 1
          SP_speed = SP_defaultSpeed;
          SP_gamesWon = 0;
          state = 4;
          Serial.println("Changed to state 4");
        break;
      }
    break;
    case 2:
      switch(btnIndex)
      {
        case 5:
          SP_index ++;
          if(SP_index > 4)
          {
            SP_index = 0;
          }
          break;
        case 4:
          customEffect1_counter = 0;
          demoSpeed = 500;
          demoIndex = 0;
          state = 3;
          Serial.println("Changed to state 3");
        break;
        case 3:
          //START GAME 2
          SP_speed = SP_speeds[SP_index];
          SP_gamesWon = 0;
          state = 11;
          Serial.println("Changed to state 11");
        break;        
      }
    break;
    case 3:
      switch(btnIndex)
      {
        case 3:
          MP_players = 2;
          state = 17;
          Serial.println("Changed to state 17");
        break;
        case 4:
          state = 1;
          demoSpeed = 500;
          customEffect1_counter = 0;
          Serial.println("Changed to state 1");
        break;
      }
    break;
    case 4:
      switch(btnIndex)
      {
        case 0:
          state = 5;
          Serial.println("Changed to state 5");
          break;
      }
    break;
    case 5:
      switch(btnIndex)
      {
        case 3:
          state = 6;
          Serial.println("Changed to state 6");
          break;
      }
    break;
    case 7:
      switch(btnIndex)
      {
          case 1:
          //player won game 1
          SP_gamesWon++;
          state = 9;
          Serial.println("Changed to state 9");
          break;          
        case 2:
          state = 8;
          Serial.println("Changed to state 8");
          break;
      }
    break;
    case 8:
      switch(btnIndex)
      {
        case 3:
          state = 4;
          SP_speed = SP_defaultSpeed;
          SP_gamesWon = 0;
          Serial.println("Changed to state 4");
          break;
      }
    break;    
    case 9:
      switch(btnIndex)
      {
        case 3:
          state = 10;
          customEffect1_counter = (SP_gamesWon-1);
          Serial.println("Changed to state 10");
          break;
      }
    break;            
    case 10:
      switch(btnIndex)
      {
        case 3:
          state = 4;
          //SP_speed = SP_speed - SP_interval;
          SP_speed = SP_speed - ((SP_interval * SP_speed)/100);
          Serial.println("Changed to state 4");
          Serial.print("Speed is now");
          Serial.println(SP_speed);
          break;
      }
    break;     
    case 11:
      switch(btnIndex)
      {
        case 0:
          state = 12;
          lastPenalty = millis();
          Serial.println("Changed to state 12");
          break;
      }       
    break;
    case 12:
      switch(btnIndex)
      {
        case 3:
          state = 13;
          Serial.println("Changed to state 13");
          break;
      }
    break;
    case 14:
      switch(btnIndex)
      {
        case 1:
          //player won game 2
          state = 16;
          Serial.println("Changed to state 16");
        break;
        case 2:
        int speedstep = SP_speeds[SP_index] / (segment_1_STOP-segment_1_START);
        int penaltyLeds = SP_penalty[SP_index] / speedstep;
        customEffect1_counter = customEffect1_counter + penaltyLeds;
        lastPenalty = millis();
        break;
      }
    break;
    case 15:
      switch(btnIndex)
      {
        case 3:
          state = 11;
          Serial.println("Changed to state 11");
          break;
      }
    break;     
    case 16:
      switch(btnIndex)
      {
        case 3:
          state = 2;
          Serial.println("Changed to state 2");
          break;
      }
    break;         
    case 17:
      switch(btnIndex)
      {
        case 3:
          MP_currentPlayer = 0;
          customEffect1_counter = 5;
          lastPenalty = millis();
          MP_speed = MP_defaultSpeed;
          MP_leader = -1;
          MP_penalties = 0;
          state = 18;
          Serial.println("Changed to state 18");
          break;
        case 4:
          MP_players --;
          if(MP_players < 2)
          {
            MP_players = 2;
          }
          Serial.print("MP players: ");
          Serial.println(MP_players);
          break;
        case 5:
          MP_players ++;
          if(MP_players > 8)
          {
            MP_players = 8;
          }
          Serial.print("MP players: ");
          Serial.println(MP_players);
          break;                    
      }
    break;      
    case 18:
      switch(btnIndex)
      {
        case 0:
          state = 19;
          Serial.println("Changed to state 19");
          break;
      }
    break;            
    case 19:
      switch(btnIndex)
      {
        case 3:
          state = 20;
          Serial.println("Changed to state 20");
          break;
      }
    break; 
    case 21:
      switch(btnIndex)
      {
        case 1:
          //player beat the time
          MP_speed = (millis() - gameStartTime) + (MP_penalties * MP_penalty);
          Serial.print("Player took ");
          Serial.print((millis() - gameStartTime)/1000);
          Serial.println (" seconds to complete the course");
          Serial.print("There were ");
          Serial.print(MP_penalties);
          Serial.print(" penalties resulting in ");
          Serial.print((MP_penalties * MP_penalty)/1000);
          Serial.println(" extra seconds");         
          Serial.print("New speed is ");
          Serial.println(MP_speed);
          MP_penalties = 0;
          MP_leader = MP_currentPlayer;
          state = 24;
          Serial.println("Changed to state 24");
        break;
        case 2:
        //tube touch
        int speedstep = MP_speed / (segment_1_STOP-segment_1_START);
        int penaltyLeds = MP_penalty / speedstep;
        MP_penalties++;
        customEffect1_counter = customEffect1_counter + penaltyLeds;
        lastPenalty = millis();
        break;
      }
    break;          
    case 22:
      switch(btnIndex)
      {
        case 3:
          if(MP_currentPlayer != (MP_players -1))
          {
            MP_currentPlayer++;
            state = 18;
            Serial.println("Changed to state 18");
          }else{
            state = 23;
            Serial.println("Changed to state 23");
          }
          break;
      }
    break;    
    case 23:
      switch(btnIndex)
      {
        case 3:
          state = 3;
          Serial.println("Changed to state 3");
          break;
      }
    break;                    
    case 24:
      switch(btnIndex)
      {
        case 3:
          if(MP_currentPlayer != (MP_players -1))
          {
            MP_currentPlayer++;
            state = 18;
            Serial.println("Changed to state 18");
          }else{
            state = 23;
            Serial.println("Changed to state 23");
          }
          break;
      }
    break;     
  }
}

void buttonReleased(int btnIndex)
{
  switch(state)
  {
    case 5:
      switch(btnIndex)
      {
        case 0:
          //ring is no longer touching cone in prep for game 1
          state = 4;
          Serial.println("Changed to state 4");
      }
    break;
    case 6:
      switch(btnIndex)
      {
        case 0:
          state = 7;
          //START GAME 1
          gameStartTime = millis();
          customEffect1_counter = 0;
          Serial.println("Changed to state 7");
      }
    break;    
    case 12:
      switch(btnIndex)
      {
        case 0:
          //ring is no longer touching cone in prep for game 2
          state = 11;
          Serial.println("Changed to state 11");
      }
    break;    
    case 13:
      switch(btnIndex)
      {
        case 0:
          //START GAME 2
          gameStartTime = millis();
          customEffect1_counter = 0;
          state = 14;
          Serial.println("Changed to state 14");
      }
    break;    
    case 19:
      switch(btnIndex)
      {
        case 0:
          //ring is no longer touching cone in prep for game 3
          state = 18;
          Serial.println("Changed to state 18");
      }  
    break;    
    case 20:
      switch(btnIndex)
      {
        case 0:
          //START GAME 3         
          gameStartTime = millis();
          customEffect1_counter = 0;
          state = 21;
          Serial.println("Changed to state 21");
      }  
    break;    
  }
}




































// custom effects
uint16_t tube_chase_clear(void) { // chase and clear
  WS2812FX::Segment* seg = ws2812fx.getSegment(); // get the current segment
  //set everyting in the tube to black
  for(int i = segment_1_START; i<=segment_1_STOP;i++)
  {
    ws2812fx.setPixelColor(i, BLACK);
  }
  //now draw as far as we got in the last loop
  for(int i = segment_1_START; i<(segment_1_START+customEffect1_counter);i++)
  {
    ws2812fx.setPixelColor(i, RED);   
  }

  //bump the counter
  customEffect1_counter ++;

  //set the pixel that is 1 step ahead to green
  if((segment_1_START+customEffect1_counter)+1 <= segment_1_STOP)
  {
    ws2812fx.setPixelColor((segment_1_START+customEffect1_counter)+1, GREEN);
  }
  

  //reset to 0 if we reached the end
  if((segment_1_START+customEffect1_counter) > segment_1_STOP)
  {
    customEffect1_counter = 0;
    //depending on the state, move quicker by value or list
    switch(state)
    {
      case 1:
        if(seg->speed > 100)
        {
          demoSpeed = demoSpeed - 100;
          Serial.print("Changed speed to ");
          Serial.println(demoSpeed);
        }else{
          demoSpeed = 800;
        }
    }
  }
  
  return seg->speed; // return the delay until the next animation step (in msec)
}
uint16_t colorRacers(void) { // chase and clear
  WS2812FX::Segment* seg = ws2812fx.getSegment(); // get the current segment
  //set everyting in the tube to black
  for(int i = segment_1_START; i<=segment_1_STOP;i++)
  {
    switch(demoIndex)
    {
      case 0:
        ws2812fx.setPixelColor(i, RED);   
      break;
      case 1:
        ws2812fx.setPixelColor(i, GREEN);   
      break;
      case 2:
        ws2812fx.setPixelColor(i, BLUE);   
      break;
      case 3:
        ws2812fx.setPixelColor(i, PURPLE);   
      break;      
    }  
  }
  //now draw as far as we got in the last loop
  for(int i = segment_1_START; i<(segment_1_START+customEffect1_counter);i++)
  {
    switch(demoIndex)
    {
      case 0:
        ws2812fx.setPixelColor(i, BLACK);   
      break;
      case 1:
        ws2812fx.setPixelColor(i, RED);   
      break;
      case 2:
        ws2812fx.setPixelColor(i, GREEN);   
      break;
      case 3:
        ws2812fx.setPixelColor(i, BLUE);   
      break;      
    }      
  }

  //bump the counter
  customEffect1_counter ++;

  //reset to 0 if we reached the end
  if((segment_1_START+customEffect1_counter) > (segment_1_STOP+1))
  {
    customEffect1_counter = 0;
    //depending on the state, move quicker by value or list
    switch(state)
    {
      case 3:
          demoIndex++;
          demoSpeed = demoSpeed - 100;
          if(demoIndex > 3)
          {
            demoIndex = 0;
            demoSpeed = 500;
          }
        break;
    }
  }
  
  return seg->speed; // return the delay until the next animation step (in msec)
}
uint16_t countBack(void) { // chase and clear
  WS2812FX::Segment* seg = ws2812fx.getSegment(); // get the current segment
  //set everyting in the tube to black
  for(int i = segment_1_START; i<=segment_1_STOP;i++)
  {
    ws2812fx.setPixelColor(i, BLACK);   
  }
  //now draw as far as we got in the last loop
  for(int i = segment_1_START; i<(segment_1_START+customEffect1_counter);i++)
  {
    switch(demoIndex)
    {
      case 0:
        ws2812fx.setPixelColor(i, RED);   
      break;
      case 1:
        ws2812fx.setPixelColor(i, GREEN);   
      break;
      case 2:
        ws2812fx.setPixelColor(i, BLUE);   
      break;
      case 3:
        ws2812fx.setPixelColor(i, PURPLE);   
      break;      
    }      
  }

  //bump the counter
  customEffect1_counter --;

  //reset to 0 if we reached the end
  if(customEffect1_counter < 0)
  {
    customEffect1_counter = 7;   
  }
  
  return seg->speed; // return the delay until the next animation step (in msec)
}
uint16_t effect_game1(void) { // chase and clear
  WS2812FX::Segment* seg = ws2812fx.getSegment(); // get the current segment
  //set everyting in the tube to black
  for(int i = segment_1_START; i<=segment_1_STOP;i++)
  {
    ws2812fx.setPixelColor(i, BLACK);   
  }
  //now draw as far as we got in the last loop
  for(int i = segment_1_START; i<(segment_1_START+customEffect1_counter);i++)
  {
    ws2812fx.setPixelColor(i, RED);
  }

  //bump the counter
  customEffect1_counter ++;

  //game over on end
  if((segment_1_START+customEffect1_counter) > (segment_1_STOP+1))
  {
    state = 8;
  }
  return seg->speed; // return the delay until the next animation step (in msec)
}
uint16_t effect_game2(void) { // chase and clear
  WS2812FX::Segment* seg = ws2812fx.getSegment(); // get the current segment
  //set everyting in the tube to black
  for(int i = segment_1_START; i<=segment_1_STOP;i++)
  {
    ws2812fx.setPixelColor(i, BLACK);   
  }
  //now draw as far as we got in the last loop
  for(int i = segment_1_START; i<(segment_1_START+customEffect1_counter);i++)
  {
    //depending on the speed selected, choose chaser color:
    ws2812fx.setPixelColor(i, SP_colors[SP_index]);
  }

  //bump the counter
  customEffect1_counter ++;

  //game over on end
  if((segment_1_START+customEffect1_counter) > (segment_1_STOP+1))
  {
    state = 15;
  }
  return seg->speed; // return the delay until the next animation step (in msec)
}
uint16_t select_game3(void) { // chase and clear
  WS2812FX::Segment* seg = ws2812fx.getSegment(); // get the current segment
  //set everyting in the tube to black
  for(int i = segment_1_START; i<=segment_1_STOP;i++)
  {
    ws2812fx.setPixelColor(i, BLACK);   
  }
  
  //now draw as far as we got in the last loop
  int colorIndex = 0;
  for(int i = segment_1_START; i<(segment_1_START+MP_players);i++)
  {
    //depending on the speed selected, choose chaser color:
    ws2812fx.setPixelColor(i, MP_colors[colorIndex]);
    colorIndex++;
  }

  return seg->speed; // return the delay until the next animation step (in msec)
}
uint16_t countBackGame3(void) { // chase and clear
  WS2812FX::Segment* seg = ws2812fx.getSegment(); // get the current segment
  //set everyting in the tube to black
  for(int i = segment_1_START; i<=segment_1_STOP;i++)
  {
    ws2812fx.setPixelColor(i, MP_colors[MP_currentPlayer]);   
  }
  
  //set pixel of customEffect1_counter to black
  //
  ws2812fx.setPixelColor(segment_1_START + customEffect1_counter, BLACK);   

  //bump the counter
  customEffect1_counter --;

  //reset to 0 if we reached the end
  if(customEffect1_counter < 0)
  {
    customEffect1_counter = 7;   
  }
  
  return seg->speed; // return the delay until the next animation step (in msec)
}
uint16_t effect_game3(void) { // chase and clear
  WS2812FX::Segment* seg = ws2812fx.getSegment(); // get the current segment
  //set everyting in the tube to black
  for(int i = segment_1_START; i<=segment_1_STOP;i++)
  {
    ws2812fx.setPixelColor(i, BLACK);   
  }
  //now draw as far as we got in the last loop
  for(int i = segment_1_START; i<(segment_1_START+customEffect1_counter);i++)
  {
    if(MP_speed == MP_defaultSpeed) //if player is 0 or no fastest time is set
    {
      //rainbow
      int randomColor = random(8);
      ws2812fx.setPixelColor(i,MP_colors[randomColor]);
      
    }else{
      ws2812fx.setPixelColor(i, MP_colors[MP_leader]);
    }   
  }

  //bump the counter
  customEffect1_counter ++;

  //game over on end
  if((segment_1_START+customEffect1_counter) > (segment_1_STOP+1))
  {
    state = 22;
    
  }
  return seg->speed; // return the delay until the next animation step (in msec)
}