#include <TensorFlowLite_ESP32.h>
#include <Wire.h>
#include <MPU6050.h>

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

// Include the model data
#include "har_model.h"

MPU6050 mpu;
float input_buffer[100][6];
int sample_index = 0;

const tflite::Model* model;
tflite::MicroInterpreter* interpreter;
TfLiteTensor* input;
TfLiteTensor* output;

constexpr int kTensorArenaSize = 150 * 1024;
uint8_t* tensor_arena;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }
  
  tensor_arena = (uint8_t*)malloc(kTensorArenaSize);
  
  model = tflite::GetModel(har_model_int8_tflite);
  
  static tflite::AllOpsResolver resolver;
  static tflite::MicroInterpreter static_interpreter(
    model, resolver, tensor_arena, kTensorArenaSize, nullptr
  );
  interpreter = &static_interpreter;
  
  if (interpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("Tensor allocation failed!");
    while (1);
  }
  
  input = interpreter->input(0);
  output = interpreter->output(0);
  
  Serial.println("Setup complete!");
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  
  input_buffer[sample_index][0] = (float)ax;
  input_buffer[sample_index][1] = (float)ay;
  input_buffer[sample_index][2] = (float)az;
  input_buffer[sample_index][3] = (float)gx;
  input_buffer[sample_index][4] = (float)gy;
  input_buffer[sample_index][5] = (float)gz;
  
  sample_index++;
  
  if (sample_index >= 100) {
    for (int i = 0; i < 100; i++) {
      for (int j = 0; j < 6; j++) {
        input->data.f[i * 6 + j] = input_buffer[i][j];
      }
    }
    
    if (interpreter->Invoke() != kTfLiteOk) {
      Serial.println("Inference failed!");
      sample_index = 0;
      return;
    }
    
    int predicted = 0;
    float max_val = output->data.f[0];
    for (int i = 1; i < 24; i++) {
      if (output->data.f[i] > max_val) {
        max_val = output->data.f[i];
        predicted = i;
      }
    }
    
    Serial.print("Class: ");
    Serial.println(predicted);
    sample_index = 0;
  }
  
  delay(10);
}