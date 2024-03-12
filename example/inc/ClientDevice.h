// ---------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 05/03/2024.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef FCM_CLIENTDEVICE_H
#define FCM_CLIENTDEVICE_H

#include "FcmDevice.h"

class ClientDevice : public FcmDevice
{
public:

    ClientDevice(int timeStepMsParam) : FcmDevice(timeStepMsParam) {}

    void initialize() override;
};


#endif //FCM_CLIENTDEVICE_H
