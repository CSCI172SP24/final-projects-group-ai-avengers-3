

//              _____               __      __  ______   _   _    _____   ______   _____     _____
//      /\     |_   _|       /\     \ \    / / |  ____| | \ | |  / ____| |  ____| |  __ \   / ____|
//     /  \      | |        /  \     \ \  / /  | |__    |  \| | | |  __  | |__    | |__) | | (___
//    / /\ \     | |       / /\ \     \ \/ /   |  __|   | . ` | | | |_ | |  __|   |  _  /   \___ \ 
//   / ____ \   _| |_     / ____ \     \  /    | |____  | |\  | | |__| | | |____  | | \ \   ____) |
//  /_/    \_\ |_____|   /_/    \_\     \/     |______| |_| \_|  \_____| |______| |_|  \_\ |_____/

#include <IRremote.h>

#define speedPinR 9         //  RIGHT PWM pin connect MODEL-X ENA
#define RightMotorDirPin1 8 // Right Motor direction pin 1 to MODEL-X IN1
#define RightMotorDirPin2 7 // Right Motor direction pin 2 to MODEL-X IN2
#define speedPinL 6         // Left PWM pin connect MODEL-X ENB
#define LeftMotorDirPin1 11 // Left Motor direction pin 1 to MODEL-X IN3
#define LeftMotorDirPin2 12 // Left Motor direction pin 1 to MODEL-X IN4
#define RECV_PIN 10
#define IR_UP 24
#define IR_DOWN 82
#define IR_LEFT 8
#define IR_RIGHT 90
#define IR_OK 28
#define IR_2 70
#define IR_3 71
#define IR_4 68
#define IR_5 64
#define ECHO_PIN 2
#define TRIG_PIN 3
#define MOVE_TIME 1000 // time to move forward
#define TURN_TIME 800  // time to turn
#define SPEED 190      // motor speed
#define PULSE_INTERVAL 500
#define IR_SENSE_LEFT 5
#define IR_SENSE_RIGHT 4
#define LFSensor_1 A4 // left
#define LFSensor_2 A3
#define LFSensor_3 A2
#define LFSensor_4 A1
#define LFSensor_5 A0 // right

IRrecv irrecv(RECV_PIN);

int state = 0; // 0: Manual Mode. 1: Wall detection mode. 2: Object Following

/*motor control*/
void go_Advance(void) // Forward
{
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2, HIGH);
  digitalWrite(LeftMotorDirPin1, LOW);
  digitalWrite(LeftMotorDirPin2, HIGH);
  analogWrite(speedPinL, 200);
  analogWrite(speedPinR, 200);
}
void go_Left(int t = 0) // Turn left
{
  digitalWrite(RightMotorDirPin1, HIGH);
  digitalWrite(RightMotorDirPin2, LOW);
  digitalWrite(LeftMotorDirPin1, LOW);
  digitalWrite(LeftMotorDirPin2, HIGH);
  analogWrite(speedPinL, 200);
  analogWrite(speedPinR, 200);
  delay(t);
}
void go_Right(int t = 0) // Turn right
{
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2, HIGH);
  digitalWrite(LeftMotorDirPin1, HIGH);
  digitalWrite(LeftMotorDirPin2, LOW);
  analogWrite(speedPinL, 200);
  analogWrite(speedPinR, 200);
  delay(t);
}
void go_Back(int t = 0) // Reverse
{
  digitalWrite(RightMotorDirPin1, HIGH);
  digitalWrite(RightMotorDirPin2, LOW);
  digitalWrite(LeftMotorDirPin1, HIGH);
  digitalWrite(LeftMotorDirPin2, LOW);
  analogWrite(speedPinL, 200);
  analogWrite(speedPinR, 200);
  delay(t);
}

void back_Right(int t = 0)
{
  digitalWrite(RightMotorDirPin1, HIGH);
  digitalWrite(RightMotorDirPin2, LOW);
  digitalWrite(LeftMotorDirPin1, HIGH);
  digitalWrite(LeftMotorDirPin2, LOW);
  set_Motorspeed(SPEED, 0);
  delay(t);
}

void back_Left(int t = 0)
{
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2, HIGH);
  digitalWrite(LeftMotorDirPin1, HIGH);
  digitalWrite(LeftMotorDirPin2, LOW);
  set_Motorspeed(0, SPEED);
  delay(t);
}
void stop_Stop() // Stop
{
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2, LOW);
  digitalWrite(LeftMotorDirPin1, LOW);
  digitalWrite(LeftMotorDirPin2, LOW);
}
/*set motor speed */
void set_Motorspeed(int speed_L, int speed_R)
{
  analogWrite(speedPinL, speed_L);
  analogWrite(speedPinR, speed_R);
}

// Pins initialize
void init_GPIO()
{
  pinMode(RightMotorDirPin1, OUTPUT);
  pinMode(RightMotorDirPin2, OUTPUT);
  pinMode(speedPinL, OUTPUT);

  pinMode(LeftMotorDirPin1, OUTPUT);
  pinMode(LeftMotorDirPin2, OUTPUT);
  pinMode(speedPinR, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(IR_SENSE_LEFT, INPUT);
  pinMode(IR_SENSE_RIGHT, INPUT);
  irrecv.enableIRIn();
  stop_Stop();
}

void IRCommand(int received)
{
  switch (received)
  {
    case IR_UP:
    {
      state = 0;
      go_Advance();
      break;
    }
    case IR_DOWN:
    {
      state = 0;
      go_Back();
      break;
    }
    case IR_LEFT:
    {
      state = 0;
      go_Left(400);
      stop_Stop();
      break;
    }
    case IR_RIGHT:
    {
      state = 0;
      go_Right(400);
      stop_Stop();
      break;
    }
    case IR_OK:
    {
      state = 0;
      stop_Stop();
      break;
    }
    case IR_2:
    {
      state = 0;
      move_in_square();
      break;
    }
    case IR_3:
      state = 1;
      go_Advance();
      break;
    case IR_4:
      state = 2;
      break;
  }
}

void move_in_square()
{
  for (int i = 0; i < 4; i++)
  {
    go_Advance();
    delay(MOVE_TIME);
    stop_Stop();
    delay(2000);
    go_Right(TURN_TIME);
    stop_Stop();
    delay(1000);
  }
  stop_Stop();
}

void detect_Walls()
{
  delay(PULSE_INTERVAL);
  unsigned long duration;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;
  if (distance <= 60)
  {
    int randNum = rand() % 1500 + 500;
    stop_Stop();
    delay(TURN_TIME);
    go_Right(randNum);
    go_Advance();
  }
}

void follow_Object()
{
  int IRLeft = digitalRead(IR_SENSE_LEFT);
  Serial.println(IRLeft);
  int IRRight = digitalRead(IR_SENSE_RIGHT);
  if(IRLeft == LOW && IRRight == LOW){
    go_Advance();
  } else if(IRLeft == HIGH && IRRight == HIGH) {
    stop_Stop();
  } else if(IRLeft == LOW && IRRight == HIGH){
    back_Right();
  } else if(IRLeft == HIGH && IRRight == LOW){
    back_Left();
  }
}

void setup()
{
  Serial.begin(9600);
  init_GPIO();
}

void loop()
{
  if (irrecv.decode())
  {
    int received = irrecv.decodedIRData.command;
    Serial.println(received);

    IRCommand(received);

    irrecv.resume();
  }

  switch (state)
  {
    case 1:
      detect_Walls();
      break;
    case 2:
      follow_Object();
      break;
    default:
      break;
  }
}
