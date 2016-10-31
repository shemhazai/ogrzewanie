#include "WebServer.h"

WebServer::WebServer(Config *aConf) {
  conf = aConf;
  initEthernet();
  initServer();
}

void WebServer::initEthernet() {
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  IPAddress ip(192, 168, 2, 241);
  Ethernet.begin(mac, ip);
}

void WebServer::initServer() {
  const int PORT = 23;
  server = new EthernetServer(PORT);
  server->begin();
}

void WebServer::handleClient() {
  EthernetClient client = server->available();
  if (client) {
    String data = "";
    data.reserve(client.available());

    while (client.available()) {
      char ch = client.read();
      data += String(ch);
    }

    data.toLowerCase();
    data.trim();

    handleRequest(client, data);
  }
}

void WebServer::handleRequest(EthernetClient &client, String data) {
  if (data.equals("get")) {
    handleGet(client, data);
  } else if (data.startsWith("set ")) {
    data.remove(0, 4); // skip "set "
    handleSet(client, data);
  }

  delay(1);
  client.stop();
}

void WebServer::handleGet(EthernetClient &client, String data) {
  String response = "";
  response.reserve(256);

  response += "{";
  appendFirst(response, "tz", conf->tz);
  appendFloat(response, "tw", conf->tw);
  appendFloat(response, "tkw", conf->tkw);
  appendFloat(response, "tskw", conf->tskw);
  appendFloat(response, "tco", conf->tco);
  appendFloat(response, "tb", conf->tb);
  appendFloat(response, "ztb", conf->ztb);
  appendFloat(response, "tcwu", conf->tcwu);
  appendFloat(response, "tkg", conf->tkg);
  appendFloat(response, "tp", conf->tp);
  appendFloat(response, "ztcwu", conf->ztcwu);
  appendFloat(response, "kg", conf->kg);
  appendFloat(response, "ptw", conf->ptw);
  appendFloat(response, "zth", conf->zth);
  appendFloat(response, "tcwuh", conf->tcwuh);
  appendFloat(response, "minrb", conf->minrb);
  appendFloat(response, "maxrb", conf->maxrb);
  appendFloat(response, "ztos", conf->ztos);
  appendFloat(response, "ztzs", conf->ztzs);
  appendInt(response, "ipa", conf->ipa);
  appendBool(response, "pkw", conf->pkw);
  appendBool(response, "pko", conf->pko);
  appendBool(response, "pco", conf->pco);
  appendBool(response, "pcwu", conf->pcwu);
  response += "}";

  if (client) {
    client.println(response);
    client.flush();
  }
}

void WebServer::handleSet(EthernetClient &client, String data) {
  while (data.indexOf('=') != -1) {
    int equalSign = data.indexOf('=');
    int delimeter = data.indexOf(';');

    if (equalSign == -1 || delimeter == -1) {
      return;
    }

    String name = data.substring(0, equalSign);
    String str = data.substring(equalSign + 1, delimeter);
    float value = str.toFloat();

    if (value == 0.00 && !str.startsWith("0.00")) {
      return;
    }

    conf->setProperty(name, value);
    data.remove(0, delimeter + 1);
    data.trim();
  }
}

void WebServer::appendFirst(String &response, String name, float value) {
  response += "\"" + name + "\":" + String(value);
}

void WebServer::appendFloat(String &response, String name, float value) {
  response += ",\"" + name + "\":" + String(value);
}

void WebServer::appendInt(String &response, String name, int value) {
  response += ",\"" + name + "\":" + String(value);
}

void WebServer::appendBool(String &response, String name, bool value) {
  response += ",\"" + name + "\":" + String(value);
}
