class Data {
    float totalVoltage = 0;
    float current      = 0;
    float speed        = 119;
    float throttle     = 0;

    void update()
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

    void display()
    {
        int   margen     = 20;
        float batW       = 50;
        float batPercent = (totalVoltage - minVoltage) / (maxVoltage - minVoltage);
        float speedW     = 1.0 * 25 / 100; // %

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
