import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import processing.net.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class sketch_0_test extends PApplet {

 

Client c; 
JSONObject json;

PFont digital;
PFont roboto;

Data        data;
FullDisplay display;

public void setup()
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

    //size(800, 480);
    noCursor();
    
    
    // Output more information
    data.testing = true;

    println("Setup Done");
}


public void draw()
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
class Data {
    boolean testing = false;
    
    // Important data
    float vTotal = 119;
    float speed        = 225;
    float torque = 0.5f;
    float throttle     = 0.2f;
    float stateOfCharge = 0.45f;
    String opMode = "Testing";
    
    // Testing data
    float maxCellV = 3.251f;
    float minCellV = 3.345f;
    float maxTemp = 21;
    float capacitorV = 85;
    float velocity = 3150;
    float current = 0;

    public void update()
    {        
        c.write("request Data\n");
        
        while(c.available()==0){
          delay(1);
        }
        
        String data = c.readString();
        data = data.substring(0, data.indexOf("\n"));
        print(data);
        
        try{
          json = parseJSONObject(data);
          
          // Important data
          vTotal = json.getFloat("vTotal");
          speed = json.getFloat("speed");
          torque = json.getFloat("torque");
          throttle = json.getFloat("throttle");
          stateOfCharge = json.getFloat("stateOfCharge");
          opMode = json.getString("opMode");
          
          // Testing data
          maxCellV = json.getFloat("maxCellV");
          minCellV = json.getFloat("minCellV");
          maxTemp = json.getFloat("maxTemp");
          capacitorV = json.getFloat("capacitorV");
          velocity = json.getFloat("velocity");
          current = json.getFloat("current");
          
        } catch (Exception e){
          println(e);
        }
    }
}
class FullDisplay {
  int margen = 15;

  public void drawBattery(float percentage){
    int batWidth = 50;

    // Green bar
    fill(0,255,71);
    noStroke();
    rect(margen, height-margen, batWidth, -(height-2*margen), batWidth/2);
    fill(0);
    rect(margen, margen, batWidth, (height-2*margen)*(1-percentage));
    // Edge of shape
    stroke(255);
    strokeWeight(4);
    noFill();
    rect(margen, height-margen, batWidth, -(height-2*margen), batWidth/2);

    // Percentage indicators
    strokeWeight(1);
    fill(255);
    textFont(roboto);
    textSize(24);
    textAlign(RIGHT);
    for (int i = 0; i <= 10; i++) {
      int y = margen+batWidth/2 + i*39;
      line(margen+7, y, margen+batWidth-7, y);
      text(String.format("%d",(10-i)*10)+" %", margen+batWidth+12+65, y+10);
    }
  }


  public void drawAlert(boolean on){
    int x = margen+170;
    int y = margen+5;
    int w = 125;
    int h = 125;

    if(!on){
      fill(10);
      noStroke();
      rect(x,y,w,h);
    }else{
      fill(100);
      noStroke();
      //rect(x,y,w,h);
      PShape s;
      s = loadShape("Alert.svg");
      shape(s, x, y, w, h);
    }
  }

  public void drawSpeed(int speed){
    textFont(digital);
    textAlign(RIGHT, BOTTOM);
    fill(255);
    text(String.valueOf(speed), 100, 191, 420, 250);
    textSize(50);
    textAlign(LEFT, BOTTOM);
    text("KM/H", 520, 333, 200, 100);
  }

  public void drawPower(float torque, float throttle){
    int h = 250;
    int powerW = 50;

    // POWER
    textFont(digital);
    textSize(24);
    textAlign(CENTER, CENTER);
    text("POWER", 670, 130, 130, 60);

    // Torque meter
    fill(0,255,240);
    noStroke();
    rect(710, 190+h, powerW, -h, powerW/2);
    fill(0);
    rect(710, 190, powerW, h*(1-torque));
    stroke(255);
    strokeWeight(4);
    noFill();
    rect(710, 190, powerW, h, powerW/2);
    
    // Throttle indicator
    stroke(255);
    strokeWeight(2);
    line(710+powerW/2-40, 190+h*(1-throttle), 710+powerW/2+40, 190+h*(1-throttle));
  }

  public void drawBatteryV(float voltage){
    textFont(digital);
    fill(0,255,71);
    textAlign(LEFT, CENTER);
    textSize(50);
    text(String.format("%.1f",voltage)+"V", 320, 20, 250, 80);
    textSize(22);
    fill(255);
    text(String.format("MAX: %.0fV", maxVoltage), 590, 20, 200, 40);
    text(String.format("MIN: %.0fV", minVoltage), 590, 60, 200, 40);
  }
  
  
  public void drawOpMode(String mode){
    textFont(digital);
    fill(255);
    textAlign(CENTER, CENTER);
    textSize(50);
    text(mode, 149, 141, 323, 96);
    textSize(40);
  }
  
  public void drawTestingData(){
    textFont(roboto);
    fill(255);
    textAlign(LEFT, TOP);
    textSize(18);
    String fullText = "";
    fullText += "Testing data:\n\n";
    fullText += "maxCellV: " + String.format("%.3f", data.maxCellV)+" v\n";
    fullText += "minCellV: " + String.format("%.3f", data.minCellV)+" v\n";
    fullText += "maxTemp: " + String.format("%.3f", data.maxTemp)+" ºC\n";
    fullText += "capacitorV: " + String.format("%.1f", data.capacitorV)+" v\n";
    fullText += "velocity: " + String.format("%.0f", data.velocity)+" rpm\n";
    text(fullText, 507, 124, 184, 224);
  }

}
static float maxVoltage = 4.2f*30;
static float minVoltage = 2.8f*30;
static int margen = 15;
  public void settings() {  fullScreen(); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "sketch_0_test" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
