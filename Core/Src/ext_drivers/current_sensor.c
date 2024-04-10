#include <ext_drivers/current_sensor.h>
#include <math.h>
void current_sensor_init(current_sensor_t *dev, ADC_HandleTypeDef *hadc_low,ADC_HandleTypeDef *hadc_high, uint32_t channel_low,uint32_t channel_high )
{
	dev->hadc_low = hadc_low;
	dev->hadc_high = hadc_high;
	dev->channel_low= channel_low;
	dev->channel_high= channel_high;
	dev->voltage_high = 0;
	dev->voltage_low = 0;
	dev->current_low = 0;
	dev->current_high = 0;
	dev->current = 0;
	dev->count_high =0;
	dev->count_low = 0;
}

// float current_sensor_current_read(current_sensor_t *dev)
// {	int Uc = 5;//default
	// int U0 = 2.5;//
	// int S = 0.0025;
	// return float dev->current = ((5/Uc)*voltage-U0)x1000/S;// formula
// }
float current_sensor_convert(current_sensor_t *dev)
{ 	
	float Vref = 3.3;
	float Uc = 5;//default
	float U0 = 2.5;//
	float S_low = 2.5;
	float S_high = 40;

	//   convert both counts to voltages
	dev->voltage_low  = (float) dev->count_low*Vref/4095; 
	dev->voltage_high = (float) dev->count_high*Vref/4095;
	//   convert low channel voltage to current
	dev->current_low = ((5/Uc)*dev->voltage_low-U0)*1000/S_low;
	//   convert high channel voltage to current
	dev->current_high =((5/Uc)*dev->voltage_high-U0)*1000/S_high;
//   if low channel > 50 amps
	if(dev->current_low >50){
		dev->current = dev->current_high;
		return dev->current_high;
	}
	else{
		dev->current = dev->current_low;
		return dev->current_low;
	}
}
