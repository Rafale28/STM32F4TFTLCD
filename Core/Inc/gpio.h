#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define LD2_Pin GPIO_PIN_0
#define LD2_GPIO_Port GPIOC
#define CS_Pin GPIO_PIN_6
#define CS_GPIO_Port GPIOB
#define RST_Pin GPIO_PIN_7
#define RST_GPIO_Port GPIOC
#define DC_Pin GPIO_PIN_9
#define DC_GPIO_Port GPIOA
/* USER CODE END Private defines */

extern void Error_Handler(void);

void MX_GPIO_Init(void);
void Led_Toggle();
void CS_High();
void CS_Low();
void DC_High();
void DC_Low();
void RST_High();
void RST_Low();

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ spi_H */