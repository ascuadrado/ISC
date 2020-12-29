class FullDisplay {
  int margen = 15;

  void drawBattery(float percentage){
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


  void drawAlert(boolean on){
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

  void drawSpeed(int speed){
    textFont(digital);
    textAlign(RIGHT, BOTTOM);
    fill(255);
    text(String.valueOf(speed), 100, 191, 420, 250);
    textSize(50);
    textAlign(LEFT, BOTTOM);
    text("KM/H", 520, 333, 200, 100);
  }

  void drawPower(float power){
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

  void drawBatteryV(float voltage){
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
