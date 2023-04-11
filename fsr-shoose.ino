/* FSR testing sketch.

Connect one end of FSR to power, the other end to Analog 0.
Then connect one end of a 10K resistor from Analog 0 to ground

For more information see www.ladyada.net/learn/sensors/fsr.html */

int fsrAnalogPins[5] = {A0, A1, A2, A3, A4};  // the FSR and 10K pulldown are connected to a0, a1, a2, a3, a4
int PULLUP_REGISTER_10K = 10000; // 10K resistor

float getPressureNewtons(int analogPin)
{
    int fsrReading;              // the analog reading from the FSR resistor divider
    int fsrVoltage;              // the analog reading converted to voltage
    unsigned long fsrResistance; // The voltage converted to resistance, can be very big so make "long"
    unsigned long fsrConductance;
    long fsrForce; // Finally, the resistance converted to force

    fsrReading = analogRead(analogPin);

    // analog voltage reading ranges from about 0 to 1023 which maps to 0V to 5V (= 5000mV)
    fsrVoltage = map(fsrReading, 0, 1023, 0, 5000); // 

    if (fsrVoltage == 0)
    {
        return 0;
    }
    else
    {
        // The voltage = Vcc * R / (R + FSR) where R = 10K and Vcc = 5V
        // so FSR = ((Vcc - V) * R) / V        yay math!
        fsrResistance = 5000 - fsrVoltage; // fsrVoltage is in millivolts so 5V = 5000mV
        fsrResistance *= PULLUP_REGISTER_10K;            
        fsrResistance /= fsrVoltage; // FSR resistance in ohms

        fsrConductance = 1000000; // we measure in micromhos so
        fsrConductance /= fsrResistance; // Conductance in microMhos

        // Use the two FSR guide graphs to approximate the force
        if (fsrConductance <= 1000)
        {
            fsrForce = fsrConductance / 80; // Force in Newtons
        }
        else
        {
            fsrForce = fsrConductance - 1000;
            fsrForce /= 30; // Force in Newtons
        }
        return fsrForce;
    }
}

void setup(void)
{
    Serial.begin(9600);
}

void loop(void)
{
    float totalPressureNewtons = 0;
    for(int i=0; i<5; i++)
    {
        totalPressureNewtons += getPressureNewtons(fsrAnalogPins[i]);
    }
    Serial.print("Force in Newtons: ");
    Serial.println(totalPressureNewtons);
    delay(1000);
}