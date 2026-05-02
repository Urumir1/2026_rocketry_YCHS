

#define BUZZPIN 4
#define DEVMODE 1


int ZACCELPIN = A6;

int zAxis = 0;
int yAxis = 0;
int xAxis = 0;

void setup() {
 

  pinMode(BUZZPIN, OUTPUT);
  xAxis = analogRead(A6);
  yAxis = analogRead(A7);
  zAxis = analogRead(A8);
  if (DEVMODE) {
    Serial.begin(9600);
    Serial.printf("Z: %i, Y: %i, X: %i\n", zAxis, yAxis, xAxis);
  }
  tone(BUZZPIN, xAxis, 200);
  delay(500);
  tone(BUZZPIN, yAxis, 200);
  delay(500);
  tone(BUZZPIN, zAxis, 200);
}

void loop() {
 

}
