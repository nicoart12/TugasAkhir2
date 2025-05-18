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

bool errorNameFlag;
bool errorSetpointFlag;
bool errorVolumeFlag;
bool errorPouringFlag;
bool errorIntervalFlag;


//untuk update temp
uint32_t time_read_temp = 0;
int currentIndex = 0;
float temp_raw[numReadings];
float temp_avg = 0;
//const int numReadings = 20; //jumlah data yang dibaca

int totalVolume = 0;
int numSteps = 0;

int countwater = 0;
int countbrew = 0;
int countwait = 0;

//untuk counter step
int countStep = 0;

int blockvolume = 220;

double Setpoint = 0;
double NilaiSuhu = 0;

//UNTUK CONTROL MOTOR
bool lessSpeed;
int pourIntervalLess;

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


dimmerLamp dimmer(outputPin, zerocross);

MAX6675 thermocouple(thermoCLK, thermoCS, thermoSO);


HX711_ADC LoadCell(HX711_dout, HX711_sck);