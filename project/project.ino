#include <Keyboard.h>
#include <KeyboardLayout.h>
#include <Keyboard_da_DK.h>
#include <Keyboard_de_DE.h>
#include <Keyboard_es_ES.h>
#include <Keyboard_fr_FR.h>
#include <Keyboard_hu_HU.h>
#include <Keyboard_it_IT.h>
#include <Keyboard_pt_PT.h>
#include <Keyboard_sv_SE.h>

#include <Mouse.h>

#include <Adafruit_CircuitPlayground.h>
#include <Adafruit_Circuit_Playground.h>

// constants
#define THRESHOLD           1.5   // mouse tilt led threshold
#define LOOK_SENS           10  // mouse sensitivity multiplier
#define X_CURVE             1.75 // mouse sesitivity exponent
#define Y_CURVE             1.75 // mouse sesitivity exponent

// globals
bool lastClickState = false;
bool toggler = false;

struct Vector3f {
  float i,j,k;

  Vector3f() {
    i = 0;
    j = 0;
    k = 0;
  }

  Vector3f(float iVal, float jVal, float kVal) {
    i = iVal;
    j = jVal;
    k = kVal;
  }

  Vector3f operator+(Vector3f otherVec) {
    return Vector3f(i + otherVec.i, j + otherVec.j, k + otherVec.k);
  }
  Vector3f operator-(Vector3f otherVec) {
    return Vector3f(i - otherVec.i, j - otherVec.j, k - otherVec.k);
  }

  Vector3f operator*(float c) {
    return Vector3f(i * c, j * c, k * c);
  }
  Vector3f operator/(float c) {
    return Vector3f(i / c, j / c, k / c);
  }

  void operator+=(Vector3f otherVec) {
    i += otherVec.i; 
    j += otherVec.j; 
    k += otherVec.k;
  }
  void operator-=(Vector3f otherVec) {
    i -= otherVec.i; 
    j -= otherVec.j; 
    k -= otherVec.k;
  }

  float dot(Vector3f otherVec) {
    return i * otherVec.i + j * otherVec.j + k * otherVec.k;
  }
  Vector3f cross(Vector3f otherVec) {
    return Vector3f(j*otherVec.k - otherVec.j*k, 
                    k*otherVec.i - otherVec.k*i, 
                    i*otherVec.j - otherVec.i*j);
  }

  void print() {
    Serial.println();
    Serial.println("I: " + String(i));
    Serial.println("J: " + String(j));
    Serial.println("K: " + String(k));
    Serial.println();
  }
};

class Gestures {
  void (*rightSwipeHandler)();
  void (*leftSwipeHandler)();

  void (*rightTiltHandler)();
  void (*leftTiltHandler)();

  long cooldownTime;

  public:
    int rsProg = 0;
    int lsProg = 0;

    void addRightSwipeHandler(void (function())) {
      rightSwipeHandler = function;
    }
    void addLeftSwipeHandler(void (function())) {
      leftSwipeHandler = function;
    }
    
    void addRightTiltHandler(void (function())) {
      rightTiltHandler = function;
    }
    void addLeftTiltHandler(void (function())) {
      leftTiltHandler = function;
    }

    void update(Vector3f accel) {
      if (rsProg != 0) { // check right swipe status
        if (rsProg == 1) {
          if (accel.i > 4) {
            rsProg = 2;
          }
        } else if (rsProg == 2) {
          if (accel.i < 0) {
            rsProg++;
          }
        } else {
          if (abs(accel.j) < 4 && abs(accel.i) < 4) {
            rsProg = 0;
          }
        }
      } else if (lsProg != 0) { // check left swipe status
        if (lsProg == 1) {
          if (accel.i < -4) {
            lsProg++;
          }
        } else if (lsProg == 2) {
          if (accel.i > 0) {
            lsProg++;
          }
        } else {
          if (abs(accel.j) < 4 && abs(accel.i) < 4) {
            lsProg = 0;
          }
        }
      } else { // check for a new swipe/tilt
        if (accel.k < 5) {
          if (accel.i > 7) {
            if (rightTiltHandler != NULL) {
              rightTiltHandler();
              delay(40);
            }
          } else if (accel.i < -7) {
            if (leftTiltHandler != NULL) {
              leftTiltHandler();
              delay(40);
            }
          }
        } else if (//abs(accel.j) > 5 && 
            (millis() - cooldownTime) > 500) { // A swipe is occurring
          if (accel.i < -12) {
            if (rightSwipeHandler != NULL) {
              rightSwipeHandler();
              cooldownTime = millis();
            }
            rsProg = 1;
          } else if (accel.i > 12) {
            if (leftSwipeHandler != NULL) {
              leftSwipeHandler();
              cooldownTime = millis();
            }
            lsProg = 1;
          }
        }
      }

      Serial.println("rs: " + String(rsProg));
      Serial.println("ls: " + String(lsProg));

      // if (lsProg == 1) {
      //   while (true) {}
      // }
    }
};

Vector3f accel;
Gestures gestures;
bool mouseMode = false;

void setup() {
  // put your setup code here, to run once:
  CircuitPlayground.begin();
  Serial.begin(9600);
  Mouse.begin();

  gestures.addRightSwipeHandler(&workspaceRight);
  gestures.addLeftSwipeHandler(&workspaceLeft);

  gestures.addRightTiltHandler(&scrollDown);
  gestures.addLeftTiltHandler(&scrollUp);
}

void loop() {
  accel = Vector3f(CircuitPlayground.motionX(), CircuitPlayground.motionY(), CircuitPlayground.motionZ());

  accel.print();
  
  if (accel.k > 18 && gestures.rsProg == 0 && gestures.lsProg == 0
      && abs(accel.i) < 7) {
    delay(300);
    mouseMode = !mouseMode;
  }

  if (mouseMode) {
    mouseLoop();
  } else {
    gestures.update(accel);
    for (int i = 0; i < 10; i++) {
      CircuitPlayground.setPixelColor(i, 0x7700FF);
    }
  }
}

void mouseLoop() {
  // get data
  float x = CircuitPlayground.motionX();
  float y = CircuitPlayground.motionY();
  float z = CircuitPlayground.motionZ();

  // normalize data to 1 and scale by sensitivity
  float cx = (constrain(CircuitPlayground.motionX(), -9.81, 9.81) / 9.81) * LOOK_SENS;
  float cy = (constrain(CircuitPlayground.motionY(), -9.81, 9.81) / 9.81) * LOOK_SENS;

  // curve values by an exponent to result in better range of speeds
  float rx = copysign(pow(abs(cx), X_CURVE), cx);
  float ry = copysign(pow(abs(cy), Y_CURVE), cy);

  // logging
  Serial.print("x:");
  Serial.print(rx);
  Serial.print(",y:");
  Serial.println(ry);

  //move mouse according to detected values
  Mouse.move(rx, ry);

  // handle "left click" presses
  if (CircuitPlayground.rightButton()) {
    Mouse.press(MOUSE_LEFT);
  } else {
    Mouse.release(MOUSE_LEFT);
  }

  // handle toggle state of "right click" presses
  if (CircuitPlayground.leftButton()) {
    if (!lastClickState) { // this avoids constantly flip-flopping toggle states, similar to edge detection
      lastClickState = true;
      toggler = !toggler;
    }
  } else {
    lastClickState = false;
  }

  // handle "right click" presses according to the state of the toggle switch
  bool slider = CircuitPlayground.slideSwitch();
  if (slider) {
    if (toggler) {
      Mouse.press(MOUSE_RIGHT);
      CircuitPlayground.setBrightness(150);
    } else {
      Mouse.release(MOUSE_RIGHT);
      CircuitPlayground.setBrightness(30);
    }
  } else {
    CircuitPlayground.setBrightness(30);
    toggler = false;
    if (CircuitPlayground.leftButton()) {
      Mouse.press(MOUSE_RIGHT);
    } else {
      Mouse.release(MOUSE_RIGHT);
    }
  }

  // if in toggle rick click mode, the color is red, else green
  int color[3] = {0, 0, 0};
  if (slider) {
    color[0] = 100;
  } else {
    color[1] = 100;
  }

  // get general direction the x value is pointing
  int xDir = 0;
  if (x < -THRESHOLD) {
    xDir = -1;
  } else if (x > THRESHOLD) {
    xDir = 1;
  }

  // get general direction the y value is pointing
  int yDir = 0;
  if (y < -THRESHOLD) {
    yDir = -1;
  } else if (y > THRESHOLD) {
    yDir = 1;
  }

  // set color indicator on board accrding to calculated direction pointed
  CircuitPlayground.clearPixels();
  switch (xDir) {
    case -1:
      switch (yDir) {
        case -1:
          setColors(5, 6, 7, color);
          break;
        case 0:
          setColors(6, 7, 8, color);
          break;
        case 1:
          setColors(7, 8, 9, color);
          break;
      }
      break;
    case 0:
      switch (yDir) {
        case -1:
          setColors(4, 5, 10, color);
          break;
        case 0:
          setColors(10, 10, 10, color);
          break;
        case 1:
          setColors(0, 9, 10, color);
          break;
      }
      break;
    case 1:
      switch (yDir) {
        case -1:
          setColors(2, 3, 4, color);
          break;
        case 0:
          setColors(1, 2, 3, color);
          break;
        case 1:
          setColors(0, 1, 2, color);
          break;
      }
    break;
  }
}

void setColors(int p1, int p2, int p3, int color[]) {
  CircuitPlayground.setPixelColor(p1, color[0], color[1], color[2]);
  CircuitPlayground.setPixelColor(p2, color[0], color[1], color[2]);
  CircuitPlayground.setPixelColor(p3, color[0], color[1], color[2]);
}

void workspaceLeft() {
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_ARROW);

  Keyboard.release(KEY_LEFT_GUI);
  Keyboard.release(KEY_LEFT_CTRL);
  Keyboard.release(KEY_LEFT_ARROW);
}
void workspaceRight() {
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_RIGHT_ARROW);

  Keyboard.release(KEY_LEFT_GUI);
  Keyboard.release(KEY_LEFT_CTRL);
  Keyboard.release(KEY_RIGHT_ARROW);
}

void scrollDown() {
  Keyboard.press(KEY_DOWN_ARROW);
  Keyboard.release(KEY_DOWN_ARROW);
}
void scrollUp() {
  Keyboard.press(KEY_UP_ARROW);
  Keyboard.release(KEY_UP_ARROW);
}
