#include "esphome.h"
using namespace esphome;
#define WF183DE_I2C_ADDRESS 0x6D  // 使用实际的I2C地址

class WF183DESensor : public PollingComponent {
 public:
  Sensor *temperature_sensor = new Sensor();
  Sensor *pressure_sensor = new Sensor();

  WF183DESensor() : PollingComponent(30000) {}  //表示每30秒更新一次

  Sensor *get_temperature_sensor() { return temperature_sensor; }
  Sensor *get_pressure_sensor() { return pressure_sensor; }

  void setup() override {
    // Setup code if needed
  }

void update() override {
    // Trigger temperature reading
    Wire.beginTransmission(WF183DE_I2C_ADDRESS); // I2C write address
    Wire.write(0x0A);  // Command register
    Wire.write(0x04);  // Temperature command
    Wire.endTransmission();
    delay(50);  // 增加延迟以确保传感器处理命令

    // Read temperature
    Wire.beginTransmission(WF183DE_I2C_ADDRESS);
    Wire.write(0x0F);  // Temperature high byte register
    Wire.endTransmission();
    delay(10);  // 再次延迟以确保数据可用
    Wire.requestFrom(WF183DE_I2C_ADDRESS, 2); // Request 2 bytes
    if (Wire.available() == 2) {
        int16_t temp_raw = (Wire.read() << 8) | Wire.read();
        float temperature = temp_raw / 10.0;
        ESP_LOGD("WF183DE", "Temperature raw: %d, converted: %.1f", temp_raw, temperature);
        temperature_sensor->publish_state(temperature);
    } else {
        ESP_LOGE("WF183DE", "Failed to read temperature: %d bytes available", Wire.available());
    }

    // Trigger pressure reading
    Wire.beginTransmission(WF183DE_I2C_ADDRESS);
    Wire.write(0x0A);
    Wire.write(0x06);  // Pressure command
    Wire.endTransmission();
    delay(50);  // 增加延迟以确保传感器处理命令

    // Read pressure
    Wire.beginTransmission(WF183DE_I2C_ADDRESS);
    Wire.write(0x0B);  // Pressure high byte register
    Wire.endTransmission();
    delay(10);  // 再次延迟以确保数据可用
    Wire.requestFrom(WF183DE_I2C_ADDRESS, 4); // Request 4 bytes
    if (Wire.available() == 4) {
        uint32_t pressure_raw = (Wire.read() << 24) | (Wire.read() << 16) | (Wire.read() << 8) | Wire.read();
        float pressure = pressure_raw / 1000.0; // Convert to kPa
        ESP_LOGD("WF183DE", "Pressure raw: %u, converted: %.1f", pressure_raw, pressure);
        pressure_sensor->publish_state(pressure);
    } else {
        ESP_LOGE("WF183DE", "Failed to read pressure: %d bytes available", Wire.available());
    }
}

};
