/**
 * @file Algorithm.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Algorithm.h"

void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, float setpoint) 
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->setpoint = setpoint;
    pid->prev_error = 0.0f;
    pid->integral = 0.0f;
    pid->output = 0.0f;
}

float PID_Compute(PID_Controller *pid, float current_yaw, float dt) 
{
    float error = pid->setpoint - current_yaw;
    pid->integral += error * dt;
    float derivative = (error - pid->prev_error) / dt;
    pid->output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    pid->prev_error = error;
    return pid->output;
}

void Kalman_Init(KalmanFilter *kf, float Q_angle, float Q_bias, float R_measure)
{
    kf->Q_angle = Q_angle;
    kf->Q_bias = Q_bias;
    kf->R_measure = R_measure;

    kf->angle = 0.0f;
    kf->bias = 0.0f;
    kf->rate = 0.0f;

    kf->P[0][0] = 0.0f;
    kf->P[0][1] = 0.0f;
    kf->P[1][0] = 0.0f;
    kf->P[1][1] = 0.0f;
}

float Kalman_Update(KalmanFilter *kf, float new_angle, float new_rate, float dt) 
{
    // 预测部分
    kf->rate = new_rate - kf->bias;
    kf->angle += dt * kf->rate;

    // 更新误差协方差矩阵
    kf->P[0][0] += dt * (dt*kf->P[1][1] - kf->P[0][1] - kf->P[1][0] + kf->Q_angle);
    kf->P[0][1] -= dt * kf->P[1][1];
    kf->P[1][0] -= dt * kf->P[1][1];
    kf->P[1][1] += kf->Q_bias * dt;

    // 计算卡尔曼增益
    float S = kf->P[0][0] + kf->R_measure;
    float K[2];
    K[0] = kf->P[0][0] / S;
    K[1] = kf->P[1][0] / S;

    // 更新估计值
    float y = new_angle - kf->angle;
    kf->angle += K[0] * y;
    kf->bias += K[1] * y;

    // 更新误差协方差矩阵
    float P00_temp = kf->P[0][0];
    float P01_temp = kf->P[0][1];

    kf->P[0][0] -= K[0] * P00_temp;
    kf->P[0][1] -= K[0] * P01_temp;
    kf->P[1][0] -= K[1] * P00_temp;
    kf->P[1][1] -= K[1] * P01_temp;

    return kf->angle;
}
