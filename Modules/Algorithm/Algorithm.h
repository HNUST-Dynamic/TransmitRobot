/**
 * @brief 
 * 
 */
#include "main.h"

//PID控制器
typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float setpoint;   // 目标Yaw角度
    float prev_error; // 上一次的误差
    float integral;   // 误差的积分
    float output;     // PID控制输出
} PID_Controller;

//卡尔曼滤波器
typedef struct {
    float Q_angle; // 角度过程噪声协方差
    float Q_bias;  // 偏差过程噪声协方差
    float R_measure; // 测量噪声协方差

    float angle;  // 过滤后的角度
    float bias;   // 角速度偏差
    float rate;   // 角速度

    float P[2][2]; // 误差协方差矩阵
} KalmanFilter;


/**
 * @brief PID控制器初始化
 * 
 * @param pid PID控制器实例
 * @param Kp 比例系数
 * @param Ki 积分系数
 * @param Kd 微分系数
 * @param setpoint 目标值
 */
void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, float setpoint);

/**
 * @brief PID计算函数
 * 
 * @param pid PID控制器实例
 * @param current_yaw 当前的观测值
 * @param dt 时间步长
 * @return float 
 */
float PID_Compute(PID_Controller *pid, float current_yaw, float dt);

/**
 * @brief 卡尔曼滤波器初始化
 * 
 * @param kf 卡尔曼滤波器实例 
 * @param Q_angle 角度过程噪声协方差
 * @param Q_bias 偏差过程噪声协方差
 * @param R_measure 测量噪声协方差
 */
void Kalman_Init(KalmanFilter *kf, float Q_angle, float Q_bias, float R_measure);

/**
 * @brief 卡尔曼滤波器更新
 * 
 * @param kf 卡尔曼滤波器实例
 * @param new_angle 未经滤波的原始yaw值
 * @param new_rate 测量角速度
 * @param dt 步长
 * @return float 
 */
float Kalman_Update(KalmanFilter *kf, float new_angle, float new_rate, float dt);


