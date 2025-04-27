#include "var.h"

// Definisi variabel
BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
BLECharacteristic *pRxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
bool dataReceived = false;

HardwareSerial SerialPort(2);

String receivedData;
const char *receivedDataconvert;

ProgramState currentState = WAITING_FOR_INPUT;

String dataIn;
String dataInBLE;
String dataReceive;

bool dataSendBLE;
bool dataSendD;
bool inputFlag;
bool tareFlag;
bool srinseFlag;
bool frinseFlag;
bool receiveFlag; 

int totalVolume = 0;
int numSteps = 0;

int countwater = 0;
int countbrew = 0;
int countwait = 0;

double Setpoint = 0, NilaiSuhu = 0, Output = 0;

String namaResep; 
int pouringVolumes[10] = {0};
int pouringDurations[10] = {0};
int pouringIntervals[10] = {0};

bool parsing = false;
bool parsingFlag = false;

int TotalLoadCell = 0;

float sum = 0.0;
unsigned long t = 0;
int motorSpeed = 0;

int slave_1_id = 1;

uint32_t lastCommand = 0;

String dt[10];
int i = 0;

float average = 0.0;

int pourstep = 0;
int stepStartTime = 0;

int PreviousMillisSend = 0;
unsigned long previousMillis = 0;
const long interval = 500;
unsigned long previousMillis_print = 0;
int previousheatingmillis = 0;
int currentheatingmillis = 0;
int total_time = 0;

double Kp = 1.0, Ki = 0.5, Kd = 0.1;

dimmerLamp dimmer(outputPin, zerocross);

MAX6675 thermocouple(thermoCLK, thermoCS, thermoSO);

PID myPID(&NilaiSuhu, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

HX711_ADC LoadCell(HX711_dout, HX711_sck);