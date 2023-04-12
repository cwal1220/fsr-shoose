
const int fsrAnalogPins[] = {A0, A1, A2, A3, A4};  // the FSR and 10K pulldown are connected to a0, a1, a2, a3, a4
const int ledPin = 9;

const float ACC_GRAVITY = 9.80665;
const int PULLUP_REGISTER_10K = 10000; // 10K resistor

// for moving average
const int movingAverageWindowSize = 10;
float movingAverageArray[movingAverageWindowSize] = {0,};
int index = 0;
float totalValue = 0;

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
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, 0);
}

void loop(void)
{
    float pressureNewtons = 0;
    float averageNewtons = 0;
    float weight = 0;
    pressureNewtons = getPressureNewtons(fsrAnalogPins[0]);

    totalValue = totalValue - movingAverageArray[index] + pressureNewtons;
    movingAverageArray[index] = pressureNewtons;
    index = (index + 1) % movingAverageWindowSize;
    averageNewtons = totalValue / movingAverageWindowSize;
    weight = averageNewtons / ACC_GRAVITY;

    if(weight > 1)
    {
        digitalWrite(ledPin, 1);
    }
    else
    {
        digitalWrite(ledPin, 0);
    }


    Serial.print("Result: ");
    Serial.print(averageNewtons);
    Serial.print("N, ");
    Serial.print(weight);
    Serial.print("kg \r\n");
    delay(100);
}