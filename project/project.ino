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

struct Vector3f{
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

  void rightSwipeCheck(Vector3f accel) {
    if (accel.j > 10) {
      if (rightSwipeHandler != NULL)
        rightSwipeHandler();
    }
  }

  public:
    void addRightSwipeHandler(void (function())) {
      rightSwipeHandler = function;
    }

    void update(Vector3f accel) {
      rightSwipeCheck(accel);
    }
};

Vector3f accel;
Gestures gestures;

void setup() {
  // put your setup code here, to run once:
  CircuitPlayground.begin();
  Serial.begin(9600);
  Mouse.begin();

  gestures.addRightSwipeHandler(&workspaceRight);
}

void loop() {
  accel = Vector3f(CircuitPlayground.motionX(), CircuitPlayground.motionY(), CircuitPlayground.motionZ());

  accel.print();

  gestures.update(accel);

  // mouseLoop();
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
