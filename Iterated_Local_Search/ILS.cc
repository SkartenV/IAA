#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <ctime> 

using namespace std;

int Seed, NumIteraciones, CambioTemperatura;
string NombreInstancia;
double T_0, TasaDecrecimiento;
unsigned t0, t1;

// Funcion obtenida del GitLab del profesor Nicolas Rojas
void Capture_Params(int argc, char **argv){
    NombreInstancia = argv[1];
    T_0 = atof(argv[2]);
    TasaDecrecimiento = atof(argv[3]);
    NumIteraciones = atoi(argv[4]);
    CambioTemperatura = atoi(argv[5]);
    Seed = atoi(argv[6]);
}

// Funcion obtenida del GitLab del profesor Nicolas Rojas
int int_rand(int a, int b){
    int retorno = 0;

    if (a < b){
        retorno = (int) ((b - a) * drand48());
        retorno = retorno + a;
    }

    else{
        retorno = (int) ((a - b) * drand48());
        retorno = retorno + b;
    }

    return retorno;
}

// Funcion obtenida del GitLab del profesor Nicolas Rojas
float float_rand(float a, float b){
    float retorno = 0;

    if (a < b){
        retorno = (float) ((b - a) * drand48());
        retorno = retorno + a;
    }

    else{
        retorno = (float) ((a - b) * drand48());
        retorno = retorno + b;
    }

    return retorno;
}

// Funcion que realiza el movimiento de Simulated Annealing
vector <int> Movimiento(vector <int> Rutas, int CantidadNodos){

    int NodoNuevo, NodoOrigen, PosicionNueva;
    
    NodoNuevo = int_rand(1, CantidadNodos);
    NodoOrigen = Rutas[0];

    while(true){
        if(NodoNuevo != NodoOrigen && find(Rutas.begin(), Rutas.end(), NodoNuevo) == Rutas.end()){
            break;
        }
        else{
            NodoNuevo = int_rand(1, CantidadNodos);
        }
    }

    PosicionNueva = int_rand(1, Rutas.size()-2);
    
    while(true){
        if(Rutas[PosicionNueva] != NodoOrigen){
            break;
        }
        else{
            PosicionNueva = int_rand(1, Rutas.size()-2);
        }
    }

    Rutas[PosicionNueva] = NodoNuevo;

    return Rutas;
}

// Funcion que realiza la perturbacion de Iterated Local Search
vector <int> Perturbacion(vector <int> Rutas, int CantidadNodos){

    int NodoOrigen, Swap1, Swap2, aux;
    
    Swap1 = int_rand(1, Rutas.size()-2);
    NodoOrigen = Rutas[0];

    while(true){
        if(Rutas[Swap1] != NodoOrigen){
            break;
        }
        else{
            Swap1 = int_rand(1, Rutas.size()-2);
        }
    }

    Swap2 = int_rand(1, Rutas.size()-2);
    
    while(true){
        if(Rutas[Swap2] != NodoOrigen && Swap2 != Swap1){
            break;
        }
        else{
            Swap2 = int_rand(1, Rutas.size()-2);
        }
    }

    aux = Rutas[Swap1];
    Rutas[Swap1] = Rutas[Swap2];
    Rutas[Swap2] = aux;

    return Rutas;
}

// Funcion para ver si las restricciones del problema se cumplen
int VerificarRestricciones(vector <int> Rutas, vector <int> CapacidadesCamiones, vector <int> Producciones, int CantidadMinima){

    int i, NodoOrigen, Capacidad = 0, IdCamion = 0, CantidadAcumulada = 0;
    NodoOrigen = Rutas[0];

    for(i=0; (unsigned)i<Rutas.size(); i++){

        if(Rutas[i] == NodoOrigen && i != 0){
            IdCamion++;
            Capacidad = 0;
        }
        
        if(Rutas[i] != NodoOrigen){
            Capacidad = Capacidad + Producciones[Rutas[i]];
            CantidadAcumulada = CantidadAcumulada + Producciones[Rutas[i]];
        }
        
        if(Capacidad > CapacidadesCamiones[IdCamion])
            return 0;

    }
    
    if(CantidadAcumulada < CantidadMinima)
        return 0;

    return 1;
}

// Funcion para calcular la produccion de cada ruta
vector <int> CalculoProduccion(vector <int> Rutas, vector <int> Producciones){
    
    int i, NodoOrigen, Capacidad = 0, IdCamion = 0;
    vector <int> Distancias;
    NodoOrigen = Rutas[0]; // AQUI EL ERROR
    
    for(i=0; (unsigned)i<Rutas.size(); i++){

        if(Rutas[i] == NodoOrigen && i != 0){
            Distancias.push_back(Capacidad);
            IdCamion++;
            Capacidad = 0;
        }
        
        if(Rutas[i] != NodoOrigen){
            Capacidad = Capacidad + Producciones[Rutas[i]];
        }

    }

    return Distancias;

}

int main(int argc, char **argv){

    t0 = clock();

    Capture_Params(argc, argv);
    srand48(Seed);

    string delimiter = " ";

    size_t pos = 0;
    string token;
    
    ifstream instancia;
    string frase, Nodos;

    instancia.open("./Instancias/" + NombreInstancia);

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
    instanciaAux.open("./Instancias/" + NombreInstancia);
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

//------------------------------------------------- Simulated Annealing -------------------------------------------------//
    
    int x, NodoActual, NodoProx, S_current, S_best, S_new = 0, Factible;
    double T;
    vector <int> NewRutas = Rutas;
    vector <int> MovimientoRuta;

    // Asignacion de parametros
    //T_0 = 100;
    T = T_0;
    //TasaDecrecimiento = 0.98;
    S_current = FuncionEvaluacion;
    S_best = S_current;
    cont = 0;

    for(i=0;i<NumIteraciones;i++){

        while(true){
            // Se realiza el movimiento a la solucion actual
            MovimientoRuta = Movimiento(Rutas, CantidadNodos);

            // Se verifica si la solución es factible
            Factible = VerificarRestricciones(MovimientoRuta, ArregloCapacidadesCamiones, ArregloProducciones, Demanda);

            if(Factible == 1)
                break;
        }

        S_new = 0;
        
        // Se obtiene la funcion de evaluacion de la solucion con el movimiento aplicado
        for(x=0; (unsigned)x<MovimientoRuta.size()-1; x++){

            NodoActual = MovimientoRuta[x];
            NodoProx = MovimientoRuta[x+1];

            if(NodoActual == NodoOrigen)
                NodoActual = 0;

            if(NodoProx == NodoOrigen)
                NodoProx = 0;

            S_new = S_new + MatrizDistancias[NodoActual][NodoProx];

        }

        // Si la nueva solucion es mejor que la actual, la solucion actual pasa a tomar el valor de la nueva
        if(S_new < S_current)
            S_current = S_new;

        // Si un numero aleatorio entre 0 y 1 es menor a e^(delta eval / T), la solucion actual pasa a tomar el valor de la nueva
        else if(float_rand(0,1) < exp(-(S_new-S_current)/T))
            S_current = S_new;

        // Si la solucion actual es mejor que la mejor hasta el momento, la mejor solucion pasa a tomar el valor de la actual
        if(S_current < S_best){
            S_best = S_current;
            NewRutas.clear();
            NewRutas = MovimientoRuta;
        }

        cont++;

        // Actualizacion de la temperatura segun la tasa de decrecimiento
        if(cont == CambioTemperatura){
            T = T * TasaDecrecimiento;
            cont = 0;
        }

    }
    
    FuncionEvaluacion = S_best;

    vector <int> RutaPerturbada;
    RutaPerturbada = Perturbacion(NewRutas, CantidadNodos);
    
    for(i=0;i<RutaPerturbada.size();i++){
        cout << RutaPerturbada[i] << "-";
    }
    S_new = 0;
    for(x=0; (unsigned)x<MovimientoRuta.size()-1; x++){

        NodoActual = MovimientoRuta[x];
        NodoProx = MovimientoRuta[x+1];

        if(NodoActual == NodoOrigen)
            NodoActual = 0;

        if(NodoProx == NodoOrigen)
            NodoProx = 0;

        S_new = S_new + MatrizDistancias[NodoActual][NodoProx];

    }
    cout << S_new << endl;
    S_current = S_new;
    S_best = S_current;
    // Segundo Simulated Annealing
    for(i=0;i<NumIteraciones;i++){

        while(true){
            // Se realiza el movimiento a la solucion actual
            MovimientoRuta = Movimiento(RutaPerturbada, CantidadNodos);

            // Se verifica si la solución es factible
            Factible = VerificarRestricciones(MovimientoRuta, ArregloCapacidadesCamiones, ArregloProducciones, Demanda);

            if(Factible == 1)
                break;
        }

        S_new = 0;
        
        // Se obtiene la funcion de evaluacion de la solucion con el movimiento aplicado
        for(x=0; (unsigned)x<MovimientoRuta.size()-1; x++){

            NodoActual = MovimientoRuta[x];
            NodoProx = MovimientoRuta[x+1];

            if(NodoActual == NodoOrigen)
                NodoActual = 0;

            if(NodoProx == NodoOrigen)
                NodoProx = 0;

            S_new = S_new + MatrizDistancias[NodoActual][NodoProx];

        }

        // Si la nueva solucion es mejor que la actual, la solucion actual pasa a tomar el valor de la nueva
        if(S_new < S_current)
            S_current = S_new;

        // Si un numero aleatorio entre 0 y 1 es menor a e^(delta eval / T), la solucion actual pasa a tomar el valor de la nueva
        else if(float_rand(0,1) < exp(-(S_new-S_current)/T))
            S_current = S_new;

        // Si la solucion actual es mejor que la mejor hasta el momento, la mejor solucion pasa a tomar el valor de la actual
        if(S_current < S_best){
            S_best = S_current;
            NewRutas.clear();
            NewRutas = MovimientoRuta;
        }

        cont++;

        // Actualizacion de la temperatura segun la tasa de decrecimiento
        if(cont == CambioTemperatura){
            T = T * TasaDecrecimiento;
            cont = 0;
        }

    }

    FuncionEvaluacion = S_best;
    for(i=0;i<NewRutas.size();i++){
        cout << NewRutas[i] << "-";
    }
    cout << FuncionEvaluacion << endl;

    // Calculo de la cantidad de leche recogida por ruta
    CantidadLecheRutas.clear();
    CantidadLecheRutas = CalculoProduccion(NewRutas, ArregloProducciones);
    
    // Calculo de la cantidad total de leche recogida
    CantidadRecogida = 0;
    for(i=0; (unsigned)i<CantidadLecheRutas.size(); i++){
        CantidadRecogida = CantidadRecogida + CantidadLecheRutas[i];
    }
    
    // Calculo de la distancia recorrida por ruta
    int DistRuta = 0;
    NodoOrigen = NewRutas[0];
    DistanciaRutas.clear();
    
    for(i=0; (unsigned)i<NewRutas.size()-1; i++){

        NodoActual = NewRutas[i];
        NodoProx = NewRutas[i+1];

        if(NodoActual == NodoOrigen && i != 0){
            NodoActual = 0;
            DistanciaRutas.push_back(DistRuta);
            DistRuta = 0;
        }
            
        if(NodoActual == NodoOrigen)
            NodoActual = 0;

        if(NodoProx == NodoOrigen)
            NodoProx = 0;
        
        if(NodoActual != NodoOrigen){
            DistRuta = DistRuta + MatrizDistancias[NodoActual][NodoProx];
        }

    }
    
    // Escritura en el archivo de salida
    delimiter = ".";
    string NombreSalida = NombreInstancia.substr(0, NombreInstancia.find(delimiter));

    Salida.open("./Salidas/" + NombreSalida + ".out");
    Salida << "Funcion de evaluacion: " << FuncionEvaluacion << " / Cantidad total recogida: " << CantidadRecogida << " / Numero de rutas: " << CantidadRutas << "\n";
    Salida << NodoOrigen << "-";

    for(i=1;(unsigned)i<NewRutas.size();i++){
        if(NewRutas[i] == NodoOrigen){
            if(TerminoRuta != CantidadRutas - 1){
                Salida << NewRutas[i] << " / Distancia: " << DistanciaRutas[TerminoRuta] << " / Cantidad recogida: " << CantidadLecheRutas[TerminoRuta] << "\n";
                Salida << NewRutas[i] << "-";
                TerminoRuta++;
            }
            else
                Salida << NewRutas[i] << " / Distancia: " << DistanciaRutas[TerminoRuta] << " / Cantidad recogida: " << CantidadLecheRutas[TerminoRuta];
        }
        else
            Salida << NewRutas[i] << "-";
    }

    Salida.close();

    t1 = clock();
    double tiempo = (double(t1-t0)/CLOCKS_PER_SEC);
    cout << "Tiempo de ejecucion: " << tiempo << " segundos" << endl;

    return 0;

}