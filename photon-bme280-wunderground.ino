
#include "SparkFunBME280/SparkFunBME280.h"

//Global sensor object
BME280 mySensor;

float humidity = 999;
float tempF = 999;
float pascals = 999;
float inches = 999;


char SERVER[] = "rtupdate.wunderground.com";        //Rapidfire update server - for multiple sends per minute
char WEBPAGE[] = "GET /weatherstation/updateweatherstation.php?";

//Station Identification
char ID [] = "KCASANFR741"; //Your station ID here
char PASSWORD [] = "xxxxxxxx"; //your Weather Underground password here

TCPClient client;

void setup()
{
	//***Driver settings********************************//
	mySensor.settings.commInterface = I2C_MODE;
	mySensor.settings.I2CAddress = 0x76; //default is 0x77

	//***Operation settings*****************************//
	mySensor.settings.runMode = 3; //Normal mode

	//Standby can be:  0, 0.5ms - 1, 62.5ms - 2, 125ms -  3, 250ms - 4, 500ms - 5, 1000ms - 6, 10ms - 7, 20ms
	mySensor.settings.tStandby = 0;
	mySensor.settings.filter = 4;

  //*OverSample can be: 0, skipped - 1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	mySensor.settings.tempOverSample = 5;
  	mySensor.settings.pressOverSample = 5;
	mySensor.settings.humidOverSample = 5;

	Serial.begin(57600);
	Serial.print("Program Started\n");
	
	delay(10);              //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
	mySensor.begin();       //Calling .begin() causes the settings to be loaded
}

void loop()
{
  getData();
  confirm();
  sendToWU();
  Serial.println();
  delay(2000);
}


//---------------------Function Junction --------------------------//

void getData(){
//Each loop, take a reading.
//Start with temperature, as that data is needed for accurate compensation.
//Reading the temperature updates the compensators of the other functions
//in the background.

// Measure Temperature
tempF = mySensor.readTempF();

// Measure Relative Humidity
humidity = mySensor.readFloatHumidity();

//Measure Pressure
pascals = mySensor.readFloatPressure();
inches = pascals * 0.0002953; // Calc for converting Pa to inHg (Wunderground expects inHg)

}

void confirm(){
  //Sends the sensor variables to serial to check
  Serial.print("Temperature: ");
  Serial.print(tempF);
  Serial.println(" degrees F");

  Serial.print("Pressure: ");
  Serial.print(inches);
  Serial.println(" Pa");

  Serial.print("%RH: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("NOWNOW: ");
}

void sendToWU()
{
  Serial.println("connecting...");

  if (client.connect(SERVER, 80)) {
  Serial.println("Connected");
  client.print(WEBPAGE);
  client.print("ID=");
  client.print(ID);
  client.print("&PASSWORD=");
  client.print(PASSWORD);
  client.print("&dateutc=now");      //can use 'now' instead of time if sending in real time
  client.print("&tempf=");
  client.print(tempF);
  client.print("&humidity=");
  client.print(humidity);
  client.print("&baromin=");
  client.print(inches);
  client.print("&softwaretype=Particle-Photon&action=updateraw&realtime=1&rtfreq=5");  //Rapid Fire update rate - for sending multiple times per minute, specify frequency in seconds
  client.println();
  Serial.println("Upload complete");
  delay(200);                         // Without the delay it is unreliable
  }else{
    Serial.println(F("Connection failed"));
  return;
  }
}
