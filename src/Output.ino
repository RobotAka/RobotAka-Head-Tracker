/* This file is part of the Razor AHRS Firmware */

// Output angles: yaw, pitch, roll
void output_angles() {
	float ypr[3];
	ypr[0] = TO_DEG(yaw);
	ypr[1] = TO_DEG(pitch);
	ypr[2] = TO_DEG(roll);

	if (output_format == OUTPUT__FORMAT_BINARY) {
		Serial.write((byte *)ypr, 12); // No new-line
	}
	else if (output_format == OUTPUT__FORMAT_TEXT) {
		Serial.print("#YPR=");
		Serial.print(TO_DEG(yaw));
		Serial.print(",");
		Serial.print(TO_DEG(pitch));
		Serial.print(",");
		Serial.print(TO_DEG(roll));
		Serial.println();
	}
	else if (output_format == OUTPUT__FORMAT_HAT) { // HAT Modification by FuraX49
		if (hat_center_ask == true) {
			if (hat_center_cpt < hat_center_Nb) {
				eprom_save.rot_offset[0] += ypr[0];
				eprom_save.rot_offset[1] += ypr[1];
				eprom_save.rot_offset[2] += ypr[2];
				hat_center_cpt++;
				hat.rot[0] = 0.0;
				hat.rot[1] = 0.0;
				hat.rot[2] = 0.0;
			}
			else {
				eprom_save.rot_offset[0] = eprom_save.rot_offset[0] / hat_center_Nb;
				eprom_save.rot_offset[1] = eprom_save.rot_offset[1] / hat_center_Nb;
				eprom_save.rot_offset[2] = eprom_save.rot_offset[2] / hat_center_Nb;
				SaveHatOffsets();
				hat_center_ask = false;
				hat_center_cpt = 9999;
			}
		}
		else {
			// Convet angles Euler en +-180 Degrees  with zero center offset
			for (int i = 0; i <= 2; i++) {
				hat.rot[i] = ypr[i] - eprom_save.rot_offset[i];

				if (hat.rot[i] > 180) {
					hat.rot[i] = hat.rot[i] - 360;
				}
				if (hat.rot[i] < -180) {
					hat.rot[i] = hat.rot[i] + 360;
				}
			}
		}

		// Send Trame to HATIRE PC
		Serial.write((byte *)&hat, 30);
		hat.Cpt++;

		if (hat.Cpt > 999) {
			hat.Cpt = 0;
		}
	}
}

void output_calibration(int calibration_sensor) {
	if (calibration_sensor == 0) { // Accelerometer
		// Output MIN/MAX values
		Serial.print("accel x,y,z (min/max) = ");

		for (int i = 0; i < 3; i++) {
			if (accel[i] < accel_min[i]) {
				accel_min[i] = accel[i];
			}

			if (accel[i] > accel_max[i]) {
				accel_max[i] = accel[i];
			}

			Serial.print(accel_min[i]);
			Serial.print("/");
			Serial.print(accel_max[i]);

			if (i < 2) {
				Serial.print("  ");
			}
			else {
				Serial.println();
			}
		}
	}
	else if (calibration_sensor == 1) { // Magnetometer
		// Output MIN/MAX values
		Serial.print("magn x,y,z (min/max) = ");
		
		for (int i = 0; i < 3; i++) {
			if (magnetom[i] < magnetom_min[i]) {
				magnetom_min[i] = magnetom[i];
			}

			if (magnetom[i] > magnetom_max[i]) {
				magnetom_max[i] = magnetom[i];
			}

			Serial.print(magnetom_min[i]);
			Serial.print("/");
			Serial.print(magnetom_max[i]);
			
			if (i < 2) {
				Serial.print("  ");
			}
			else {
				Serial.println();
			}
		}
	}
	else if (calibration_sensor == 2) { // Gyroscope
		// Average gyro values
		for (int i = 0; i < 3; i++) {
			gyro_average[i] += gyro[i];
		}

		gyro_num_samples++;

		// Output current and averaged gyroscope values
		Serial.print("gyro x,y,z (current/average) = ");
		
		for (int i = 0; i < 3; i++) {
			Serial.print(gyro[i]);
			Serial.print("/");
			Serial.print(gyro_average[i] / (float)gyro_num_samples);
			
			if (i < 2) {
				Serial.print("  ");
			}
			else {
				Serial.println();
			}
		}
	}
}

void output_sensors_text(char raw_or_calibrated) {
	Serial.print("#A-");
	Serial.print(raw_or_calibrated);
	Serial.print('=');
	Serial.print(accel[0]);
	Serial.print(",");
	Serial.print(accel[1]);
	Serial.print(",");
	Serial.print(accel[2]);
	Serial.println();

	Serial.print("#M-");
	Serial.print(raw_or_calibrated);
	Serial.print('=');
	Serial.print(magnetom[0]);
	Serial.print(",");
	Serial.print(magnetom[1]);
	Serial.print(",");
	Serial.print(magnetom[2]);
	Serial.println();

	Serial.print("#G-");
	Serial.print(raw_or_calibrated);
	Serial.print('=');
	Serial.print(gyro[0]);
	Serial.print(",");
	Serial.print(gyro[1]);
	Serial.print(",");
	Serial.print(gyro[2]);
	Serial.println();
}

void output_sensors_binary() {
	Serial.write((byte *)accel, 12);
	Serial.write((byte *)magnetom, 12);
	Serial.write((byte *)gyro, 12);
}

void output_sensors() {
	if (output_mode == OUTPUT__MODE_SENSORS_RAW) {
		if (output_format == OUTPUT__FORMAT_BINARY) {
			output_sensors_binary();
		}
		else if (output_format == OUTPUT__FORMAT_TEXT) {
			output_sensors_text('R');
		}
	}
	else if (output_mode == OUTPUT__MODE_SENSORS_CALIB) {
		// Apply sensor calibration
		compensate_sensor_errors();

		if (output_format == OUTPUT__FORMAT_BINARY) {
			output_sensors_binary();
		}
		else if (output_format == OUTPUT__FORMAT_TEXT) {
			output_sensors_text('C');
		}
	}
	else if (output_mode == OUTPUT__MODE_SENSORS_BOTH) {
		if (output_format == OUTPUT__FORMAT_BINARY) {
			output_sensors_binary();
			compensate_sensor_errors();
			output_sensors_binary();
		}
		else if (output_format == OUTPUT__FORMAT_TEXT) {
			output_sensors_text('R');
			compensate_sensor_errors();
			output_sensors_text('C');
		}
	}
}
