#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

int16_t ax, ay, az;
int16_t gx, gy, gz;

const int NUM_SAMPLES = 10;
int array1[10], array2[10], array3[10];
int array4[10], array5[10], array6[10];

// Simple max function (unchanged)
int findMax(int arr[], int size) {
  int maxVal = arr[0];
  for (int i = 1; i < size; i++) {
    if (arr[i] > maxVal) maxVal = arr[i];
  }
  return maxVal;
}

void setup() {
  Serial.begin(115200);          // faster baud rate
  Wire.begin();
  mpu.initialize();

  if (mpu.testConnection()) {
    Serial.println("MPU6050 Connected!");
  } else {
    Serial.println("Connection Failed!");
    while (1);
  }

  // Set best resolution (optional but recommended)
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  // (DLPF not set here to keep it simple – default is fine)
}

void loop() {
  // Collect 10 readings, one per second
  for (int i = 0; i < NUM_SAMPLES; i++) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);   // read fresh data
    array1[i] = ax;
    array2[i] = ay;
    array3[i] = az;
    array4[i] = gx;
    array5[i] = gy;
    array6[i] = gz;
    delay(1000);   // 1 second between samples
  }

  // Compute maximums
  int max_ax = findMax(array1, 10);
  int max_ay = findMax(array2, 10);
  int max_az = findMax(array3, 10);
  int max_gx = findMax(array4, 10);
  int max_gy = findMax(array5, 10);
  int max_gz = findMax(array6, 10);

  // Print the latest reading and the maximums (like your original)
  Serial.print("Accel: ");
  Serial.print(ax); Serial.print("  maxX="); Serial.print(max_ax);
  Serial.print("  ");
  Serial.print(ay); Serial.print("  maxY="); Serial.print(max_ay);
  Serial.print("  ");
  Serial.print(az); Serial.print("  maxZ="); Serial.println(max_az);

  Serial.print("Gyro:  ");
  Serial.print(gx); Serial.print("  maxX="); Serial.print(max_gx);
  Serial.print("  ");
  Serial.print(gy); Serial.print("  maxY="); Serial.print(max_gy);
  Serial.print("  ");
  Serial.print(gz); Serial.print("  maxZ="); Serial.println(max_gz);

  Serial.println();
  delay(500);
}