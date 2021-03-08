/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
#define RC522_CS_Pin GPIO_PIN_4
#define RC522_CS_GPIO_Port GPIOA
#define RC522_RST_Pin GPIO_PIN_4
#define RC522_RST_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_5
#define LCD_RST_GPIO_Port GPIOC
#define LCD_CE_Pin GPIO_PIN_0
#define LCD_CE_GPIO_Port GPIOB
#define LCD_DC_Pin GPIO_PIN_1
#define LCD_DC_GPIO_Port GPIOB
#define LCD_DIN_Pin GPIO_PIN_10
#define LCD_DIN_GPIO_Port GPIOB
#define LCD_CLK_Pin GPIO_PIN_11
#define LCD_CLK_GPIO_Port GPIOB
#define KEY_Pin GPIO_PIN_9
#define KEY_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_8
#define LED2_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_9
#define LED1_GPIO_Port GPIOA
#define FLASH_CS_Pin GPIO_PIN_2
#define FLASH_CS_GPIO_Port GPIOD
#define CONTROL_Pin GPIO_PIN_8
#define CONTROL_GPIO_Port GPIOB


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
