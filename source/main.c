/*
 * Copyright 2019-2020 Sergio Alejandro González
 *
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_adc.h"
#include "fsl_adc_etc.h"
#include "fsl_gpio.h"
#include "board.h"

#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Task priorities. */
#define hello_task_PRIORITY (configMAX_PRIORITIES - 1)
#define DEMO_ADC_BASE ADC1
#define DEMO_ADC_USER_CHANNEL 16U
#define DEMO_ADC_CHANNEL_GROUP 0U

#define DEMO_ADC_ETC_BASE ADC_ETC
#define DEMO_ADC_ETC_CHAIN_LENGTH 0U
#define DEMO_ADC_ETC_CHANNEL 14U
#define EXAMPLE_ADC_ETC_DONE0_Handler ADC_ETC_IRQ0_IRQHandler

#define NUM_TIMERS 5

#define LED_GPIO BOARD_USER_LED_GPIO
#define LED_GPIO_PIN BOARD_USER_LED_GPIO_PIN



/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void hello_task(void *pvParameters);
void ADC_Configuration(void);

void vTimerCallback( TimerHandle_t pxTimer );

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile bool g_AdcConversionDoneFlag;
volatile uint32_t g_AdcConversionValue;
const uint32_t g_Adc_12bitFullRange = 4096U;

// An array to hold handles to the created timers.
TimerHandle_t xTimers[ NUM_TIMERS ];

// An array to hold a count of the number of times each timer expires.
int32_t lExpireCounters[ NUM_TIMERS ] = { 0 };
// a variable to set type of control
uint8_t	controlType;
/* The PIN status */
volatile bool g_pinSet = false;

/*******************************************************************************
 * Code
 ******************************************************************************/
void EXAMPLE_ADC_ETC_DONE0_Handler(void)
{
    ADC_ETC_ClearInterruptStatusFlags(DEMO_ADC_ETC_BASE, kADC_ETC_Trg0TriggerSource, kADC_ETC_Done0StatusFlagMask);
    g_AdcConversionDoneFlag = true;
    g_AdcConversionValue = ADC_ETC_GetADCConversionValue(DEMO_ADC_ETC_BASE, 0U, 0U); /* Get trigger0 chain0 result. */
    __DSB();
}

/*!
 * @brief Configure ADC to working with ADC_ETC.
 */
void ADC_Configuration(void)
{
    adc_config_t adcConfig;
    adc_channel_config_t adcChannelConfigStruct;

    /* Initialize the ADC module. */
    ADC_GetDefaultConfig(&adcConfig);
    ADC_Init(DEMO_ADC_BASE, &adcConfig);
    ADC_EnableHardwareTrigger(DEMO_ADC_BASE, true);

    adcChannelConfigStruct.channelNumber = DEMO_ADC_USER_CHANNEL; /* External channel selection from ADC_ETC. */
    adcChannelConfigStruct.enableInterruptOnConversionCompleted = false;
    ADC_SetChannelConfig(DEMO_ADC_BASE, DEMO_ADC_CHANNEL_GROUP, &adcChannelConfigStruct);

    /* Do auto hardware calibration. */
    if (kStatus_Success == ADC_DoAutoCalibration(DEMO_ADC_BASE))
    {
        PRINTF("ADC_DoAutoCalibration() Done.\r\n");
    }
    else
    {
        PRINTF("ADC_DoAutoCalibration() Failed.\r\n");
    }
}


// Define a callback function that will be used by multiple timer instances.
// The callback function does nothing but count the number of times the
// associated timer expires, and stop the timer once the timer has expired
// 10 times.
void vTimerCallback( TimerHandle_t pxTimer )
{
	int32_t lArrayIndex;

	//int32_t currentTemperature = 0;

	const int32_t xMaxExpiryCountBeforeStopping = 2;

	// Optionally do something if the pxTimer parameter is NULL.
	configASSERT(pxTimer);

	// Which timer expired?
	lArrayIndex = ( int32_t ) pvTimerGetTimerID( pxTimer );

	// Increment the number of times that pxTimer has expired.
	lExpireCounters[ lArrayIndex ] += 1;

	// If the timer has expired 10 times then stop it from running.
//	if( lExpireCounters[ lArrayIndex ] == xMaxExpiryCountBeforeStopping )
//	{
//		// Do not use a block time if calling a timer API function from a
//		// timer callback function, as doing so could cause a deadlock!
//		xTimerStop( pxTimer, 0 );
////		LED_TOGGLE();
//		GUI_main( 0 );
//
//		if (TCA9554_Reg_map.Output.bit.P5)
//		    TCA9554_Reg_map.Output.bit.P5 = 0;
//		else
//		    TCA9554_Reg_map.Output.bit.P5 = 1;
//
//		i2c_buffer[0u] = CMD_PCA9554_CHANGE_OUTPUT;
//		i2c_new_command_flag = 1;
//	}


}
/*!
 * @brief Application entry point.
 */
int main(void)
{
    /* Define some initialization structures for different peripheral drivers */
    gpio_pin_config_t led_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
	adc_etc_config_t adcEtcConfig;
	adc_etc_trigger_config_t adcEtcTriggerConfig;
	adc_etc_trigger_chain_config_t adcEtcTriggerChainConfig;

    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    ADC_Configuration();

    /* Initialize the output LED GPIO. */
    GPIO_PinInit(LED_GPIO, LED_GPIO_PIN, &led_config);

    /* Initialize the ADC_ETC. */
    ADC_ETC_GetDefaultConfig(&adcEtcConfig);
    adcEtcConfig.XBARtriggerMask = 1U; /* Enable the external XBAR trigger0. */
    ADC_ETC_Init(DEMO_ADC_ETC_BASE, &adcEtcConfig);

    /* Set the external XBAR trigger0 configuration. */
    adcEtcTriggerConfig.enableSyncMode      = false;
    adcEtcTriggerConfig.enableSWTriggerMode = true;
    adcEtcTriggerConfig.triggerChainLength  = DEMO_ADC_ETC_CHAIN_LENGTH; /* Chain length is 1. */
    adcEtcTriggerConfig.triggerPriority     = 0U;
    adcEtcTriggerConfig.sampleIntervalDelay = 0U;
    adcEtcTriggerConfig.initialDelay        = 0U;
    ADC_ETC_SetTriggerConfig(DEMO_ADC_ETC_BASE, 0U, &adcEtcTriggerConfig);

    /* Set the external XBAR trigger0 chain0 configuration. */
    adcEtcTriggerChainConfig.enableB2BMode       = false;
    adcEtcTriggerChainConfig.ADCHCRegisterSelect = 1U
    		<< DEMO_ADC_CHANNEL_GROUP; /* Select ADC_HC0 register to trigger. */
    adcEtcTriggerChainConfig.ADCChannelSelect =
    		DEMO_ADC_ETC_CHANNEL; /* ADC_HC0 will be triggered to sample Corresponding channel. */
    adcEtcTriggerChainConfig.InterruptEnable = kADC_ETC_Done0InterruptEnable; /* Enable the Done0 interrupt. */

#if defined(FSL_FEATURE_ADC_ETC_HAS_TRIGm_CHAIN_a_b_IEn_EN) && FSL_FEATURE_ADC_ETC_HAS_TRIGm_CHAIN_a_b_IEn_EN
    adcEtcTriggerChainConfig.enableIrq = true; /* Enable the IRQ. */
#endif                                         /* FSL_FEATURE_ADC_ETC_HAS_TRIGm_CHAIN_a_b_IEn_EN */
    ADC_ETC_SetTriggerChainConfig(DEMO_ADC_ETC_BASE, 0U, 0U,
    		&adcEtcTriggerChainConfig); /* Configure the trigger0 chain0. */

    /* Enable the NVIC. */
    EnableIRQ(ADC_ETC_IRQ0_IRQn);

    /* Now set the several FreeRTOS tasks. */

    if (xTaskCreate(hello_task, "Hello_task", configMINIMAL_STACK_SIZE + 10, NULL, hello_task_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }

    // Create then start some timers.  Starting the timers before the scheduler
    // has been started means the timers will start running immediately that
    // the scheduler starts.
    int32_t x;
    for( x = 0; x < NUM_TIMERS; x++ )
    {
  	  xTimers[ x ] = xTimerCreate(    "Timer",       // Just a text name, not used by the kernel.
  			  //( 1000 * (x+1) ),   // The timer period in ticks.
  			  ( 200 * (x+1) ),   // The timer period in ticks.
  			  pdTRUE,        // The timers will auto-reload themselves when they expire.
  			  ( void * ) x,  // Assign each timer a unique id equal to its array index
  			  vTimerCallback // Each timer calls the same callback when it expires.
  			  );
  	  if( xTimers[ x ] == NULL )
  		  {
  		  // The timer was not created.
  		  PRINTF("The timer was not created.\r\n");
  		  }
  	  else
  	  {
  		  // Start the timer.  No block time is specified, and even if one was
  		  // it would be ignored because the scheduler has not yet been
  		  // started.
  		  if( xTimerStart( xTimers[ x ], 0 ) != pdPASS )
  		  {
  			  // The timer could not be set into the Active state.
  		  }
  	  }
    }

    // Starting the scheduler will start the timers running as they have already
    // been set into the active state
    vTaskStartScheduler();
    for (;;)
        ;
}

/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void hello_task(void *pvParameters)
{
    for (;;)
    {
        GPIO_PinWrite(LED_GPIO, LED_GPIO_PIN, 0U);
        g_pinSet = false;

        PRINTF("Hello from the amazing ARM world.\r\n");
        PRINTF("...System clock: %u Hz.\r\n", SystemCoreClock);
        PRINTF("and welcome to the ultrasonic anemometer demo\r\n");
        PRINTF("this a project is in its early stages\r\n");
        PRINTF("and aims at testing the feasibility of producing a \r\n");
        PRINTF("low-cost full digital highly accurate anemometer.\r\n");
        PRINTF("Currently, using %s:\r\n", DEMO_ADC_BASE);
        PRINTF("...ADC Full Range: %d\r\n", g_Adc_12bitFullRange);
        PRINTF("...ADC channel: %d\r\n", DEMO_ADC_USER_CHANNEL);

        GPIO_PinWrite(LED_GPIO, LED_GPIO_PIN, 1U);
        g_pinSet = true;

        vTaskSuspend(NULL);
    }
}




















///*!
// * @brief Main function.
// */
//int main(void)
//{
//    adc_etc_config_t adcEtcConfig;
//    adc_etc_trigger_config_t adcEtcTriggerConfig;
//    adc_etc_trigger_chain_config_t adcEtcTriggerChainConfig;
//
//    BOARD_ConfigMPU();
//    BOARD_InitPins();
//    BOARD_BootClockRUN();
//    BOARD_InitDebugConsole();
//
//    PRINTF("ADC_ETC_Software_Trigger_Conv Example Start!\r\n");
//
//    ADC_Configuration();
//
//    /* Initialize the ADC_ETC. */
//    ADC_ETC_GetDefaultConfig(&adcEtcConfig);
//    adcEtcConfig.XBARtriggerMask = 1U; /* Enable the external XBAR trigger0. */
//    ADC_ETC_Init(DEMO_ADC_ETC_BASE, &adcEtcConfig);
//
//    /* Set the external XBAR trigger0 configuration. */
//    adcEtcTriggerConfig.enableSyncMode      = false;
//    adcEtcTriggerConfig.enableSWTriggerMode = true;
//    adcEtcTriggerConfig.triggerChainLength  = DEMO_ADC_ETC_CHAIN_LENGTH; /* Chain length is 1. */
//    adcEtcTriggerConfig.triggerPriority     = 0U;
//    adcEtcTriggerConfig.sampleIntervalDelay = 0U;
//    adcEtcTriggerConfig.initialDelay        = 0U;
//    ADC_ETC_SetTriggerConfig(DEMO_ADC_ETC_BASE, 0U, &adcEtcTriggerConfig);
//
//    /* Set the external XBAR trigger0 chain0 configuration. */
//    adcEtcTriggerChainConfig.enableB2BMode       = false;
//    adcEtcTriggerChainConfig.ADCHCRegisterSelect = 1U
//                                                   << DEMO_ADC_CHANNEL_GROUP; /* Select ADC_HC0 register to trigger. */
//    adcEtcTriggerChainConfig.ADCChannelSelect =
//        DEMO_ADC_ETC_CHANNEL; /* ADC_HC0 will be triggered to sample Corresponding channel. */
//    adcEtcTriggerChainConfig.InterruptEnable = kADC_ETC_Done0InterruptEnable; /* Enable the Done0 interrupt. */
//#if defined(FSL_FEATURE_ADC_ETC_HAS_TRIGm_CHAIN_a_b_IEn_EN) && FSL_FEATURE_ADC_ETC_HAS_TRIGm_CHAIN_a_b_IEn_EN
//    adcEtcTriggerChainConfig.enableIrq = true; /* Enable the IRQ. */
//#endif                                         /* FSL_FEATURE_ADC_ETC_HAS_TRIGm_CHAIN_a_b_IEn_EN */
//    ADC_ETC_SetTriggerChainConfig(DEMO_ADC_ETC_BASE, 0U, 0U,
//                                  &adcEtcTriggerChainConfig); /* Configure the trigger0 chain0. */
//
//    /* Enable the NVIC. */
//    EnableIRQ(ADC_ETC_IRQ0_IRQn);
//
//    PRINTF("ADC Full Range: %d\r\n", g_Adc_12bitFullRange);
//    while (1)
//    {
//        g_AdcConversionDoneFlag = false;
//        PRINTF("Press any key to get user channel's ADC value.\r\n");
//        GETCHAR();
//        ADC_ETC_DoSoftwareTrigger(DEMO_ADC_ETC_BASE, 0U); /* Do software XBAR trigger0. */
//        while (!g_AdcConversionDoneFlag)
//        {
//        }
//        PRINTF("ADC conversion value is %d\r\n", g_AdcConversionValue);
//    }
//}
//
