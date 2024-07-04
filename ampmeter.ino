// ADC PINOUT
#define A_LEAD A0
#define B_LEAD A1

#define VOLTAGE_MAX 5
#define ADC_RESOLUTION 10

#define REFERENCE_RESISTOR 1000

int read_A = 0;
int read_B = 0;
float voltage_across = 0.0;
float current_across = 0.0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Demo differential voltage");
}

void loop() {
  // Read the voltage value from A0 -> A1
  read_A = analogRead(A_LEAD);
  read_B = analogRead(B_LEAD);

  // Calculate voltage
  voltage_across = (float) ((read_A - read_B)*VOLTAGE_MAX)/(pow(2, ADC_RESOLUTION) - 1);
  current_across = (float) (voltage_across / REFERENCE_RESISTOR);

  // Send via serial
  Serial.print(read_A);
  Serial.print("  ");
  Serial.print(read_B);
  Serial.print("  ");
  Serial.print(voltage_across);
  Serial.print("  ");
  Serial.print(current_across*1000); // To show mA multiply by 1000
  Serial.println("mA");

}
