/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
#include "delay.h"
#include "retarget.h"
#include "rc522.h"
#include "nokia_5110.h"
#include "remote.h"
#include "userinfo.h"
#include "mode.h"
#include "w25qxx.h"
#include "display.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

int main(void) {

    u8 mode=2;                          //初始化模式为read_mode
    u8 key_val;                         //保存红外遥控键值
    u8 option;                          //选中的菜单选项

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_RTC_Init();
    MX_SPI2_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();

    delay_init(72);
    RC522_Init();
    Remote_Init();
    RetargetInit(&huart1);                  //串口打印重定向
    W25QXX_Init();
    LCD_init();
    LCD_clear();
    LCD_write_english_string(0,0,"Loading ");
    information_init(1);

    while (1)
    {
        switch(mode)
        {
            case 1:add_mode();break;
            case 2:read_mode();break;
            case 3:del_mode();mode=2;break;
            case 4:root_mode();break;
            case SystemInfo:infomation_mode(0);mode=2;break;
            case MakerInfo:infomation_mode(1);mode=2;break;
            default:read_mode();
        }
        key_val=Remote_Scan();          //无按键按下时返回0
        if(key_val==2)  //    >>|进入菜单页
        {
            meanu_mode(&option);
        }
        execute(option,&mode);
        option=0;
    }
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line NUM_USER_USERber
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source NUM_USER_USERber
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line NUM_USER_USERber,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
