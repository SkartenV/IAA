#include <iostream>
#include <fstream>
#include <bits/stdc++.h> 

using namespace std;

int main(){

    string delimiter = " ";

    size_t pos = 0;
    string token;
    
    ifstream instancia;
    string frase, Nodos, NombreInstancia;

    while(true){
        // Se pide el nombre de la instancia al usuario
        cout << "Ingrese el nombre de la instancia (sin el .dat):" << endl;
        cin >> NombreInstancia;
        instancia.open("./Instancias/" + NombreInstancia + ".dat");
        if(instancia)
            break;
        else
            cout << "Archivo no existente." << endl; 
    }

    int i=0, j=0, flag = 0;

    string::size_type beg = 0;
    string::size_type end = 0;

    // Parametros
    int Demanda, CantidadNodos, CantidadCamiones, ValorInt, NodoOrigen;

    vector<int> ArregloNodos;
    vector<int> ArregloGranjas;
    vector<int> ArregloCamiones;
    vector<int> ArregloCapacidadesCamiones;
    vector<int> ArregloProducciones;
    vector<int> FilaMatriz;

//---------------------------------------------------- Leer Archivo ----------------------------------------------------//

    while (!instancia.eof()){

        getline(instancia, frase);

        // Nodos de la red
        if(frase.find("set Nodos") != string::npos){

            flag = 0;
            beg = frase.find("= ");
            end = frase.find(";", beg+1);
            Nodos = frase.substr(beg+1, end-beg-1);

            while ((pos = Nodos.find(delimiter)) != string::npos){

                ValorInt = 0;

                if(flag == 0){
                    flag = 1;
                    Nodos.erase(0, pos + delimiter.length());
                }
                else{
                    token = Nodos.substr(0, pos);
                    istringstream(token) >> ValorInt;
                    ArregloNodos.push_back(ValorInt);
                    Nodos.erase(0, pos + delimiter.length());
                }
                
            }

            istringstream(Nodos) >> ValorInt;
            ArregloNodos.push_back(ValorInt);

        }

        // Granjas productoras
        else if(frase.find("set I") != string::npos){

            flag = 0;
            beg = frase.find("= ");
            end = frase.find(";", beg+1);
            Nodos = frase.substr(beg+1, end-beg-1);

            while ((pos = Nodos.find(delimiter)) != string::npos){

                ValorInt = 0;

                if(flag == 0){
                    flag = 1;
                    Nodos.erase(0, pos + delimiter.length());
                }
                else{
                    token = Nodos.substr(0, pos);
                    istringstream(token) >> ValorInt;
                    ArregloGranjas.push_back(ValorInt);
                    Nodos.erase(0, pos + delimiter.length());
                }
                
            }

            istringstream(Nodos) >> ValorInt;
            ArregloGranjas.push_back(ValorInt);

        }
        
        // Camiones disponibles
        else if(frase.find("set K") != string::npos){

            flag = 0;
            beg = frase.find("= ");
            end = frase.find(";", beg+1);
            Nodos = frase.substr(beg+1, end-beg-1);

            while ((pos = Nodos.find(delimiter)) != string::npos){

                ValorInt = 0;

                if(flag == 0){
                    flag = 1;
                    Nodos.erase(0, pos + delimiter.length());
                }
                else{
                    token = Nodos.substr(0, pos);
                    istringstream(token) >> ValorInt;
                    ArregloCamiones.push_back(ValorInt);
                    Nodos.erase(0, pos + delimiter.length());
                }
                
            }

            istringstream(Nodos) >> ValorInt;
            ArregloCamiones.push_back(ValorInt);

        }

        // Capacidades de los camiones
        else if(frase.find("param Q") != string::npos){

            CantidadCamiones = ArregloCamiones.size();
            
            for(i=0; i<CantidadCamiones; i++){
                ValorInt = 0;
                getline(instancia, frase);
                istringstream(frase.substr(frase.find(' ') + 1)) >> ValorInt;
                ArregloCapacidadesCamiones.push_back(ValorInt);
            }

        }

        // Cantidad minima a recolectar
        else if(frase.find("param P") != string::npos){

            ValorInt = 0;
            getline(instancia, frase);
            istringstream(frase) >> ValorInt;
            Demanda = ValorInt;

        }

        // Produccion de cada granja
        else if(frase.find("param qu") != string::npos){

            CantidadNodos = ArregloNodos.size();

            for(i=0; i<CantidadNodos; i++){
                ValorInt = 0;
                getline(instancia, frase);
                istringstream(frase.substr(frase.find(' ') + 1)) >> ValorInt;
                ArregloProducciones.push_back(ValorInt);
            }

            istringstream(Nodos) >> ValorInt;
            ArregloProducciones.push_back(ValorInt);

        }
    }

    instancia.close();
    ifstream instanciaAux;
    instanciaAux.open("./Instancias/" + NombreInstancia + ".dat");
    int MatrizDistancias[CantidadNodos][CantidadNodos];

    while (!instanciaAux.eof()){
        
        getline(instanciaAux, frase);

        // Matriz de distancias
        if(frase.find("param c") != string::npos){
            
            while(frase.find("=") == string::npos)
                getline(instanciaAux, frase);

            for(i=0;i<CantidadNodos;i++){

                getline(instanciaAux, frase);

                for(j=0;j<CantidadNodos;j++){
                    
                    ValorInt = 0;
                    Nodos = frase.substr(frase.find(' ') + 1);

                    while ((pos = Nodos.find(delimiter)) != string::npos){

                        token = Nodos.substr(0, pos);
                        istringstream(token) >> ValorInt;
                        MatrizDistancias[i][j] = ValorInt;
                        Nodos.erase(0, pos + delimiter.length());
                        j++;
                        
                    }

                    istringstream(Nodos) >> ValorInt;
                    MatrizDistancias[i][j] = ValorInt;

                }
            }
        }

    }

    instanciaAux.close();

//------------------------------------------------------- Greedy -------------------------------------------------------//

    int ProxNodo, ActualNodo = 0, CapacidadActual = 0, NumCamion = 0, TerminoRuta = 0;
    int FuncionEvaluacion = 0, Dist = 0, CantidadRutas = 1, CantidadRecogida = 0, cont = 0;
    double Tasa, TasaMin = 99999;
    vector<int> Rutas;
    vector<int> DistanciaRutas;
    vector<int> CantidadLecheRutas;
    vector<int> NodosProhibidos;
    vector<double> Tasas;
    vector<double> TasasOrdenadas;
    ofstream Salida;

    NodoOrigen = ArregloNodos[0];

    // Se agrega el nodo origen en el arreglo de rutas
    Rutas.push_back(ArregloNodos[ActualNodo]);

    while(true){

        TasaMin = 99999;

        // Desde el nodo actual, se busca al nodo con menor tasa (distancia/produccion)
        for(i=0;i<CantidadNodos;i++){
            if(i != ActualNodo && find(Rutas.begin(), Rutas.end(), i) == Rutas.end() && find(NodosProhibidos.begin(), NodosProhibidos.end(), i) == NodosProhibidos.end()){
                Tasa = (static_cast<double>(MatrizDistancias[ActualNodo][i])) / (static_cast<double>(ArregloProducciones[i]));
                Tasas.push_back(Tasa);
                if(Tasa < TasaMin){
                    TasaMin = Tasa;
                    ProxNodo = i;
                }
            }
        }

        CapacidadActual = CapacidadActual + ArregloProducciones[ProxNodo];

        // Si sobrepasa la capacidad del camion, se vuelve a la planta y sale otro camion
        if(CapacidadActual > ArregloCapacidadesCamiones[NumCamion]){
            CapacidadActual = CapacidadActual - ArregloProducciones[ProxNodo];
            NodosProhibidos.push_back(ProxNodo);
            cont++;
            if(cont == CantidadNodos){
                Rutas.push_back(ArregloNodos[0]);
                Dist = Dist + MatrizDistancias[ActualNodo][0];
                FuncionEvaluacion = FuncionEvaluacion + MatrizDistancias[ActualNodo][0];

                DistanciaRutas.push_back(Dist);
                CantidadLecheRutas.push_back(CapacidadActual);

                // Se reinicia todo para que salga otro nuevo camion
                NodosProhibidos.clear();
                ActualNodo = 0;
                Dist = 0;
                CapacidadActual = 0;
                cont = 0;
                NumCamion++;
                CantidadRutas++;

                // Si no hay mas camiones disponibles se terminan las rutas
                if((unsigned)NumCamion == ArregloCamiones.size())
                    break;
            }
        }

        // Si no se sobrepasa la capacidad del camion, se sigue recorriendo a las granjas
        else{
            // Se agrega el nodo a la ruta
            Rutas.push_back(ProxNodo);

            // Se acumula la cantidad de leche recogida, la distancia recorrida y la funcion de evaluacion
            CantidadRecogida = CantidadRecogida + ArregloProducciones[ProxNodo];
            Dist = Dist + MatrizDistancias[ActualNodo][ProxNodo];
            FuncionEvaluacion = FuncionEvaluacion + MatrizDistancias[ActualNodo][ProxNodo];

            // Se actualiza el nodo actual
            ActualNodo = ProxNodo;

            // Si se iguala o supera la cantidad de leche minima, se termina la ejecucion
            if(CantidadRecogida >= Demanda){
                Rutas.push_back(ArregloNodos[0]);
                Dist = Dist + MatrizDistancias[ActualNodo][0];
                FuncionEvaluacion = FuncionEvaluacion + MatrizDistancias[ActualNodo][0];
                DistanciaRutas.push_back(Dist);
                CantidadLecheRutas.push_back(CapacidadActual);
                break;
            }

        }

    }

    // Escritura en el archivo de salida
    Salida.open("./Salidas/" + NombreInstancia + ".out");
    Salida << FuncionEvaluacion << " " << CantidadRecogida << " " << CantidadRutas << "\n";
    Salida << NodoOrigen << "-";

    for(i=1;(unsigned)i<Rutas.size();i++){
        if(Rutas[i] == NodoOrigen){
            if(TerminoRuta != CantidadRutas - 1){
                Salida << Rutas[i] << " " << DistanciaRutas[TerminoRuta] << " " << CantidadLecheRutas[TerminoRuta] << "\n";
                Salida << Rutas[i] << "-";
                TerminoRuta++;
            }
            else
                Salida << Rutas[i] << " " << DistanciaRutas[TerminoRuta] << " " << CantidadLecheRutas[TerminoRuta];
        }
        else
            Salida << Rutas[i] << "-";
    }
    
    Salida.close();

    return 0;

}
