#include "adc1.h"



__IO uint16_t ADC_ConvertedValue;

/**
  * @brief  ÅäÖÃADCµÄGPIO
  * @param  ÎŞ
  * @retval ÎŞ
  */
static void Rheostat_ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// Ê¹ÄÜ GPIO Ê±ÖÓ
	RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK, ENABLE);
		
	// ÅäÖÃ IO
	GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	    
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ; //²»ÉÏÀ­²»ÏÂÀ­
	GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStructure);			
}

/**
  * @brief  ÅäÖÃADC£¬DMA´«Êä
  * @param  ÎŞ
  * @retval ÎŞ
  */
static void Rheostat_ADC_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
  // ¿ªÆôADCÊ±ÖÓ
	RCC_APB2PeriphClockCmd(RHEOSTAT_ADC_CLK , ENABLE);
	
	// ------------------DMA Init ½á¹¹Ìå²ÎÊı ³õÊ¼»¯--------------------------
  // ADC1Ê¹ÓÃDMA2£¬Êı¾İÁ÷0£¬Í¨µÀ0£¬Õâ¸öÊÇÊÖ²á¹Ì¶¨ËÀµÄ
  // ¿ªÆôDMAÊ±ÖÓ
  RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_DMA_CLK, ENABLE); 
	// ÍâÉè»ùÖ·Îª£ºADC Êı¾İ¼Ä´æÆ÷µØÖ·
	DMA_InitStructure.DMA_PeripheralBaseAddr = RHEOSTAT_ADC_DR_ADDR;	
  // ´æ´¢Æ÷µØÖ·£¬Êµ¼ÊÉÏ¾ÍÊÇÒ»¸öÄÚ²¿SRAMµÄ±äÁ¿	
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&ADC_ConvertedValue;  
  // Êı¾İ´«Êä·½ÏòÎªÍâÉèµ½´æ´¢Æ÷	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
	// »º³åÇø´óĞ¡Îª1£¬»º³åÇøµÄ´óĞ¡Ó¦¸ÃµÈÓÚ´æ´¢Æ÷µÄ´óĞ¡
	DMA_InitStructure.DMA_BufferSize = 1;	
	// ÍâÉè¼Ä´æÆ÷Ö»ÓĞÒ»¸ö£¬µØÖ·²»ÓÃµİÔö
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  // ´æ´¢Æ÷µØÖ·¹Ì¶¨
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; 
  // // ÍâÉèÊı¾İ´óĞ¡Îª°ë×Ö£¬¼´Á½¸ö×Ö½Ú 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
  //	´æ´¢Æ÷Êı¾İ´óĞ¡Ò²Îª°ë×Ö£¬¸úÍâÉèÊı¾İ´óĞ¡ÏàÍ¬
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	
	// Ñ­»·´«ÊäÄ£Ê½
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  // DMA ´«ÊäÍ¨µÀÓÅÏÈ¼¶Îª¸ß£¬µ±Ê¹ÓÃÒ»¸öDMAÍ¨µÀÊ±£¬ÓÅÏÈ¼¶ÉèÖÃ²»Ó°Ïì
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  // ½ûÖ¹DMA FIFO	£¬Ê¹ÓÃÖ±Á¬Ä£Ê½
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  
  // FIFO ´óĞ¡£¬FIFOÄ£Ê½½ûÖ¹Ê±£¬Õâ¸ö²»ÓÃÅäÖÃ	
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
	// Ñ¡Ôñ DMA Í¨µÀ£¬Í¨µÀ´æÔÚÓÚÁ÷ÖĞ
  DMA_InitStructure.DMA_Channel = RHEOSTAT_ADC_DMA_CHANNEL; 
  //³õÊ¼»¯DMAÁ÷£¬Á÷Ïàµ±ÓÚÒ»¸ö´óµÄ¹ÜµÀ£¬¹ÜµÀÀïÃæÓĞºÜ¶àÍ¨µÀ
	DMA_Init(RHEOSTAT_ADC_DMA_STREAM, &DMA_InitStructure);
	// Ê¹ÄÜDMAÁ÷
  DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE);

  // -------------------ADC Common ½á¹¹Ìå ²ÎÊı ³õÊ¼»¯------------------------
	// ¶ÀÁ¢ADCÄ£Ê½
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  // Ê±ÖÓÎªfpclk x·ÖÆµ	
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  // ½ûÖ¹DMAÖ±½Ó·ÃÎÊÄ£Ê½	
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  // ²ÉÑùÊ±¼ä¼ä¸ô	20¸öÖÜÆÚ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;  
  ADC_CommonInit(&ADC_CommonInitStructure);
	
  // -------------------ADC Init ½á¹¹Ìå ²ÎÊı ³õÊ¼»¯--------------------------
	ADC_StructInit(&ADC_InitStructure);
  // ADC ·Ö±æÂÊ
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  // ½ûÖ¹É¨ÃèÄ£Ê½£¬¶àÍ¨µÀ²É¼¯²ÅĞèÒª	
  ADC_InitStructure.ADC_ScanConvMode = DISABLE; 
  // Á¬Ğø×ª»»	
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  //½ûÖ¹Íâ²¿±ßÑØ´¥·¢
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  //Íâ²¿´¥·¢Í¨µÀ£¬±¾Àı×ÓÊ¹ÓÃÈí¼ş´¥·¢£¬´ËÖµËæ±ã¸³Öµ¼´¿É
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  //Êı¾İÓÒ¶ÔÆë	
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  //×ª»»Í¨µÀ 1¸ö
  ADC_InitStructure.ADC_NbrOfConversion = 1;                                    
  ADC_Init(RHEOSTAT_ADC, &ADC_InitStructure);
  //---------------------------------------------------------------------------
	
  // ÅäÖÃ ADC Í¨µÀ×ª»»Ë³ĞòÎª1£¬µÚÒ»¸ö×ª»»£¬²ÉÑùÊ±¼äÎª84¸öÊ±ÖÓÖÜÆ
	//Ô¼Îª3us
  ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL, 1, ADC_SampleTime_84Cycles); 
  // Ê¹ÄÜDMAÇëÇó after last transfer (Single-ADC mode)
  ADC_DMARequestAfterLastTransferCmd(RHEOSTAT_ADC, ENABLE);
  // Ê¹ÄÜADC DMA
  ADC_DMACmd(RHEOSTAT_ADC, ENABLE);
  // Ê¹ÄÜADC
  ADC_Cmd(RHEOSTAT_ADC, ENABLE);  
  //¿ªÊ¼adc×ª»»£¬Èí¼ş´¥·¢
  ADC_SoftwareStartConv(RHEOSTAT_ADC);
}

/**
  * @brief  ADC1³õÊ¼»¯
  * @param  ÎŞ
  * @retval ÎŞ
  */
void ADC1_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
}
/*********************************************END OF FILE**********************/
