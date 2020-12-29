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

float maxVoltage = 135.0f;
float minVoltage = 80.0f;

PFont digital;
PFont roboto;

Data        data    = new Data();
FullDisplay display = new FullDisplay();

public void printTime()
{
    print(hour());
    print(":");
    print(minute());
    print(":");
    println(second());
}


public void setup()
{
    printTime();
    digital = createFont("Digital.ttf", 150);
    roboto  = createFont("Roboto-Regular.ttf", 24);
    c = new Client(this, "127.0.0.1", 50007);  // Connect to server on port 80
    json = new JSONObject();

    
    //noCursor();
    //fullScreen();

    println("Setup Done");
}


public void draw()
{
    background(0);

    data.update();


    //display.drawBattery(1-1.0*mouseX/700);
    display.drawBattery((data.totalVoltage - minVoltage) / (maxVoltage - minVoltage));
    display.drawAlert(true);
    display.drawSpeed((int)data.speed);
    display.drawPower(data.throttle);
    display.drawBatteryV(data.totalVoltage);
    //println(frameRate);

    //rect(0,0, totalV, totalV);

    //text("Total Voltage detected: ", 10,20);
    //text(1.0/1000, 10, 40);
}
class Data {
    float totalVoltage = 0;
    float current      = 0;
    float speed        = 119;
    float throttle     = 0;

    public void update()
    {
      print("data request - ");
        totalVoltage = 129;
        c.write("request Data\n");
        while(c.available()==0){
          delay(1);
          print(1);
        }
        String data = c.readString();
        data = data.substring(0, data.indexOf("\n"));
        print(data);
        try{
          json = parseJSONObject(data);
          totalVoltage = json.getFloat("vtotal");
          speed = json.getFloat("speed");
          throttle = json.getFloat("power");
          print(throttle);
        } catch (Exception e){
          println(e);
        }
        println();
    }

    public void display()
    {
        int   margen     = 20;
        float batW       = 50;
        float batPercent = (totalVoltage - minVoltage) / (maxVoltage - minVoltage);
        float speedW     = 1.0f * 25 / 100; // %

        // Battery
        fill(0, 255, 71);
        noStroke();
        rect(margen, height - margen, batW, -(height - 2 * margen) * batPercent, 0, 0, batW / 2, batW / 2);

        stroke(255);
        strokeWeight(4);
        noFill();
        rect(margen, margen, batW, height - 2 * margen, batW / 2);


        // Text
        fill(255);
        textSize(16);
        textAlign(CENTER);
        text(String.format("%.2f", batPercent * 100) + "%", margen + batW / 2, margen + 20);

        // Speedometer
        noFill();
        stroke(255);
        strokeWeight(4);
        //float d = 2*(width-4*margen-batW*width);
        float d = (height - 2 * margen);
        arc(width - margen, height - margen, 2 * d, 2 * d, PI, PI + HALF_PI);
        arc(width - margen, height - margen, 2 * (d - height * speedW), 2 * (d - height * speedW), PI, PI + HALF_PI);
        line(width - margen, margen, width - margen, margen + speedW * height);
        line(width - margen - d, height - margen, width - margen - d + speedW * height, height - margen);
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

  public void drawPower(float power){
    int h = 250;
    int powerW = 50;

    // POWER
    textFont(digital);
    textSize(24);
    textAlign(CENTER, CENTER);
    text("POWER", 670, 130, 130, 60);

    // Power meter
    fill(0,255,240);
    noStroke();
    rect(710, 190+h, powerW, -h, powerW/2);
    fill(0);
    rect(710, 190, powerW, h*(1-power));
    stroke(255);
    strokeWeight(4);
    noFill();
    rect(710, 190, powerW, h, powerW/2);
  }

  public void drawBatteryV(float voltage){
    textFont(digital);
    fill(0,255,71);
    textAlign(LEFT, CENTER);
    textSize(50);
    text(String.format("%.1f",voltage)+"V", 320, 20, 250, 80);
    textSize(22);
    fill(255);
    text("MAX: 120V", 590, 20, 200, 40);
    text("MIN:  80V", 590, 60, 200, 40);
  }


}
  public void settings() {  size(800, 480); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "sketch_0_test" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
