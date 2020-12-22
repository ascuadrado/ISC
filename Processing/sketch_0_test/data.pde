class Data {
    float totalVoltage = 0;
    float current      = 0;
    float speed        = 119;
    float throttle     = 0;

    void update()
    {
        json = loadJSONObject(fileName);
        JSONArray  BMS         = json.getJSONArray("BMS");
        JSONObject BMS1        = BMS.getJSONObject(0);
        JSONObject BMS2        = BMS.getJSONObject(1);
        JSONObject BMS3        = BMS.getJSONObject(2);
        JSONArray  BMS1_v_json = BMS1.getJSONArray("cellVoltagemV");
        JSONArray  BMS2_v_json = BMS2.getJSONArray("cellVoltagemV");
        JSONArray  BMS3_v_json = BMS3.getJSONArray("cellVoltagemV");
        int[] BMS1_v = BMS1_v_json.getIntArray();
        int[] BMS2_v = BMS2_v_json.getIntArray();
        int[] BMS3_v = BMS3_v_json.getIntArray();

        int totalV = 0;
        for (int i = 0; i < 12; i++)
        {
            totalV += BMS1_v[i];
        }
        for (int i = 0; i < 12; i++)
        {
            totalV += BMS2_v[i];
        }
        for (int i = 0; i < 12; i++)
        {
            totalV += BMS3_v[i];
        }

        totalVoltage = 1.0 * totalV / 1000;

        JSONObject SEVCON = json.getJSONObject("SEVCON");
        int        tpdo1  = SEVCON.getInt("TPDO1_1");
        throttle = 1.0 * tpdo1 / 32768;
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
