JSONObject json;
String fileName = "/home/pi/Desktop/Rasp-main/datos.json";

float maxVoltage = 135.0;
float minVoltage = 80.0;

PFont digital;
PFont roboto;

Data data = new Data();
FullDisplay display = new FullDisplay();

void printTime(){
  print(hour());
  print(":");
  print(minute());
  print(":");
  println(second());
}

void setup() {
  printTime();
  digital = createFont("Digital.ttf", 150);
  roboto = createFont("Roboto-Regular.ttf", 24);

  //size(800,480);
  noCursor();
  fullScreen();
  
  println("Setup Done");
}

void draw(){
  background(0);
  
  data.update();
  
  //display.drawBattery(1-1.0*mouseX/700);
  display.drawBattery((data.totalVoltage-minVoltage)/(maxVoltage-minVoltage));
  display.drawAlert(true);
  display.drawSpeed((int)data.speed);
  display.drawPower(data.throttle);
  display.drawBatteryV(data.totalVoltage);
  
  //rect(0,0, totalV, totalV);

  //text("Total Voltage detected: ", 10,20);
  //text(1.0/1000, 10, 40);
}
