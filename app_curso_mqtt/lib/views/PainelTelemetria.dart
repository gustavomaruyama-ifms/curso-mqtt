import 'package:app_curso_mqtt/views/BlocoDeDados.dart';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:flutter/foundation.dart' show kIsWeb;
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_browser_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'dart:math';

class PainelTelemetria extends StatefulWidget{
  @override
  State<PainelTelemetria> createState() => _PainelTelemetriaState();
}

class _PainelTelemetriaState extends State<PainelTelemetria> {
  final client = (kIsWeb?
                    MqttBrowserClient('', Random().toString()):
                    MqttServerClient('', Random().toString()));

  final String TOPICO_LAB_TEMPERATURA = "/lab/temperatura";
  final String TOPICO_LAB_UMIDADE = "/lab/umidade";

  BlocoDeDados blocoUmidade = BlocoDeDados("%","Umidade");
  BlocoDeDados blocoTemperatura = BlocoDeDados("º","Temperatura");

  Future<bool> conexaoMQTT() async{
    if(client.connectionStatus!.state == MqttConnectionState.connected){
      return true;
    }

    if(client is MqttServerClient) {
      MqttServerClient sClient = client as MqttServerClient;
      sClient.useWebSocket = true;
    }

    client.port = 8080;

    client.onConnected = () async {
      ScaffoldMessenger.of(context).showSnackBar(const SnackBar(
        content: Text("Conectado ao broker."),
      ));
      await client.subscribe(TOPICO_LAB_UMIDADE, MqttQos.atMostOnce);
      await client.subscribe(TOPICO_LAB_TEMPERATURA, MqttQos.atMostOnce);
    };
      // Callbacks
      try{
        await client.connect();
        client.updates!.listen((List<MqttReceivedMessage<MqttMessage?>>? pacote) {
          final topico = pacote?[0].topic;
          final mensagemMQTT = pacote![0].payload as MqttPublishMessage;
          final payload = MqttPublishPayload.bytesToStringAsString(mensagemMQTT.payload.message);
          if(topico == TOPICO_LAB_TEMPERATURA){
            double valor = double.parse(payload);
            blocoTemperatura.setValue(valor);
          }
          if(topico == TOPICO_LAB_UMIDADE){
            double valor = double.parse(payload);
            blocoUmidade.setValue(valor);
          }
        });
        return true;
    }on Exception catch(e){
      ScaffoldMessenger.of(context).showSnackBar(const SnackBar(
        content: Text("Problema ao conectar"),
      ));
      return false;
    }
  }

  @override
  Widget build(BuildContext context) {
    return Center(
      child:
        FutureBuilder<bool>(
          future: conexaoMQTT(),
          builder: (context, snapshot){
            if(snapshot.hasData){
              if (snapshot.data == true) {
                return buildPainel();
              }else{
                return Text("Não Conectado.");
              }
            }else{
              return CircularProgressIndicator();
            }
          }
          )
    );
  }

  Widget buildPainel(){
    return Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children:[
          blocoUmidade,
          blocoTemperatura
        ]
    );
  }
}