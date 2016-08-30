/*
 Name:		AccessSystem.ino
 Created:	2016/7/10 12:22:07
 Author:	Fing
*/

#include <SoftwareSerial.h>
#include <Servo.h>
#include <MFRC522.h>
#include <SPI.h>

/*�Զ���RC522������PIN��*/
#define SS_PIN 10
#define RST_PIN 9

#define BT_RX 3		//����ģ��˿�
#define BT_TX 2
#define SERVO_PIN 6		//����˿�
#define BUZZ_PIN 4
#define CARDS 2		//����֤�Ŀ�Ƭ��Ŀ

MFRC522	mfrc522(SS_PIN, RST_PIN);		//�½�RC522����
SoftwareSerial BT(BT_TX, BT_RX);		//�����������ڶ��󣬱����ͻ

Servo myservo;		//�����������
bool isAuthed = false;		//��֤�Ƿ�ͨ��
const byte AuthedID[CARDS][4] = { {0xFF, 0xFF, 0xFF, 0xFF}};  //���Ա�������ƬUIDֵ
char val;		//�����洢������������

void setup() {
	BT.begin(9600);		//�������ڣ�������9600
	myservo.attach(SERVO_PIN);		//���Ӷ��
	SPI.begin();		//��ʼ��SPI����
	mfrc522.PCD_Init();		//��ʼ��MFRC522��
	pinMode(BUZZ_PIN, OUTPUT);		//��ʼ��������
	digitalWrite(BUZZ_PIN, HIGH);
	myservo.write(45);
}

// the loop function runs over and over again until power down or reset
void loop()
{
	Authenticate();
	if (isAuthed) {
		OpenDoor();
		BeepChecked();
		delay(3000);
		isAuthed = false;
	}
	CloseDoor();
	isAuthed = false;
	BTCheck();
	delay(500);
}


//Servo������
void OpenDoor()
{
	myservo.write(140);
}

void CloseDoor()
{
	myservo.write(45);
}

//��֤��Ƭ
void Authenticate()
{
	//����Ƿ����¿�Ƭ
	if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
		delay(50);
		return;
	}

	//���п�Ƭ��֤
	for (byte num = 0; num < CARDS; num++)		//��Ƭѭ��  
	{
		byte i;
		for (i = 0; i < mfrc522.uid.size; i++)
		{
			if (mfrc522.uid.uidByte[i] != AuthedID[num][i]) break;
		}
		if (i == mfrc522.uid.size)
		{
			isAuthed = true;		//��֤ͨ��
			break;
		}
		else if (num == CARDS - 1) {
			BeepFailed();
		}
	}
}

//����ɨ��
void BTCheck() {
	if (BT.available()) {
		val = BT.read();
		if (val == '1') {
			OpenDoor();
			BeepChecked();
			delay(3000);
			isAuthed = false;
		}
		val = 0;
	}
}

void BeepChecked() {
	digitalWrite(BUZZ_PIN, LOW);
	delay(200);
	digitalWrite(BUZZ_PIN, HIGH);
}

void BeepFailed() {
	digitalWrite(BUZZ_PIN, LOW);
	delay(100);
	digitalWrite(BUZZ_PIN, HIGH);
	delay(100);
	digitalWrite(BUZZ_PIN, LOW);
	delay(100);
	digitalWrite(BUZZ_PIN, HIGH);
}