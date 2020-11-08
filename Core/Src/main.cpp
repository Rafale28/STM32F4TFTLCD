/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "uart.h"
#include "gpio.h"
#include "fatfs.h"
#include "spi.h"
//#include "sdio.h"
#include "ILI9341.h"
#include "ILI9341_GFX.h"

//#include "snow_tiger.h"
#include <cstring>

I2S_HandleTypeDef hi2s2;
SD_HandleTypeDef hsd;

void SystemClock_Config(void);
static void MX_I2S2_Init(void);
static void MX_SDIO_SD_Init(void);


uint8_t katou[320*240*2] = {0};
int main(void)
{
  int ret =1;
  FRESULT res;                                          /* FatFs function common result code */
  //int res =0;
  uint32_t bytesread;                     /* File write/read counts */
  uint8_t rtext[100];    
  //uint8_t *picture;    
  //picture=(uint8_t *)malloc(320*240*2);    

  HAL_Init();
  SystemClock_Config();
  MX_I2S2_Init();
  MX_SDIO_SD_Init();
  MX_SPI1_Init();
  //MX_FATFS_Init();

  /* USER CODE BEGIN 2 */
  MX_GPIO_Init();
  UART_Init();
  ILI9341_Init();
  uprintf("Hello World!!\r\n");
  uprintf("ret=%d\r\n", ret);
  uprintf("fopen\r\n");

  FATFS SDFatFs;  /* File system object for SD disk logical drive */
  FIL MyFile;     /* File object */
  char SDPath[4]; /* SD disk logical drive path */
  static uint8_t buffer[_MAX_SS]; /* a work buffer for the f_mkfs() */

  /*##-1- Link the SD disk I/O driver ########################################*/
  if(FATFS_LinkDriver(&SD_Driver, SDPath) != 0)
  {
      Error_Handler();
      uprintf("[Error]:Linkdriver\r\n");
      return -1;
  }
  uprintf("Linkdriver\r\n");
  if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) != FR_OK)
  {
      Error_Handler();
      return -1;
  }
  uprintf("Mount\r\n");
  //FRESULT fr;
  // uprintf("mkfs\r\n");
  // fr = f_mkfs((TCHAR const*)SDPath, FM_ANY, 0, buffer, sizeof(buffer));
  // if (fr != FR_OK)
  // {
  //    Error_Handler();
  // }
  if(f_open(&MyFile, "hello.txt", FA_READ) != FR_OK)
  {
    Error_Handler();
    return 0;
  }

  res = f_read(&MyFile, rtext, sizeof(rtext), (UINT*)&bytesread);

  if((bytesread == 0) || (res != FR_OK)) /* EOF or Error */
  {
    Error_Handler();
    f_close(&MyFile);
    return 0;
  }

  uprintf("%s\r\n", rtext);
  f_close(&MyFile);

  //if(f_open(&MyFile, "katou.bin", FA_READ) != FR_OK)
  //{
  //  Error_Handler();
  //  return 0;
  //}
  //uprintf("open katou.bin\r\n");
  //res = f_read(&MyFile, katou, sizeof(320*240*2), (UINT*)&bytesread);
  //uprintf("fread %d, bytesread %lu\r\n", res, bytesread);
  //f_close(&MyFile);
  //while (1)
  //{
    uprintf("Pic!\r\n");
    //ILI9341_Draw_Image((const char*)snow_tiger, SCREEN_VERTICAL_1);		
    ILI9341_Draw_From_SD("katou.bin", SCREEN_HORIZONTAL_1);		
		HAL_Delay(1000);			
    Led_Toggle();
  //}
  FATFS_UnLinkDriver(SDPath);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2S2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S2_Init(void)
{

  /* USER CODE BEGIN I2S2_Init 0 */

  /* USER CODE END I2S2_Init 0 */

  /* USER CODE BEGIN I2S2_Init 1 */

  /* USER CODE END I2S2_Init 1 */
  hi2s2.Instance = SPI2;
  hi2s2.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_8K;
  hi2s2.Init.CPOL = I2S_CPOL_LOW;
  hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_ENABLE;
  if (HAL_I2S_Init(&hi2s2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S2_Init 2 */

  /* USER CODE END I2S2_Init 2 */

}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 10;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  uprintf("Error Handler\r\n");

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
