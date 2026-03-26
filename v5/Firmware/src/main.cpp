#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
  delay(250);

  Serial.println();
  Serial.println("Project Octopus v5");
  Serial.println("Wired camera scaffold booted.");

  // v5 starts from a clean baseline so we can add networking, camera,
  // and enclosure-specific behavior incrementally.
}

void loop()
{
  delay(1000);
}
