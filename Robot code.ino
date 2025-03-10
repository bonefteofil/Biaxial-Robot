
// This code has 2 motor objects that follow the predefined movements at the given speeds.

class Motor
{
private:
  int stepPin, dirPin;
  int steps = 0;
  int speeds[2] = {0, 1000};
  int targetSteps = 0;
  int actualSpeed = 0;
  unsigned long targetTime = 0;
  unsigned long lastTime = 0;
  unsigned long lastSpeedTime = 0;
  bool reversed = false;
  bool activated = false;
  bool bussy = false;

  void SpeedCalculator()
  {
    actualSpeed = (1000000.0 / (micros() - lastSpeedTime));
    lastSpeedTime = micros();
  }

public:
  int Steps() { return steps; }
  int Speed() { return actualSpeed; }
  bool IsBussy() { return bussy; }

  void Setup(int StepPin, int DirPin)
  {
    stepPin = StepPin;
    dirPin = DirPin;
    pinMode(dirPin, OUTPUT);
    pinMode(stepPin, OUTPUT);
    digitalWrite(dirPin, LOW);
    digitalWrite(stepPin, LOW);
  }

  void StartMovement(int speed, int TargetSteps)
  {
    if (bussy)
      return;
    lastTime = micros();
    lastSpeedTime = micros();
    if (TargetSteps > steps == reversed)
    {
      reversed = !reversed;
      digitalWrite(dirPin, reversed);
    }
    targetSteps = TargetSteps;
    speeds[0] = speed; // set the activation duration
    bussy = true;
    activated = 0;
  }

  void StopMovement()
  {
    bussy = false;
    digitalWrite(stepPin, LOW);
  }

  void Update()
  {
    if (!bussy)
      return;
    if (steps == targetSteps && !activated)
      StopMovement();
    // check if it passed the time for the activation or deactivation
    else if (micros() >= (lastTime + speeds[!activated]))
    {
      activated = !activated;
      digitalWrite(stepPin, activated);
      if (!activated)
      {
        SpeedCalculator();
        (reversed ? --steps : ++steps);
      }
      lastTime = micros();
    }
  }
} motors[2];

// predefined movements
int moves[][4] = {
    {5000, 100, 5000, 100},
    {2300, -900, 2000, 1200}, // stanga sus
    {1000, 300, 4000, 1700},
    {1000, 1400, 3400, 2200}, // stanga jos
    {1600, 2400, 2300, 1400}, // dreapta jos
    {900, 1600, 0, 0},
    {2100, 1200, 0, 1200}, // mijloc jos
    {5000, 0, 5000, 0},

    // romb
    {0, 1600, 0, 0},
    {0, 1600, 0, 1600}, // mijloc jos
    {0, 0, 0, 1600},
    {0, 0, 0, 0},

    {3000, 2400, 6000, 1400},  // dreapta jos
    {5000, 1200, 2000, -1000}, // dreapta sus
    {4000, -600, 4000, 800},   // stanga sus
    {4000, 1400, 6200, 2200},  // stanga jos
    {0, 2200, 0, 1400},
    {0, 1400, 0, 2200},
    {0, 2200, 0, 1400},
    {0, 1400, 0, 2200},
    {0, 2200, 0, 1400},
    {20000, 1800, 20000, 1800},
    {20000, 900, 20000, 900},
    {0, 100, 0, 100},

    {1650, -900, 1400, 1200}, // stanga sus
    {0, 300, 1600, 1700},
    {0, 1400, 1200, 2200},  // stanga jos
    {300, 2400, 600, 1400}, // dreapta jos
    {900, 1600, 0, 0},
    {2100, 1200, 0, 1200}, // mijloc jos
    {0, 0, 0, 0},

    {1000, -1000, 1000, 1000},
    {1000, 1000, 1000, -1000},
    {10000, 1400, 21000, -800},
    {10000, 800, 1900, 1400},
    {10000, 1400, 3000, 1400},
    {0, 0, 0, 0},
};

struct Movement
{
private:
  int step = 0;
  int n = sizeof(moves) / sizeof(moves[0]);

public:
  // start the movement to the next position
  void NextStep()
  {
    if (step == n)
      step = 0;
    delay(1000);
    motors[1].StartMovement(moves[step][0], moves[step][1]);
    motors[0].StartMovement(moves[step][2], -moves[step][3]);
    ++step;
  }

  void Pause()
  {
    Serial.println("Paused");
    while (1)
      if (Serial.available() && Serial.readString() != "")
        return;
  }

  void CheckInput()
  {
    // if user presses any key, pause the robot
    if (Serial.available() && Serial.readString() != "")
      Pause();
  }

  void Update()
  {
    if (!motors[0].IsBussy() && !motors[1].IsBussy())
      NextStep();
    motors[0].Update();
    motors[1].Update();
  }
} movement;

void setup()
{
  motors[0].Setup(3, 2);
  motors[1].Setup(5, 4);
  Serial.begin(115200);
}

void loop()
{
  movement.Update();
  movement.CheckInput();
}
