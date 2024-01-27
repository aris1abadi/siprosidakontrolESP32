#ifndef otaHandle_h
#define otaHandle_h

  
void registerToNodeJS(const char *hostName, const char *firmwareVersion, const char *macAddress, int wifiSignalStrength);
  void sendHeartbeatOta();
  void reRegisterDevice();
  void sendFirmwareFlashingInitiated();
  String getCommandFromServer();
  String urlEncode(const String& str);
  void processReceivedCommand(const String &command);

  void ota_loop();
  void ota_init();

#endif