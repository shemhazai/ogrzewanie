#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#include <Config.h>
#include <Ethernet.h>
#include <Memory.h>
#include <SPI.h>

class WebServer {
public:
  WebServer(Config *aConf, Memory *aMemory);

  void handleClient();

private:
  EthernetServer *server;
  Config *conf;
  Memory *memory;

  void handleRequest(EthernetClient &client, String data);
  void findAndSetProperty(String name, float value);
};

#endif
