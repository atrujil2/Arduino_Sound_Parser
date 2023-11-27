const int microphonePin = A0;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
}

void loop() {
  // put your main code here, to run repeatedly:
  int mn = 1024;
  int mx = 0;
  
  for (int i = 0; i < 1000; i++) {

    int val = analogRead(microphonePin);

    mn = min(mn, val);
    mx = max(mx, val);

  }

  int delta = mx - mn;
  Serial.print("\nMin = ");
  Serial.print(mn);
  Serial.print("\nMax = ");
  Serial.print(mx);
  Serial.print("\nDelta = ");
  Serial.print(delta);

}
