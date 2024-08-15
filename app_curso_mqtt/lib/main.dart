import 'package:app_curso_mqtt/views/PainelTelemetria.dart';
import 'package:flutter/material.dart';
import 'package:intl/date_symbol_data_local.dart';

const String titulo = 'Curso MQTT';

void main () {
    initializeDateFormatting('pt_br');
    runApp(MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      theme: ThemeData(useMaterial3: true),
      debugShowCheckedModeBanner: false,
      title: titulo,
      home: ViewNavigation()
    );
  }
}

class ViewNavigation extends StatefulWidget{
  @override
  State<StatefulWidget> createState() {
    return _ViewNavigationState();
  }
}

class _ViewNavigationState extends State<ViewNavigation>{
  @override
  Widget build(BuildContext context) {
    return DefaultTabController(
        initialIndex: 0,
        length: 1,
        child: Scaffold(
          appBar: AppBar(
            title: Text(titulo),
            bottom: const TabBar(
              tabs: <Widget>[
                Tab(
                  icon: Icon(Icons.percent),
                  text: "Telemetria",
                )
              ]
            )
          ),
          body: TabBarView(
            children: <Widget>[
              PainelTelemetria()
            ]
          )
      )
    );
  }
}