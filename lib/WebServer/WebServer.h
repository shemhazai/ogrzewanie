#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <Config.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <SPI.h>

class WebServer {
public:
  WebServer(Config *aConf);
  void handleClient();

private:
  void handleRequest(EthernetClient &client, String data);
  void handleGet(EthernetClient &client, String data);
  void handleSet(EthernetClient &client, String data);
  void appendFirst(String &response, String name, float value);
  void appendFloat(String &response, String name, float value);
  void appendInt(String &response, String name, int value);
  void appendBool(String &response, String name, bool value);

  void initEthernet();
  void initServer();

  EthernetServer *server;
  Config *conf;
};

#endif
