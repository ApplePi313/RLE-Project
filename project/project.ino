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

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

  //Hello World

}
