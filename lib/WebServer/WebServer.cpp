#include "WebServer.h"

WebServer::WebServer(Config *aConf, Memory *aMemory) {
  conf = aConf;
  memory = aMemory;

  byte mac[] = {0xDE, 0xAD, 0xBE, 0xC0, 0xA2, 0x74};
  IPAddress ip(192, 168, 2, 241);
  Ethernet.begin(mac, ip);

  const int PORT = 80;
  server = new EthernetServer(PORT);
  server->begin();
}

void WebServer::handleClient() {
  EthernetClient client = server->available();
  if (client) {
    String data = "";
    while (client.available()) {
      data += String(client.read());
    }
    handleRequest(client, data);
  }
}

void WebServer::handleRequest(EthernetClient &client, String data) {
  if (data.startsWith("set ")) {
    data = data.substring(4);
    while (data.indexOf("=") >= 0) {
      int a = data.indexOf('=');
      int b = data.indexOf(';');

      String name = data.substring(0, a);
      String strValue = data.substring(a + 1, b);
      float value = data.toFloat();

      findAndSetProperty(name, value);
    }
  } else if (data.equals("get")) {
    String response =
        String("{") + "  \"tz\":" + String(conf->tz) + ", \"tkw\":" +
        String(conf->tkw) + ", \"tskw\":" + String(conf->tskw) + ", \"tco\":" +
        String(conf->tco) + ", \"tb\":" + String(conf->tb) + ", \"tcwu\":" +
        String(conf->tcwu) + ", \"tkg\":" + String(conf->tkg) + ", \"tp\":" +
        String(conf->tp) + ", \"ztcwu\":" + String(conf->ztcwu) + ", \"kg\":" +
        String(conf->kg) + ", \"tw\":" + String(conf->tw) + ", \"zth\":" +
        String(conf->zth) + ", \"tcwuh\":" + String(conf->tcwuh) +
        ", \"minrb\":" + String(conf->minrb) + ", \"maxrb\":" +
        String(conf->maxrb) + ", \"ztos\":" + String(conf->ztos) +
        ", \"ztzs\":" + String(conf->ztzs) + "}";
    client.println(response);
    client.flush();
  }
}

void WebServer::findAndSetProperty(String name, float value) {
  if (name.equals("ztcwu")) {
    memory->writeZTCWU(value);
    conf->ztcwu = value;
  } else if (name.equals("kg")) {
    memory->writeKG(value);
    conf->kg = value;
  } else if (name.equals("tw")) {
    memory->writeTW(value);
    conf->tw = value;
  } else if (name.equals("zth")) {
    memory->writeZTH(value);
    conf->zth = value;
  } else if (name.equals("tcwuh")) {
    memory->writeTCWUH(value);
    conf->tcwuh = value;
  } else if (name.equals("minrb")) {
    memory->writeMINRB(value);
    conf->minrb = value;
  } else if (name.equals("maxrb")) {
    memory->writeMAXRB(value);
    conf->maxrb = value;
  } else if (name.equals("ztos")) {
    memory->writeZTOS(value);
    conf->ztos = value;
  } else if (name.equals("ztzs")) {
    memory->writeZTZS(value);
    conf->ztzs = value;
  }
}
