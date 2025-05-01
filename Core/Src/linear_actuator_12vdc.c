/*
 * 12vdc_linear_actuator.c
 *
 *  Created on: Apr 28, 2025
 *      Author: frank
 */

#include <linear_actuator_12vdc.h>

void Move_Forward()
{

    /* Set direction - enable right side, disable left side */
	HAL_GPIO_WritePin(R_EN_GPIO_Port, R_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(L_EN_GPIO_Port, L_EN_Pin, GPIO_PIN_SET);

	/* Set PWM duty cycle for speed control */
	//__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
	HAL_GPIO_WritePin(R_PWM_GPIO_Port, R_PWM_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(L_PWM_GPIO_Port, L_PWM_Pin, GPIO_PIN_SET);
}

/**
 * @brief Move actuator backward at specified speed
 * @param speed - PWM duty cycle (0-1000 representing 0-100%)
 */
void Move_Backward()
{

    /* Set direction - enable left side, disable right side */
	HAL_GPIO_WritePin(R_EN_GPIO_Port, R_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(L_EN_GPIO_Port, L_EN_Pin, GPIO_PIN_SET);

	/* Set PWM duty cycle for speed control */
	//__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
	HAL_GPIO_WritePin(L_PWM_GPIO_Port, L_PWM_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(R_PWM_GPIO_Port, R_PWM_Pin, GPIO_PIN_SET);
}

/**
 * @brief Stop the actuator motor
 */
void Stop_Motor(void)
{
    /* Disable both sides */
    HAL_GPIO_WritePin(R_EN_GPIO_Port, R_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(L_EN_GPIO_Port, L_EN_Pin, GPIO_PIN_RESET);

    /* Set PWM to zero (optional, already stopped by disable) */
    //__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
    HAL_GPIO_WritePin(R_PWM_GPIO_Port, R_PWM_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(L_PWM_GPIO_Port, L_PWM_Pin, GPIO_PIN_RESET);
}
