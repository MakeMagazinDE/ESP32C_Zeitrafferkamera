#include "esp_camera.h"
#include "SD_MMC.h"

void setup() {
  Serial.begin(115200);

  Serial.print("Initialisiere die Kamera...");
  initCamera();
  Serial.println("Ok!");
 
  Serial.print("Initialisiere das SD-Karten-Modul...");
  initSDCard();
  Serial.println("Ok!");
}

void loop() {
  static uint16_t photoNumber = 0;
  char tmp[6];

  sprintf(tmp, "%05d", photoNumber);
  String path = "/foto" + String(tmp) + ".jpg";
  Serial.printf("Dateiname: %s\n", path.c_str());

  camera_fb_t* frameBuffer = esp_camera_fb_get();
  if (!frameBuffer) {
    Serial.println("Foto wurde nicht aufgenommen.");
  } else {
    storePhoto(path, frameBuffer);
    esp_camera_fb_return(frameBuffer);
  }
  photoNumber++;
  delay(5000);
}

void initCamera() {
  static const int8_t PWDN_GPIO_NUM = 32;
  static const int8_t RESET_GPIO_NUM = -1;
  static const int8_t XCLK_GPIO_NUM = 0;
  static const int8_t SIOD_GPIO_NUM = 26;
  static const int8_t SIOC_GPIO_NUM = 27;
  static const int8_t Y9_GPIO_NUM = 35;
  static const int8_t Y8_GPIO_NUM = 34;
  static const int8_t Y7_GPIO_NUM = 39;
  static const int8_t Y6_GPIO_NUM = 36;
  static const int8_t Y5_GPIO_NUM = 21;
  static const int8_t Y4_GPIO_NUM = 19;
  static const int8_t Y3_GPIO_NUM = 18;
  static const int8_t Y2_GPIO_NUM = 5;
  static const int8_t VSYNC_GPIO_NUM = 25;
  static const int8_t HREF_GPIO_NUM = 23;
  static const int8_t PCLK_GPIO_NUM = 22;

  camera_config_t config = {
    .pin_pwdn = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .pin_xclk = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,
    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href = HREF_GPIO_NUM,
    .pin_pclk = PCLK_GPIO_NUM,

    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_UXGA,
    .jpeg_quality = 10,
    .fb_count = 2,
    .grab_mode = CAMERA_GRAB_LATEST
  };
  
  const esp_err_t status = esp_camera_init(&config);
  if (status != ESP_OK) {
    Serial.printf("Kamera nicht initialisiert: 0x%x", status);
    return;
  }

  sensor_t* sensor = esp_camera_sensor_get();
  sensor->set_brightness(sensor, 0);
  sensor->set_contrast(sensor, 0);
  sensor->set_saturation(sensor, 0);
  sensor->set_special_effect(sensor, 0);
  sensor->set_whitebal(sensor, 1);
  sensor->set_awb_gain(sensor, 1);
  sensor->set_wb_mode(sensor, 0);
  sensor->set_exposure_ctrl(sensor, 1);
  sensor->set_aec2(sensor, 0);
  sensor->set_ae_level(sensor, 0);
  sensor->set_aec_value(sensor, 300);
  sensor->set_gain_ctrl(sensor, 1);
  sensor->set_agc_gain(sensor, 0);
  sensor->set_gainceiling(sensor, (gainceiling_t)0);
  sensor->set_bpc(sensor, 0);
  sensor->set_wpc(sensor, 1);
  sensor->set_raw_gma(sensor, 1);
  sensor->set_lenc(sensor, 1);
  sensor->set_hmirror(sensor, 0);
  sensor->set_vflip(sensor, 0);
  sensor->set_dcw(sensor, 1);
  sensor->set_colorbar(sensor, 0);
}

void initSDCard() {
  if (!SD_MMC.begin()) {
    Serial.println("SD-Karten-Modul konnte nicht initialisiert werden.");
    return;
  }
  const uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("Keine SD-Karte eingelegt.");
    return;
  }
}

void storePhoto(const String& path, const camera_fb_t* frameBuffer) {
  File file = SD_MMC.open(path.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Konnte Datei nicht erzeugen.");
  } else {
    file.write(frameBuffer->buf, frameBuffer->len);
    Serial.printf("Datei %s wurde gespeichert.\n", path.c_str());
  }
  file.close();  
}