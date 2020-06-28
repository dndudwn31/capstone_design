#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

byte note = 0; //The MIDI note value to be played
byte resetMIDI = 4; //Tied to VS1053 Reset line
byte ledPin = 13; //MIDI traffic inidicator
int  instrument = 0;
// 60 =c, 62=d, 64 = e...
int drum[] =        {44,44,44,38,44,44,44,38,44,44,44,38,44,44,44,38,44,44,44,38,44,44,44,38,44,44,44,38,44,44,44,38,44,44,44,38,44,44,44,38,44,44,44,38,44,44,44,38,44,44,44,38,44,44,44,38,44,44,44,38};
int airplane[] =    {64,62,60,62,64,64,64, 0,62,62,62, 0,64,64,64, 0,64,62,60,62,64,64,64, 0,62,62,64,62,60, 0,64,62,60,62,64,64,64, 0,62,62,62, 0,64,64,64, 0,64,62,60,62,64,64,64, 0,62,62,64,62,60, 0};
int Megalovania[] = {52,52,64, 0,59,58,57,55,52,55,57,50,50,64, 0,59,58,57,55,52,55,57,49,48,64, 0,59,58,57,55,52,55,57, 0,49,48,64, 0,59,58,57,55,52,55,57, 0,52,52,64, 0,59,58,57,55,52,55,57, 0, 0, 0};
int Mario[] =       {64,64, 0,64, 0,60,64, 0,67, 0, 0,55, 0, 0,60, 0, 0,55, 0, 0,52, 0,57,59,57,55,64,67,69,65,67,64,60,62, 0,60, 0, 0, 0,55, 0, 0,52,57,59, 0,57,55,64,67,69,65,67,64,60,62, 0, 0, 0, 0};
int analogPin0 = 0; //음악 생성 및 박자 변화 측정값 센서
int analogPin1 = 2; //버전 결정을 위한 센서
int Vmode = 0; //음악 생성 or 박자 변화 선택
int Vcnt = 0;
int Mmode = 0; //멜로디 선택
int Mcnt = 0;
int Melody[3][60] = { {64,62,60,62,64,64,64, 0,62,62,62, 0,64,64,64, 0,64,62,60,62,64,64,64, 0,62,62,64,62,60, 0,64,62,60,62,64,64,64, 0,62,62,62, 0,64,64,64, 0,64,62,60,62,64,64,64, 0,62,62,64,62,60, 0},
                      {52,52,64, 0,59,58,57,55,52,55,57,50,50,64, 0,59,58,57,55,52,55,57,49,48,64, 0,59,58,57,55,52,55,57, 0,49,48,64, 0,59,58,57,55,52,55,57, 0,52,52,64, 0,59,58,57,55,52,55,57, 0, 0, 0},
                      {64,64, 0,64, 0,60,64, 0,67, 0, 0,55, 0, 0,60, 0, 0,55, 0, 0,52, 0,57,59,57,55,64,67,69,65,67,64,60,62, 0,60, 0, 0, 0,55, 0, 0,52,57,59, 0,57,55,64,67,69,65,67,64,60,62, 0, 0, 0, 0}
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Setup soft serial for MIDI control
  mySerial.begin(31250);

  //Reset the VS1053
  pinMode(resetMIDI, OUTPUT);
  digitalWrite(resetMIDI, LOW);
  delay(100);
  digitalWrite(resetMIDI, HIGH);
  delay(100);
  talkMIDI(0xB0, 0x07, 120); //0xB0 is channel message, set channel volume to near max (127)
}

void AnalogAD()
{
  for (int i = 0; i < 60; i++) {
    int reading0 = analogRead(analogPin0);
    int reading1 = analogRead(analogPin1);
    float Voltage0 = (float)reading0 / 1023 * 10;
    float Voltage1 = (float)reading1 / 1023 * 10;
    int note = 0;
    float del = pow(Voltage0, Voltage0) / 3 * 10;
    float del1 = pow(Voltage1, Voltage1) / 3 * 10;
    if (del > 700) del = 700;


    //mode 선택 센서 2번 값이 200이상 찍었다
    //if문으로 200이상찍엇고 mode 0 일경우 mode = 1 그리고 i =0
    //if문으로 200이상찍엇고 mode 1 일경우 mode = 0 그리고 i =0

    if (del1 > 300 && Vmode == 0 && del < 300 && Vcnt > 15) {
      Vmode = 1;
      noteOn(0, 80 , 70);
      i = 0;
      Vcnt = 0;
    }
    else if (del1 > 300 && Vmode == 1 && del < 300 && Vcnt > 15) {
      Vmode = 0;
      noteOn(0, 80 , 70);
      i = 0;
      Vcnt = 0;
    }

    Vcnt++;
    
    //Serial.println(Vmode);
    
    Serial.print(del);
    Serial.print(',');
    Serial.println(del1);
    
    
    if (del > 700) del = 700; // 최대값 설정
    if (del < 250) note = 0;
    if (del > 250 && del < 300) note = 60; //도
    else if (del > 300 && del < 350) note = 62; //레
    else if (del > 350 && del < 400) note = 64; //미
    else if (del > 400 && del < 450) note = 65; //파
    else if (del > 450 && del < 500) note = 67; //솔
    else if (del > 500 && del < 550) note = 69; //라
    else if (del > 550 && del < 600) note = 71; //시
    else if (del > 600 && del < 650) note = 72; //도
    else note = 74; //레
    //Serial.println(음표);

    if (Vmode == 0)
    {
      talkMIDI(0xB0, 0x07, 120);
      noteOn(9, drum[i], 50);
      if (del > 250) //소리가 날때, 250이하는 소리를 내지 않는다.
      {
        //Serial.println(음표);
        noteOn(0, note , 60);

        delay(200);
        noteOff(0, note, 60);
      }
      else {
        delay(200);
      }
    }

    else
    {
      if (del > 0) //소리안나도록
      {
        
        if (del1 > 300 && del > 300 && Mmode == 0 && Mcnt > 15) {
          Mmode = 1;
          //Serial.println("Megal");
          noteOn(9, drum[i], 50);        
          noteOn(0, Melody[Mmode][i], 60);
          delay(200 - del / 9);
          noteOff(0, Melody[Mmode][i], 60);
          Mcnt = 0;
        }
        else if (del1 > 300 && del > 300 && Mmode == 1 && Mcnt > 15) {
          Mmode = 2;
          //Serial.println("Mario");
          noteOn(9, drum[i], 50);   
          noteOn(0, Melody[Mmode][i], 60);
          delay(200 - del / 9);
          noteOff(0, Melody[Mmode][i], 60);
          Mcnt = 0;
        }
        else if (del1 > 300 && del > 300 && Mmode == 2 && Mcnt > 15) {
          Mmode = 0;
          //Serial.println("airplane");
          noteOn(9, drum[i], 50);   
          noteOn(0, Melody[Mmode][i], 60);
          delay(200 - del / 9);
          noteOff(0, Melody[Mmode][i], 60);
          Mcnt = 0;
        }
        else {
          //Serial.println("V2");
          noteOn(9, drum[i], 50);   
          noteOn(0,  Melody[Mmode][i], 60);
          delay(200 - del / 9);
          noteOff(0,  Melody[Mmode][i], 60);
          //delay(00);        
        }
        Mcnt ++;
        /*
          noteOn(9, drum[i], 70);
          noteOn(0, airplane[i] , 30);
          talkMIDI(0xC0, 67, 0);//instrument 교체
          noteOn(0, Megalovania[i], 30);
          delay(200 - del / 9);
          noteOff(0, Megalovania[i], 30);
        */
      }
      delay(20);
    }
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  AnalogAD();
}


//Send a MIDI note-on message.  Like pressing a piano key
//channel ranges from 0-15
void noteOn(byte channel, byte note, byte attack_velocity) {
  talkMIDI( (0x90 | channel), note, attack_velocity);
}

//Send a MIDI note-off message.  Like releasing a piano key
void noteOff(byte channel, byte note, byte release_velocity) {
  talkMIDI( (0x80 | channel), note, release_velocity);
}

//Plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that data values are less than 127
void talkMIDI(byte cmd, byte data1, byte data2) {
  digitalWrite(ledPin, HIGH);
  mySerial.write(cmd);
  mySerial.write(data1);

  //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes
  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
  if ( (cmd & 0xF0) <= 0xB0)
    mySerial.write(data2);

  digitalWrite(ledPin, LOW);
}
