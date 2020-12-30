import processing.net.*; 

Client c; 
JSONObject json;

PFont digital;
PFont roboto;

Data        data;
FullDisplay display;

void setup()
{
    
    try {
      digital = createFont("Digital.ttf", 150);
      roboto  = createFont("Roboto-Regular.ttf", 24);
      c = new Client(this, "127.0.0.1", 50007);  // Connect to server on port 80
    } catch (Exception e){
      println(e);
    }
    
    json = new JSONObject();
    data    = new Data();
    display = new FullDisplay();

    size(800, 480);
    //noCursor();
    //fullScreen();
    
    // Output more information
    data.testing = true;

    println("Setup Done");
}


void draw()
{
    background(0);

    data.update();

    display.drawBattery(data.stateOfCharge);
    display.drawAlert(true);
    display.drawSpeed((int)data.speed);
    display.drawPower(data.torque, data.throttle);
    display.drawBatteryV(data.vTotal);
    display.drawOpMode(data.opMode);
    if(data.testing){
      display.drawTestingData();
    }
    
    //display.drawBattery(1-1.0*mouseX/700);
    println(frameRate);
}
