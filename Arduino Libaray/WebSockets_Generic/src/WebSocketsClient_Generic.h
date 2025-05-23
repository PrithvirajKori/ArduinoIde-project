/****************************************************************************************************************************
  WebSocketsClient_Generic.h - WebSockets Library for boards
  
  Based on and modified from WebSockets libarary https://github.com/Links2004/arduinoWebSockets
  to support other boards such as  SAMD21, SAMD51, Adafruit's nRF52 boards, etc.
  
  Built by Khoi Hoang https://github.com/khoih-prog/WebSockets_Generic
  Licensed under MIT license
   
  @original file WebSocketsClient.h
  @date 20.05.2015
  @author Markus Sattler

  Copyright (c) 2015 Markus Sattler. All rights reserved.
  This file is part of the WebSockets for Arduino.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  Version: 2.3.2

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  2.1.3   K Hoang      15/05/2020 Initial porting to support SAMD21, SAMD51, nRF52 boards, such as AdaFruit Feather nRF52832,
                                  nRF52840 Express, BlueFruit Sense, Itsy-Bitsy nRF52840 Express, Metro nRF52840 Express, etc.
  2.2.1   K Hoang      18/05/2020 Bump up to sync with v2.2.1 of original WebSockets library
  2.2.2   K Hoang      25/05/2020 Add support to Teensy, SAM DUE and STM32. Enable WebSocket Server for new supported boards.
  2.2.3   K Hoang      02/08/2020 Add support to W5x00's Ethernet2, Ethernet3, EthernetLarge Libraries. 
                                  Add support to STM32F/L/H/G/WB/MP1 and Seeeduino SAMD21/SAMD51 boards.
  2.3.1   K Hoang      07/10/2020 Sync with v2.3.1 of original WebSockets library. Add ENC28J60 EthernetENC library support
  2.3.2   K Hoang      12/11/2020 Add RTL8720DN Seeed_Arduino_rpcWiFi library support
 *****************************************************************************************************************************/

#pragma once

#include "WebSockets_Generic.h"

class WebSocketsClient : protected WebSockets
{
  public:
#ifdef __AVR__
    typedef void (*WebSocketClientEvent)(WStype_t type, uint8_t * payload, size_t length);
#else
    typedef std::function<void(WStype_t type, uint8_t * payload, size_t length)> WebSocketClientEvent;
#endif

    WebSocketsClient(void);
    virtual ~WebSocketsClient(void);

    void begin(const char * host, uint16_t port, const char * url = "/", const char * protocol = "arduino");
    void begin(String host, uint16_t port, String url = "/", String protocol = "arduino");
    void begin(IPAddress host, uint16_t port, const char * url = "/", const char * protocol = "arduino");

    // KH
    void begin(IPAddress host, uint16_t port, String url = "/", String protocol = "arduino");

#if defined(HAS_SSL)
#ifdef SSL_AXTLS

    void beginSSL(const char * host, uint16_t port, const char * url = "/", const char * fingerprint = "", const char * protocol = "arduino");
    
    // KH
    void beginSSL(IPAddress host, uint16_t port, String url = "/", String fingerprint = "", String protocol = "arduino");
    //////
    
    void beginSSL(String host, uint16_t port, String url = "/", String fingerprint = "", String protocol = "arduino");

#else

    void beginSSL(const char * host, uint16_t port, const char * url = "/", const uint8_t * fingerprint = NULL, const char * protocol = "arduino");
    
    // KH
    void beginSSL(IPAddress host, uint16_t port, String url = "/", String fingerprint = "", String protocol = "arduino");
    //////
    
    void beginSslWithCA(const char * host, uint16_t port, const char * url = "/", BearSSL::X509List * CA_cert = NULL, const char * protocol = "arduino");

#endif    // SSL_AXTLS

    void beginSslWithCA(const char * host, uint16_t port, const char * url = "/", const char * CA_cert = NULL, const char * protocol = "arduino");

#endif    // HAS_SSL

    void beginSocketIO(const char * host, uint16_t port, const char * url = "/socket.io/?EIO=3", const char * protocol = "arduino");
    void beginSocketIO(String host, uint16_t port, String url = "/socket.io/?EIO=3", String protocol = "arduino");
    
    // KH
    void beginSocketIO(IPAddress host, uint16_t port, String url = "/socket.io/?EIO=3", String protocol = "arduino");
    //////

#if defined(HAS_SSL)
    void beginSocketIOSSL(const char * host, uint16_t port, const char * url = "/socket.io/?EIO=3", const char * protocol = "arduino");
    void beginSocketIOSSL(String host, uint16_t port, String url = "/socket.io/?EIO=3", String protocol = "arduino");
    
    // KH
    void beginSocketIOSSL(IPAddress host, uint16_t port, String url = "/socket.io/?EIO=3", String protocol = "arduino");
    //////
    
    void beginSocketIOSSLWithCA(const char * host, uint16_t port, const char * url = "/socket.io/?EIO=3", const char * CA_cert = NULL, const char * protocol = "arduino");
#endif

#if(WEBSOCKETS_NETWORK_TYPE != NETWORK_ESP8266_ASYNC)
    void loop(void);
#else
    // Async interface not need a loop call
    void loop(void) __attribute__((deprecated)) {}
#endif

    void onEvent(WebSocketClientEvent cbEvent);

    bool sendTXT(uint8_t * payload, size_t length = 0, bool headerToPayload = false);
    bool sendTXT(const uint8_t * payload, size_t length = 0);
    bool sendTXT(char * payload, size_t length = 0, bool headerToPayload = false);
    bool sendTXT(const char * payload, size_t length = 0);
    bool sendTXT(String & payload);
    bool sendTXT(char payload);

    bool sendBIN(uint8_t * payload, size_t length, bool headerToPayload = false);
    bool sendBIN(const uint8_t * payload, size_t length);

    bool sendPing(uint8_t * payload = NULL, size_t length = 0);
    bool sendPing(String & payload);

    void disconnect(void);

    void setAuthorization(const char * user, const char * password);
    void setAuthorization(const char * auth);

    void setExtraHeaders(const char * extraHeaders = NULL);

    void setReconnectInterval(unsigned long time);

    void enableHeartbeat(uint32_t pingInterval, uint32_t pongTimeout, uint8_t disconnectTimeoutCount);
    void disableHeartbeat();

    bool isConnected(void);

  protected:
    String _host;
    uint16_t _port;

#if defined(HAS_SSL)

#ifdef SSL_AXTLS
    String _fingerprint;
    const char * _CA_cert;
#define SSL_FINGERPRINT_NULL ""
#else
    const uint8_t * _fingerprint;
    BearSSL::X509List * _CA_cert;
#define SSL_FINGERPRINT_NULL NULL
#endif

#endif

    WSclient_t _client;

    WebSocketClientEvent _cbEvent;

    unsigned long _lastConnectionFail;
    unsigned long _reconnectInterval;
    unsigned long _lastHeaderSent;

    void messageReceived(WSclient_t * client, WSopcode_t opcode, uint8_t * payload, size_t length, bool fin);

    void clientDisconnect(WSclient_t * client);
    bool clientIsConnected(WSclient_t * client);

#if(WEBSOCKETS_NETWORK_TYPE != NETWORK_ESP8266_ASYNC)
    void handleClientData(void);
#endif

    void sendHeader(WSclient_t * client);
    void handleHeader(WSclient_t * client, String * headerLine);

    void connectedCb();
    void connectFailedCb();

    void handleHBPing();    // send ping in specified intervals

#if(WEBSOCKETS_NETWORK_TYPE == NETWORK_ESP8266_ASYNC)
    void asyncConnect();
#endif

    /**
           called for sending a Event to the app
           @param type WStype_t
           @param payload uint8_t
           @param length size_t
    */
    virtual void runCbEvent(WStype_t type, uint8_t * payload, size_t length)
    {
      if (_cbEvent)
      {
        _cbEvent(type, payload, length);
      }
    }
};

#include "WebSocketsClient_Generic-Impl.h"
