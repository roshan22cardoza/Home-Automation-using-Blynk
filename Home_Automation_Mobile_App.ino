#define BLYNK_TEMPLATE_ID "TMPL6kq55FW_O"
#define BLYNK_TEMPLATE_NAME "Home auto"
#define BLYNK_AUTH_TOKEN "i1m_4iMldFbKm21dRvhLu4Q6mPfUFzpy"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

BlynkTimer timer;

// Relay pins
#define relay1 13
#define relay2 14
#define relay3 27
#define relay4 26
#define push1 25 // Fan toggle button
#define push2 33 // Fan speed control button
#define push3 32
#define push4 2

// Stepper motor control pins
#define A_plus 21
#define A_minus 19
#define B_plus 18
#define B_minus 5

int relay1_state = 0;
int relay2_state = 0;
int relay3_state = 0;
int relay4_state = 0;

// Fan control variables
int fan_speed = 0; // 0 = off, 1 = low, 2 = medium, 3 = high
int fan_on = 0; // 0 = off, 1 = on
unsigned long fan_step_delay = 0; // Delay between steps for the stepper motor

#define push1_vpin V1
#define push2_vpin V2
#define push3_vpin V3
#define push4_vpin V4

BLYNK_CONNECTED()
{
  Blynk.syncVirtual(push1_vpin);
  Blynk.syncVirtual(push2_vpin);
  Blynk.syncVirtual(push3_vpin);
  Blynk.syncVirtual(push4_vpin);
}

void setup()
{
  Serial.begin(115200);

  // Initialize pins
  pinMode(push1, INPUT_PULLUP);
  pinMode(push2, INPUT_PULLUP);
  pinMode(push3, INPUT_PULLUP);
  pinMode(push4, INPUT_PULLUP); 
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);

  // Initialize stepper motor pins
  pinMode(A_plus, OUTPUT);
  pinMode(A_minus, OUTPUT);
  pinMode(B_plus, OUTPUT);
  pinMode(B_minus, OUTPUT);

  // Set all relays and fan off initially
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);
  digitalWrite(A_plus, LOW);
  digitalWrite(A_minus, LOW);
  digitalWrite(B_plus, LOW);
  digitalWrite(B_minus, LOW);

  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run();
  timer.run();
  listen_push_button();

  // Control fan rotation if fan is on
  if (fan_on) {
    rotate_fan();
  }
}

void listen_push_button()
{
  if (digitalRead(push1) == LOW) // Fan toggle button
  {
    delay(200);
    fan_on = !fan_on; // Toggle fan state
    if (!fan_on) { // If fan is turned off, reset motor
      stop_fan();
    }
    Serial.print("Fan State: ");
    Serial.println(fan_on ? "On" : "Off");
  }
  else if (digitalRead(push2) == LOW) // Fan speed control button
  {
    delay(200);
    control_fan_speed(); // Change fan speed
  }
}

void control_fan_speed()
{
  fan_speed = (fan_speed + 1) % 4; // Cycle through speeds: 0 -> 1 -> 2 -> 3 -> 0
  switch (fan_speed)
  {
    case 0:
      fan_step_delay = 0; // Off
      break;
    case 1:
      fan_step_delay = 20; // Low speed
      break;
    case 2:
      fan_step_delay = 10; // Medium speed
      break;
    case 3:
      fan_step_delay = 5; // High speed
      break;
  }
  Serial.print("Fan Speed Level: ");
  Serial.println(fan_speed);
}

void rotate_fan()
{
  if (fan_step_delay > 0) {
    // Step 1
    digitalWrite(A_plus, HIGH);
    digitalWrite(A_minus, LOW);
    digitalWrite(B_plus, LOW);
    digitalWrite(B_minus, LOW);
    delay(fan_step_delay);

    // Step 2
    digitalWrite(A_plus, LOW);
    digitalWrite(A_minus, LOW);
    digitalWrite(B_plus, HIGH);
    digitalWrite(B_minus, LOW);
    delay(fan_step_delay);

    // Step 3
    digitalWrite(A_plus, LOW);
    digitalWrite(A_minus, HIGH);
    digitalWrite(B_plus, LOW);
    digitalWrite(B_minus, LOW);
    delay(fan_step_delay);

    // Step 4
    digitalWrite(A_plus, LOW);
    digitalWrite(A_minus, LOW);
    digitalWrite(B_plus, LOW);
    digitalWrite(B_minus, HIGH);
    delay(fan_step_delay);
  }
}

void stop_fan()
{
  digitalWrite(A_plus, LOW);
  digitalWrite(A_minus, LOW);
  digitalWrite(B_plus, LOW);
  digitalWrite(B_minus, LOW);
}
