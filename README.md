# Inertial-Sensor-Signal-Processing:

## Objective : 
To implement MMA8451Q , on-board inertial sensor over I2C for orientation and motion detection(acceleration) on KL25Z.

While attempting to implement the above objective, the idea of skid detection also emerged, the below project description explains the idea behind skid detection. 

## Project Description : 

![Roll and Pitch](Roll_and_pitch.png)

The above picture shows the Roll and Pitch calculation around the axis. 

The formulas to convert the acceleration values to pitch and roll are as follows.
Roll = atan2(aY, aZ) * 180/PI;
Pitch = atan2(aX, sqrt(aY*aY + aZ*aZ)) * 180/PI;


![Project Idea](ProjectIdea.png)

As per the above picture, KL25Z will be receiving wheel speed values continuosly from the wheel speed sensor and It will be comparing the same with its own accelerometer (Ax) readings. As soon as wheel speed sensor reading becomes "0" but still on board accelerometer if showing some value, Then it is decided that the vehicle is skidding. This is the principle behind skid detection.


