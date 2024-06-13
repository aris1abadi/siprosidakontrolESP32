//efine WIFI_SSID "siprosida"
//#define WIFI_PASSWORD "balingtan123"
//#define USE_ABADINET_BROKER
#define USE_ECLIPSE_BROKER

String ssid = "siprosida";
String pass = "balingtan123";

#ifdef USE_ECLIPSE_BROKER
String mqtt_host = "mqtt.eclipseprojects.io";
int mqtt_port = 1883;
#else
String mqtt_host = "abadinet.mooo.com";
int mqtt_port = 2000;
#endif

const char *serverAddress = "http://abadinet.mooo.com:2200"; // Replace with your Node.js server address
//const char *serverAddress = "http://abadinet.my.id:2200"; // Replace with your Node.js server address


//String version = "1.0.2";

char hostName[6] ;  
const char *firmwareVersion = "1.1.0";

String kontrolID = "SP0000";

//#define WIFI_SSID "BSIP_GH_NETWORK"
//#define WIFI_PASSWORD "balingtan123"

//#define MQTT_HOST "abadinet.my.id"
//#define MQTT_PORT 2000



