/*
 * ESP32 + MPU6050 + Nokia 5110 sous Zephyr
 * Affiche la température et l'accélération (en g) toutes les 500 ms.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include "pcd8544.h"

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

#define STANDARD_GRAVITY 9.80665
#define REFRESH_MS       500
#define LCD_CONTRAST     0x3F   /* à ajuster entre 0x30 et 0x50 */

static const struct device *const mpu = DEVICE_DT_GET_ONE(invensense_mpu6050);

int main(void)
{
	char buf[PCD8544_COLS + 1];

	int err = pcd8544_init(LCD_CONTRAST);
	if (err) {
		LOG_ERR("Nokia 5110 non prêt (%d)", err);
		return 0;
	}

	pcd8544_print(0, 0, "Demarrage...");
	pcd8544_update();

	if (!device_is_ready(mpu)) {
		LOG_ERR("MPU6050 non prêt (vérifie le câblage / adresse I2C)");
		pcd8544_clear();
		pcd8544_print(0, 0, "Erreur MPU");
		pcd8544_update();
		return 0;
	}

	LOG_INF("Démarrage OK");

	while (1) {
		struct sensor_value accel[3], temp;

		int rc = sensor_sample_fetch(mpu);
		if (rc == 0) {
			rc = sensor_channel_get(mpu, SENSOR_CHAN_ACCEL_XYZ, accel);
		}
		if (rc == 0) {
			rc = sensor_channel_get(mpu, SENSOR_CHAN_DIE_TEMP, &temp);
		}

		pcd8544_clear();

		if (rc != 0) {
			LOG_WRN("Lecture capteur échouée (%d)", rc);
			pcd8544_print(0, 0, "Erreur lecture");
		} else {
			double t  = sensor_value_to_double(&temp);
			double ax = sensor_value_to_double(&accel[0]) / STANDARD_GRAVITY;
			double ay = sensor_value_to_double(&accel[1]) / STANDARD_GRAVITY;
			double az = sensor_value_to_double(&accel[2]) / STANDARD_GRAVITY;

			pcd8544_print(0, 0, "ESP32 MPU6050");

			snprintf(buf, sizeof(buf), "T: %.1f C", t);
			pcd8544_print(0, 2, buf);
			snprintf(buf, sizeof(buf), "X: %+.2f g", ax);
			pcd8544_print(0, 3, buf);
			snprintf(buf, sizeof(buf), "Y: %+.2f g", ay);
			pcd8544_print(0, 4, buf);
			snprintf(buf, sizeof(buf), "Z: %+.2f g", az);
			pcd8544_print(0, 5, buf);

			LOG_INF("T=%.2f C  a=(%.2f, %.2f, %.2f) g", t, ax, ay, az);
		}

		pcd8544_update();
		k_msleep(REFRESH_MS);
	}

	return 0;
}