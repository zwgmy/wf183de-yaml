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
    // 初始化传感器，如果需要
  }

  void update() override {
    // 读取温度和压力
    Wire.beginTransmission(0x6D);
    Wire.write(0x0A);
    Wire.write(0x04);
    Wire.endTransmission();
    delay(10);

    Wire.beginTransmission(0x6D);
    Wire.write(0x0F);
    Wire.endTransmission();
    Wire.requestFrom(0x6D, 2);
    if (Wire.available() == 2) {
        int16_t temp_raw = (Wire.read() << 8) | Wire.read();
        float temperature = temp_raw / 10.0;
        temperature_sensor->publish_state(temperature);
    }

    Wire.beginTransmission(0x6D);
    Wire.write(0x0A);
    Wire.write(0x06);
    Wire.endTransmission();
    delay(10);

    Wire.beginTransmission(0x6D);
    Wire.write(0x0B);
    Wire.endTransmission();
    Wire.requestFrom(0x6D, 4);
    if (Wire.available() == 4) {
        uint32_t pressure_raw = (Wire.read() << 24) | (Wire.read() << 16) | (Wire.read() << 8) | Wire.read();
        float pressure = pressure_raw / 1000.0;
        pressure_sensor->publish_state(pressure);
    }
  }
};

}  // namespace wf183de
}  // namespace esphome
