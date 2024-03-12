//
// Created by Fred Dijkstra on 05/03/2024.
//

#ifndef FCM_UDPHANDLER_H
#define FCM_UDPHANDLER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#include "FcmAsyncInterfaceHandler.h"
#include "UdpEvents.h"

#define UDP_PORT 9001

class UdpHandler : public FcmAsyncInterfaceHandler
{
public:

UdpHandler(const std::string& nameParam,
           const std::shared_ptr<FcmMessageQueue>& messageQueueParam,
           const std::map<std::string, std::any>& settingsParam);


    void initialize() override;

    // -- Interface functions --
    void enable();
    void disable();
    void send(const std::string& message);

private:
    int socketHandle = -1;
    struct sockaddr_in serverAddress = {0};
    std::thread listenThread;
    bool listening = false;

    void initSocket();
    void receive();
};


#endif //FCM_UDPHANDLER_H
