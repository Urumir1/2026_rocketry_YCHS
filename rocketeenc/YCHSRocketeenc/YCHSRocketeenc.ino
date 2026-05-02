#define BUZZPIN 4
int ZACCELPIN = A6;

int zAxis = 0;
int yAxis = 0;
int xAxis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUZZPIN, OUTPUT);
  //pinMode()
}

void loop() {
  xAxis = analogRead(A6);
  yAxis = analogRead(A7);
  zAxis = analogRead(A8);
  Serial.printf("Z: %i, Y: %i, X: %i\n", zAxis, yAxis, xAxis);
  tone(BUZZPIN, xAxis, 200);
  delay(500);
  tone(BUZZPIN, yAxis, 200);
  delay(500);
  tone(BUZZPIN, zAxis, 200);
  delay(500);
  //noTone(BUZZPIN);
  //delay(500);
  

}
