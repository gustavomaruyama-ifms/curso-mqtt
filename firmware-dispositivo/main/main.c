#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "DHT.h"

#define WIFI_SSID "" 												//ssid da rede wifi
#define WIFI_PASSWORD ""											//senha da rede wifi
#define MQTT_BROKER_URI "" 											//endereco do broker mqtt
#define MQTT_CLIENT_ID "5faa9f3a0de5770bfcf02b94"					//deviceId
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""
#define MQTT_TOPIC_TEMPERATURA "/lab/temperatura"
#define MQTT_TOPIC_UMIDADE "/lab/umidade"

esp_mqtt_client_handle_t cliente_mqtt;

/**
 * M�todo que manipula os eventos MQTT
 */
void manipulador_de_eventos_mqtt(void *args, esp_event_base_t base, int32_t event_id, void *dados_do_evento) {
	printf("Entrou no manipulador de eventos do MQTT!\n");

	esp_mqtt_event_handle_t evento_mqtt = (esp_mqtt_event_handle_t) dados_do_evento;
	cliente_mqtt = evento_mqtt->client;

	//Evento realizado quando a conex�o for estabelecida (ao receber CONNACK)
	if (evento_mqtt->event_id == MQTT_EVENT_CONNECTED) {
		printf("CONNACK RECEBIDO:\n");
		printf("Conectado ao broker MQTT.\n");
		return;
	}

	//Evento realizado quando for desconectado
	if (evento_mqtt->event_id == MQTT_EVENT_DISCONNECTED) {
		printf("Desconectado do broker MQTT.\n");
		return;
	}

	//Evento realizado quando o subscribe for efetuado (ao receber SUBACK)
	if (evento_mqtt->event_id == MQTT_EVENT_SUBSCRIBED) {
		printf("SUBACK RECEBIDO.\n");
		return;
	}

	//Evento realizado quando o publish for efetuado (ao receber PUBACK) - somente quando o qos > 0
	if (evento_mqtt->event_id == MQTT_EVENT_PUBLISHED) {
		printf("PUBACK RECEBIDO.\n");
		return;
	}

	if (evento_mqtt->event_id == MQTT_EVENT_DATA) {
		printf("PUBLISH RECEBIDO:\n");
		printf("T�pico: %.*s.\n", evento_mqtt->topic_len, evento_mqtt->topic);
		printf("Mensagem: %.*s\n", evento_mqtt->data_len, evento_mqtt->data);
		return;
	}

	if (evento_mqtt->event_id == MQTT_EVENT_ERROR) {
		printf("MQTT Erro.\n");
		return;
	}
}

/**
 * M�todo que inicializa o cliente MQTT
 */
void iniciar_mqtt(void) {
	const esp_mqtt_client_config_t mqtt_config = {
			.uri = MQTT_BROKER_URI,			//Caminho do broker MQTT, definido na linha 12
			.client_id = MQTT_CLIENT_ID,	//Caminho do broker MQTT, definido na linha 13
			.username = MQTT_USERNAME,		//Caminho do broker MQTT, definido na linha 14
			.password =	MQTT_PASSWORD		//Caminho do broker MQTT, definido na linha 15
	};

	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, &manipulador_de_eventos_mqtt, client);
	esp_mqtt_client_start(client);
}

/**
 * M�todo que manipula os eventos da wifi
 */
void manipulador_de_eventos_wifi(void *arg, esp_event_base_t event_base, int32_t event_id, void *dados_do_evento) {
	printf("Entrou no manipulador de eventos do WiFi!\n");

	//Evento realizado quando a wifi for configurada e inicializada
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		printf("Modo Station de WiFi inicializado!\n");
		printf("Conectando com a rede WiFi.\n");
		esp_wifi_connect();
	}

	//Evento realizado quando a conex�o wifi for estabelecida
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
		printf("Conectado a rede WiFi.\n");
	}

	//Evento realizado quando o modulo receber um n�mero IP do DHCP do roteador
	if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t *event = (ip_event_got_ip_t*) dados_do_evento;
		printf("Endere�o IP obtido: %s.\n", ip4addr_ntoa(&event->ip_info.ip));
		iniciar_mqtt();
	}
}

/**
 * M�todo que inicializa a wifi
 */
void iniciar_wifi(void) {
	esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &manipulador_de_eventos_wifi, NULL);
	esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &manipulador_de_eventos_wifi, NULL);

	wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();

	wifi_config_t wifi_config = {
			.sta = {
					.ssid = WIFI_SSID,			//SSID da rede WIFI, definido na linha 10
					.password = WIFI_PASSWORD,  //Senha da rede WIFI, definido na linha 11
			}
	};

	esp_wifi_init(&config);
	esp_wifi_set_mode(WIFI_MODE_STA);
	esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
	esp_wifi_start();
}

/**
 * Funcao que realiza a leitura do sensor DHT22
 */
void ler_sensor_dht(void* pvParamters){
	setDHTgpio(GPIO_NUM_4); 						// Porta no qual o sensor est� conectado;

	while (1){
		printf("teste\n");
		readDHT();

		//float umidade = getHumidity(); 				// Realiza a leitura do sensor - getTemperature() - getHumidity()
		//float temperatura = getTemperature();

		float umidade = 30.2;
		float temperatura = 28.2;

		char umidadeString[5];
		sprintf(umidadeString, "%.1f", umidade);			// Converte o valor float e uma string

		char temperaturaString[5];
		sprintf(temperaturaString, "%.1f", temperatura);	// Converte o valor float e uma string

		if(!cliente_mqtt){
			continue;
		}

		esp_mqtt_client_publish(cliente_mqtt, MQTT_TOPIC_UMIDADE, umidadeString, 0, 1, 0);			// Realizar o publish
		esp_mqtt_client_publish(cliente_mqtt, MQTT_TOPIC_TEMPERATURA, temperaturaString, 0, 1, 0);	// Realizar o publish

		vTaskDelay(2000 / portTICK_RATE_MS);		// Espera 2 segundo para realizar a proxima leitura
	}
}

/**
 * M�todo principal - a aplica��o come�a aqui
 */
void app_main(void) {
	nvs_flash_init();
	tcpip_adapter_init();
	esp_event_loop_create_default();

	iniciar_wifi();

	xTaskCreatePinnedToCore(ler_sensor_dht, "ler_sensor_dht", 2*1024, NULL, 10, NULL, 1);
}

