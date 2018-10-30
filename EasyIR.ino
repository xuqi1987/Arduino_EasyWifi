#include "EasyWifi-Globals.h"
// AC
void sendAC_CMD(String name,String characteristic, int value)
{
    LOG("sendAC_CMD");

    if (name.equals(String("GreeAc")))
    {
        if (characteristic.equals(String("TargetHeatingCoolingState")))
        {
            
            Serial.print("TargetHeatingCoolingState:");
            LOG(value);
        }

       if (characteristic.equals(String("TargetTemperature")))
       {
            Serial.print("TargetTemperature:");
            LOG(value);
       }
    }
}