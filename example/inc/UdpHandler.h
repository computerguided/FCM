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

#define UDP_PORT 9001

class UdpHandler : public FcmAsyncInterfaceHandler
{
public:

    void initialize() override;

    // -- Interface functions --
    void startListening();
    void stopListening();
    void send(const std::string& message);

private:
    int socketHandle;
    struct sockaddr_in serverAddress;
    std::thread listenThread;
    bool listening = false;

    void initSocket();
    void receive(std::string& message) const;
};


#endif //FCM_UDPHANDLER_H
