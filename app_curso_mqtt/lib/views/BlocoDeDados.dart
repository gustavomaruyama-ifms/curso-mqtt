import 'package:flutter/cupertino.dart';

class BlocoDeDados extends StatefulWidget{
  String unidadeDeMedida = "";
  String rotulo = "-";
  _BlocoDeDadosState state;

  BlocoDeDados(this.unidadeDeMedida, this.rotulo, {super.key}): state = _BlocoDeDadosState();

  @override
  State<BlocoDeDados> createState(){
    return state;
  }

  void setValue(double value){
    state.setValue(value);
  }
}

class _BlocoDeDadosState extends State<BlocoDeDados> {
  double valor = 0;
  @override
  Widget build(BuildContext context) {
    return  Padding(
        padding: EdgeInsets.all(10),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children:[
            Text("$valor ${widget.unidadeDeMedida}", style: TextStyle(fontSize: 30,fontWeight: FontWeight.bold),),
            Text(widget.rotulo)
          ]
        )
    );
  }

  void setValue(double value){
    setState(() {
      this.valor = value;
    });
  }
}