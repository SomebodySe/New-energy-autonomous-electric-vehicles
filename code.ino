//发动机接口
#define LF1 13
#define LB1 27
#define RF1 17
#define RB1 16
#define LED 2

#define LF2 14
#define LB2 26
#define RF2 5
#define RB2 4

#define LF 32
#define LB 33
#define RF 18
#define RB 19
//探测器接口
#define sensor1 34
#define sensor2 35
#define sensor3 39
#define sensor4 36
//测距仪接口
#define sensorI 25
#define sensorO 23

int sensor[4] = {LOW,LOW,LOW,LOW};
int i = 0,k=0;
float l = 0;//计数或计时

void setup() //初始化部分
{
  Serial.begin(9600);
  SensorInit();
  MotorInit();
  ledcInit();
}

void loop() //循环部分（程序运行时持续调用）
{
  Sensor();
  avoid();
  run();
  stop();
  Distance();
  //delay(500);
}

void ledcInit() //初始化
{
  ledcAttachPin(LF, 1);
  ledcAttachPin(LB, 2);
  ledcAttachPin(RF, 3);
  ledcAttachPin(RB, 4);
  ledcSetup(1, 2000, 8); 
  ledcSetup(2, 2000, 8);
  ledcSetup(3, 2000, 8);
  ledcSetup(4, 2000, 8);
}
void SensorInit() //初始化
{
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);
  pinMode(sensorI, INPUT);
  pinMode(sensorO, OUTPUT);
}
void MotorInit() //初始化
{
  pinMode(LF1, OUTPUT);
  pinMode(LB1, OUTPUT);
  pinMode(RF1, OUTPUT);
  pinMode(RB1, OUTPUT);
  pinMode(LF2, OUTPUT);
  pinMode(LB2, OUTPUT);
  pinMode(RF2, OUTPUT);
  pinMode(RB2, OUTPUT);
  pinMode(LF, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(RB, OUTPUT);
  pinMode(LED, OUTPUT);
}

float Distance() //返回障碍物距离
{
  digitalWrite(sensorO, LOW);
  delayMicroseconds(4);
  digitalWrite(sensorO, HIGH);
  delayMicroseconds(20);
  digitalWrite(sensorO, LOW);
  l = pulseIn(sensorI, HIGH,20000)/58.0;
  //if(l!=0)
  //Serial.println("sensorI:"+String(sensorI)+"  sensorO:"+String(sensorO)+"  Distance:" + String(l));
  //delay(1000);
  return l;
}

void Sensor()
/*探测黑线函数，从左到右为sensor[0]到sensor[3]，探测到黑线为值为0，否则值为1*/
{
  sensor [0] = digitalRead(sensor1);
  sensor [1] = digitalRead(sensor2);
  sensor [2] = digitalRead(sensor3);
  sensor [3] = digitalRead(sensor4);
}

void Motor_Speed(int lf, int lb, int rf, int rb)
/*四轮速度设置函数，四个参数顺序为左前、左后、右前、右后*/
{
  if(lf>0)
  {
    digitalWrite(LF1, HIGH);
    digitalWrite(LF2, LOW);
  }
  else
  {
    digitalWrite(LF2, HIGH);
    digitalWrite(LF1, LOW);
  }
  if(lb>0)
  {
    digitalWrite(LB1, HIGH);
    digitalWrite(LB2, LOW);
  }
  else
  {
    digitalWrite(LB2, HIGH);
    digitalWrite(LB1, LOW);
  }
  if(rf>0)
  {
    digitalWrite(RF1, HIGH);
    digitalWrite(RF2, LOW);
  }
  else
  {
    digitalWrite(RF2, HIGH);
    digitalWrite(RF1, LOW);
  }
  if(rb>0)
  {
    digitalWrite(RB1, HIGH);
    digitalWrite(RB2, LOW);
  }
  else
  {
    digitalWrite(RB2, HIGH);
    digitalWrite(RB1, LOW);
  }
  ledcWrite(1, abs(lf));
  ledcWrite(2, abs(lb));
  ledcWrite(3, abs(rf));
  ledcWrite(4, abs(rb));
}

void avoid()//避障
{
  if(Distance()<6.0)
  {
    Motor_Speed(-120, -120, -120, -120);
    delay(200);
    Motor_Speed(120, -120, -120, 120);
    delay(700);
    Motor_Speed(120, 120, 120, 120);
    delay(1000);
    Motor_Speed(-120, 120, 120, -120);
    delay(600);
    sensor[0]=1;sensor[1]=1;sensor[2]=1;sensor[3]=1;
  }
}

void run()//主控函数
{
  if (sensor [0] == 0 && sensor [1] == 0 && sensor [2] == 1 && sensor [3] == 1)
    Motor_Speed(0, 0, 180, 180);//左急
  else if (sensor [0] == 0 && sensor [1] == 1 && sensor [2] == 1 && sensor [3] == 1)
    Motor_Speed(0, 0, 240, 240); //左急急急
  else if (sensor [0] == 0 && sensor [1] == 0 && sensor [2] == 0 && sensor [3] == 1)
    Motor_Speed(60, 60, 160, 160); //左缓
  else if (sensor [0] == 1 && sensor [1] == 1 && sensor [2] == 1 && sensor [3] == 0)
    Motor_Speed(240, 240, 0, 0); //右急急急
  else if (sensor [0] == 1 && sensor [1] == 1 && sensor [2] == 0 && sensor [3] == 0)
    Motor_Speed(180, 180, 0, 0); //右急
  else if (sensor [0] == 1 && sensor [1] == 0 && sensor [2] == 0 && sensor [3] == 0)
    Motor_Speed(160, 160, 60, 60);//右缓
  
  /*else if (sensor [0] == 0 && sensor [1] == 0 && sensor [2] == 0 && sensor [3] == 0)
    Delay();
  /*else if (sensor [0] == 0 && sensor [1] == 1 && sensor [2] == 1 && sensor [3] == 1)
    Motor_Speed(120, -120, -120, 120);//右平*/
  else if (sensor [0] == 1 && sensor [1] == 0 && sensor [2] == 0 && sensor [3] == 1)
    Motor_Speed(150, 150, 150, 150);//直走
  else
    sensor[0]=sensor[0];
}

void stop()//T字路口停车函数
{
  if (i > 20)
  {
    Motor_Speed(0, 0, 0, 0);
    //i = 0;
  }
  else if (sensor [0] == 0 && sensor [1] == 0 && sensor [2] == 0 && sensor [3] == 0) 
  {
    i++;
    Serial.println(String(i));
    Motor_Speed(0, 0, 0, 0);
    delay(10); //延时可以防止i加的过快
  }
  /*else if (sensor [0] == 1 && sensor [1] == 0 && sensor [2] == 0 && sensor [3] == 1) 
  {
    i++;
    delay(100);
  }
  else if (sensor [0] == 0 && sensor [1] == 0 && sensor [2] == 0 && sensor [3] == 1)
  {
    i++;
    delay(100);
  }
  else if (sensor [0] == 1 && sensor [1] == 0 && sensor [2] == 0 && sensor [3] == 0) 
  {
    i++;
    delay(100);
  }*/

  /*if (sensor [0] == 0 && sensor [1] == 0 && sensor [2] == 0 && sensor [3] == 0)
  {
    k=1;
    if ((sensor [0] == 1 && sensor [1] == 1 && sensor [2] == 1 && sensor [3] == 1) && k == 1)
    {
      Motor_Speed(0, 0, 0, 0);
      k=0;
    }
  }*/
}
