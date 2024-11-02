#include "esphome.h"

namespace esphome {
namespace wf183de {

class WF183DESensor : public PollingComponent {
 public:
  Sensor *temperature_sensor = new Sensor();
  Sensor *pressure_sensor = new Sensor();

  WF183DESensor(uint32_t update_interval) : PollingComponent(update_interval) {}

  Sensor *get_temperature_sensor() { return temperature_sensor; }
  Sensor *get_pressure_sensor() { return pressure_sensor; }

  void setup() override {
    ESP_LOGD("WF183DE", "Sensor setup completed");
  }

  void update() override {
    // Trigger temperature reading
    Wire.beginTransmission(0x6D);  // I2C 地址
    Wire.write(0x0A);  // 温度命令寄存器
    Wire.write(0x04);  // 触发温度转换命令
    if (Wire.endTransmission() != 0) {
      ESP_LOGE("WF183DE", "Failed to start temperature conversion");
      return;
    }
    delay(40);//10

    // Read temperature
    Wire.beginTransmission(0x6D);
    Wire.write(0x0F);  // 温度高字节
    Wire.endTransmission();
    //delay(10);  // 再次延迟以确保数据可用
    Wire.requestFrom(0x6D, 2);
    if (Wire.available() == 2) {
      int16_t temp_raw = (Wire.read() << 8) | Wire.read();
      float temperature = temp_raw / 10.0;
      ESP_LOGD("WF183DE", "Temperature raw: %d, converted: %.1f", temp_raw, temperature);
      temperature_sensor->publish_state(temperature);
    } else {
      ESP_LOGE("WF183DE", "Failed to read temperature data");
    }

    // Trigger pressure reading
    Wire.beginTransmission(0x6D);
    Wire.write(0x0A);
    Wire.write(0x06);  // 压力命令
    if (Wire.endTransmission() != 0) {
      ESP_LOGE("WF183DE", "Failed to start pressure conversion");
      return;
    }
    delay(40);//10

    // Read pressure
    Wire.beginTransmission(0x6D);
    Wire.write(0x0B);
    Wire.endTransmission();
    //delay(10);  // 再次延迟以确保数据可用
    Wire.requestFrom(0x6D, 4);
    if (Wire.available() == 4) {
      uint32_t pressure_raw = (Wire.read() << 24) | (Wire.read() << 16) | (Wire.read() << 8) | Wire.read();
      float pressure = pressure_raw / 1000.0;
      ESP_LOGD("WF183DE", "Pressure raw: %u, converted: %.1f", pressure_raw, pressure);
      pressure_sensor->publish_state(pressure);
    } else {
      ESP_LOGE("WF183DE", "Failed to read pressure data");
    }
  }
};

}  // namespace wf183de
}  // namespace esphome
