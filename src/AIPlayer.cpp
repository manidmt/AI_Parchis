# include "AIPlayer.h"
# include "Parchis.h"

const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;
const int num_pieces = 3;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta

bool AIPlayer::move(){
    cout << "Realizo un movimiento automatico" << endl;

    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}

void AIPlayer::think(color & c_piece, int & id_piece, int & dice) const{

    /*
    switch(id){
        case 0:
            thinkAleatorio(c_piece, id_piece, dice);
            break;

        case 1:
            thinkAleatorioMasInteligente(c_piece, id_piece, dice);
            break;

        case 2:
            thinkFichaMasAdelantada(c_piece, id_piece, dice);
            break;

        case 3:
            thinkMejorOpcion(c_piece, id_piece, dice);
            break;
    }
    */

    
    // El siguiente código se proporciona como sugerencia para iniciar la implementación del agente.

    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta

    // Llamada a la función para la poda (los parámetros son solo una sugerencia, se pueden modificar).
    //valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
    //cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    // ----------------------------------------------------------------- //

    // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch(id){
        case 0:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
            break;
        case 1:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, Heuristica);
            break;
        /*case 2:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion2);
            break;*/
    }
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;
}




double AIPlayer::Poda_AlfaBeta(const Parchis & estado, int jugador, int profundidad, const int profundidad_max, color &c_piece, int &id_piece, int &dice, double alpha, double beta, double (*heuristica)(const Parchis &,int)) const{

    double valor;

    if(profundidad == profundidad_max or estado.gameOver())
        return heuristica(estado, jugador);
    
    
    if (jugador == estado.getCurrentPlayerId()){

        ParchisBros hijos = estado.getChildren();
        
        for(ParchisBros::Iterator it = hijos.begin(); it != hijos.end() and alpha < beta; ++it){

            double auxiliar = Poda_AlfaBeta(*it, jugador, profundidad+1, profundidad_max, c_piece, id_piece, dice, alpha, beta, heuristica);
            if(auxiliar > alpha){

                alpha = auxiliar;

                if(profundidad == 0){

                    c_piece = it.getMovedColor();
                    id_piece = it.getMovedPieceId();
                    dice = it.getMovedDiceValue();
                }

                if(beta <= alpha){

                    return beta;
                    break;
                }
            }
        }

        return alpha;

    } else {

        ParchisBros hijos = estado.getChildren();

        for(ParchisBros::Iterator it = hijos.begin(); it != hijos.end() and alpha < beta; ++it){

            double auxiliar = Poda_AlfaBeta(*it, jugador, profundidad+1, profundidad_max, c_piece, id_piece, dice, alpha, beta, heuristica);
            if(auxiliar < beta){
                beta = auxiliar;

                if(beta <= alpha)
                    return alpha;
                
            }
        }

        return beta;
    }
}


double AIPlayer::Heuristica(const Parchis &estado, int jugador) {

    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        double puntuacion_jugador = Puntuar(estado, jugador);
        double puntuacion_oponente = Puntuar(estado, oponente);
        return puntuacion_jugador - puntuacion_oponente;
    }
}

double AIPlayer::Puntuar(const Parchis &estado, int jugador){

     // 
    double puntuacion_0 = 0;
    double puntuacion_1 = 0;
    vector<color> colores = estado.getPlayerColors(jugador);
    int current_power = estado.getPowerBar(jugador).getPower();
    bool dado_especial;


   // bool dado_especial = estado.isSpecialDice(estado.getDice());

    // Primer color
    color current_color = colores[0];

    for (int i = 0; i < num_pieces; i++)
    {
        puntuacion_0 -= estado.distanceToGoal(current_color, i) * 3; // Distancia a la meta

        // Contamos las piezas que estan en un lugar seguro
        if (estado.isSafePiece(current_color, i))
        {
            puntuacion_0 += 70;
        }
    }

    // Priorizamos al que menos piezas tenga en casa y más en la meta
    puntuacion_0 -= estado.piecesAtHome(current_color) * 150;
    puntuacion_0 += estado.piecesAtGoal(current_color) * 100;

    if (find(estado.getAvailableNormalDices(current_color).begin(), estado.getAvailableNormalDices(current_color).end(), 100) != estado.getAvailableNormalDices(current_color).end()){

        Board board = estado.getBoard();
        int min_dist = 1000;
        vector<pair<color,int>> piezas_afectadas;
        for (int i = 0; i < estado.game_colors.size(); i++){
            color c = estado.game_colors[i];
            if (c != current_color){
                for (int j = 0; j < board.getPieces(c).size(); j++){
                    int dist = estado.distanceBoxtoBox(current_color, jugador, c, j);
                    Piece current_piece = board.getPiece(c, j);
                    if(current_piece.get_type() != boo_piece){
                        if(dist < min_dist and dist > 0){
                            min_dist = dist;
                            piezas_afectadas.clear();
                            piezas_afectadas.push_back(pair<color,int>(c,j));
                        }else if(dist == min_dist){
                            piezas_afectadas.push_back(pair<color,int>(c,j));
                        }
                    }
                }
            }
        }

        // Si el vector piezas_afectadas es vacío significa que el caparazón azul afectaría a mis piezas
        if (piezas_afectadas.empty())   puntuacion_0 -= 500;
        else                            puntuacion_0 += 300;
    }
    
    // Segundo color
    current_color = colores[1];

    for (int i = 0; i < num_pieces; i++)
    {
        puntuacion_1 -= estado.distanceToGoal(current_color, i) * 3; // Distancia a la meta

        if (estado.isSafePiece(current_color, i))
        {
            puntuacion_1 += 70;
        }
    }

    // Priorizamos al que menos piezas tenga en casa y más en la meta
    puntuacion_1 -= estado.piecesAtHome(current_color) * 150;
    puntuacion_1 += estado.piecesAtGoal(current_color) * 100;
/*
    if (find(estado.getAvailableNormalDices(current_color).begin(), estado.getAvailableNormalDices(current_color).end(), 100) != estado.getAvailableNormalDices(current_color).end()){

        Board board = estado.getBoard();
        int min_dist = 1000;
        vector<pair<color,int>> piezas_afectadas;
        for (int i = 0; i < estado.game_colors.size(); i++){
            color c = estado.game_colors[i];
            if (c != current_color){
                for (int j = 0; j < board.getPieces(c).size(); j++){
                    int dist = estado.distanceBoxtoBox(current_color, jugador, c, j);
                    Piece current_piece = board.getPiece(c, j);
                    if(current_piece.get_type() != boo_piece){
                        if(dist < min_dist and dist > 0){
                            min_dist = dist;
                            piezas_afectadas.clear();
                            piezas_afectadas.push_back(pair<color,int>(c,j));
                        }else if(dist == min_dist){
                            piezas_afectadas.push_back(pair<color,int>(c,j));
                        }
                    }
                }
            }
        }

        // Si el vector piezas_afectadas es vacío significa que el caparazón azul afectaría a mis piezas
        if (piezas_afectadas.empty())   puntuacion_1 -= 500;
        else                            puntuacion_1 += 300;
    }
*/
    // Ahora pasamos a combinar ambas puntuaciones
    int puntuacion_total;

    if (puntuacion_0 > puntuacion_1)
    {
        puntuacion_total = 2 * puntuacion_0 + 0.75 * puntuacion_1;
    }
    else if (puntuacion_1 > puntuacion_0)
    {
        puntuacion_total = 2 * puntuacion_1 + 0.75 * puntuacion_0;
    }
    else
    {
        puntuacion_total = puntuacion_0 + puntuacion_1;
    }

    // Dado especial

    if (find(estado.getAvailableNormalDices(jugador).begin(), estado.getAvailableNormalDices(jugador).end(), 100) != estado.getAvailableNormalDices(jugador).end()){

        if (0 <= current_power && current_power < 50)   puntuacion_total += 50;
        else if (50 <= current_power && current_power < 60) ; // casilla cercana
        else if (60 <= current_power && current_power < 65) puntuacion_total -= 500;
        else if (65 <= current_power && current_power < 70) puntuacion_total += 300;
        else if (70 <= current_power && current_power < 75) ; // casilla cercana
        else if (75 <= current_power && current_power < 80) puntuacion_total += 500;
        else if (80 <= current_power && current_power < 85) puntuacion_total -= 700;
        else if (85 <= current_power && current_power < 90) {
            
            Board board = estado.getBoard();
            int min_dist = 1000;
            vector<pair<color,int>> piezas_afectadas;
            for (int i = 0; i < estado.game_colors.size(); i++){
                color c = estado.game_colors[i];
                if (c != colores[0] && c != colores[1]){
                    for (int j = 0; j < board.getPieces(c).size(); j++){
                        int dist = estado.distanceToGoal(c, j);
                        Piece current_piece = board.getPiece(c, j);
                        if(current_piece.get_type() != boo_piece){
                            if(dist < min_dist and dist > 0){
                                min_dist = dist;
                                piezas_afectadas.clear();
                                piezas_afectadas.push_back(pair<color,int>(c,j));
                            }else if(dist == min_dist){
                                piezas_afectadas.push_back(pair<color,int>(c,j));
                            }
                        }
                    }
                }
            }

            // Si el vector piezas_afectadas es vacío significa que el caparazón azul afectaría a mis piezas
            if (piezas_afectadas.empty())   puntuacion_total -= 1000;
            else                            puntuacion_total += 300;
        }
        else if (90 <= current_power && current_power < 95) puntuacion_total -= 900;
        else if (95 <= current_power && current_power < 100) puntuacion_total += 200;
        else if (current_power == 100)                      puntuacion_total -= 1000;
    }

    return puntuacion_total;
}

double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.


    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

void AIPlayer::thinkAleatorio( color &c_piece, int &id_piece, int &dice) const{

    // IMPLEMENTACIÓN INICIAL DEL AGENTE
    // Esta implementación realiza un movimiento aleatorio.
    // Se proporciona como ejemplo, pero se debe cambiar por una que realice un movimiento inteligente
    //como lo que se muestran al final de la función.

    // OBJETIVO: Asignar a las variables c_piece, id_piece, dice (pasadas por referencia) los valores,
    //respectivamente, de:
    // - color de ficha a mover
    // - identificador de la ficha que se va a mover
    // - valor del dado con el que se va a mover la ficha.

    // El id de mi jugador actual.
    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableNormalDices(player);
    // Elijo un dado de forma aleatoria.
    dice = current_dices[rand() % current_dices.size()];

    // Se obtiene el vector de fichas que se pueden mover para el dado elegido
    current_pieces = actual->getAvailablePieces(player, dice);

    // Si tengo fichas para el dado elegido muevo una al azar.
    if (current_pieces.size() > 0)
    {
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]); // get<i>(tuple<...>) me devuelve el i-ésimo
        c_piece = get<0>(current_pieces[random_id]);  // elemento de la tupla
    }
    else
    {
        // Si no tengo fichas para el dado elegido, pasa turno (la macro SKIP_TURN me permite no mover).
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }
}

void AIPlayer::thinkAleatorioMasInteligente(color &c_piece, int &id_piece, int &dice) const
{
    // El número de mi jugador actual.
    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento.
    vector<int> current_dices;

    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;

    // Obtengo el vector de dados que puedo usar para el movimiento.
    // - actual->getAvailableNormalDices(player) -> solo dados disponibles en ese turno.
    // Importante : "getAvailableNormalDices" me da los dados que puedo usar en el turno actual.
    // Por ejemplo, si me tengo que contar 10 o 20 solo me saldrán los dados 10 y 20.
    // Puedo saber qué más dados tengo, aunque no los pueda usar en este turno, con:
    // - actual->getNormalDices(player) -> todos los dados
    current_dices = actual->getAvailableNormalDices(player);

    // En vez de elegir un dado al azar, miro primero cuáles tienen fichas que se puedan mover.
    vector<int> current_dices_que_pueden_mover_ficha;

    for (int i = 0; i < current_dices.size(); i++)
    {
        // Se obtiene el vector de fichas que se pueden mover para el dado elegido.
        current_pieces = actual->getAvailablePieces(player, current_dices[i]);

        // Si se pueden mover fichas para el dado actual, lo añado al vector de dados que pueden mover fichas.
        if (current_pieces.size() > 0)
        {
            current_dices_que_pueden_mover_ficha.push_back(current_dices[i]);
        }
    }

    // Si no tengo ningún dado que pueda mover fichas, paso turno con un dado al azar (la macro SKIP_TURN me permite no mover).
    if (current_dices_que_pueden_mover_ficha.size() == 0)
    {
        dice = current_dices[rand() % current_dices.size()];

        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }
    // En caso contrario, elijo un dado de forma aleatoria de entre los que pueden mover ficha.
    else
    {
        dice = current_dices_que_pueden_mover_ficha[rand() % current_dices_que_pueden_mover_ficha.size()];

        // Se obtiene el vector de fichas que se pueden mover para el dado elegido.
        current_pieces = actual->getAvailablePieces(player, dice);

        // Muevo una ficha al azar de entre las que se pueden mover.
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]);
        c_piece = get<0>(current_pieces[random_id]);
    }
}

void AIPlayer::thinkFichaMasAdelantada(color &c_piece, int &id_piece, int &dice) const
{
    // Elijo el dado haciendo lo mismo que el jugador anterior.
    thinkAleatorioMasInteligente(c_piece, id_piece, dice);
    // Tras llamar a esta función, ya tengo en dice el número de dado que quiero usar.
    // Ahora, en vez de mover una ficha al azar, voy a mover (o a aplicar
    // el dado especial a) la que esté más adelantada
    // (equivalentemente, la más cercana a la meta).
    int player = actual->getCurrentPlayerId();
    vector<tuple<color, int>> current_pieces = actual->getAvailablePieces(player, dice);
    int id_ficha_mas_adelantada = -1;
    color col_ficha_mas_adelantada = none;
    int min_distancia_meta = 9999;
    for (int i = 0; i < current_pieces.size(); i++)
    {
        // distanceToGoal(color, id) devuelve la distancia a la meta de la ficha [id] del color que le indique.
        color col = get<0>(current_pieces[i]);
        int id = get<1>(current_pieces[i]);
        int distancia_meta = actual->distanceToGoal(col, id);
        if (distancia_meta < min_distancia_meta)
        {
            min_distancia_meta = distancia_meta;
            id_ficha_mas_adelantada = id;
            col_ficha_mas_adelantada = col;
        }
        }
        // Si no he encontrado ninguna ficha, paso turno.
        if (id_ficha_mas_adelantada == -1)
        {
            id_piece = SKIP_TURN;
            c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
        }
        // En caso contrario, moveré la ficha más adelantada.
        else
        {
            id_piece = id_ficha_mas_adelantada;
            c_piece = col_ficha_mas_adelantada;
        }
}

void AIPlayer::thinkMejorOpcion(color &c_piece, int &id_piece, int &dice) const
{
    // Vamos a mirar todos los posibles movimientos del jugador actual accediendo a los hijos del estado actual.
    // Cuando ya he recorrido todos los hijos, la función devuelve el estado actual. De esta forma puedo saber
    // cuándo paro de iterar.
    // Para ello, vamos a iterar sobre los hijos con la función de Parchis getChildren().
    // Esta función devuelve un objeto de la clase ParchisBros, que es una estructura iterable
    // sobre la que se pueden recorrer todos los hijos del estado sobre el que se llama.
    ParchisBros hijos = actual->getChildren();

    bool me_quedo_con_esta_accion = false;

    // La clase ParchisBros viene con un iterador muy útil y sencillo de usar.
    // Al hacer begin() accedemos al primer hijo de la rama,
    // y cada vez que hagamos ++it saltaremos al siguiente hijo.
    // Comparando con el iterador end() podemos consultar cuándo hemos terminado de visitar los hijos.
    // Voy a moverme a la casilla siempre con la que gane más energía, salvo que me encuentre con
    // algún movimiento muy interesante, como comer fichas o llegar a la meta.
    int current_power = actual->getPowerBar(this->jugador).getPower();
    int max_power = -101; // Máxima ganancia de energía.

    for (ParchisBros::Iterator it = hijos.begin();
        it != hijos.end() and !me_quedo_con_esta_accion; ++it)
    {
        Parchis siguiente_hijo = *it;

        // Si en el movimiento elegido comiera ficha, llegara a la meta o ganara, me quedo con esa acción.
        // Termino el bucle en este caso.
        if (siguiente_hijo.isEatingMove() or
            siguiente_hijo.isGoalMove() or
            (siguiente_hijo.gameOver() and siguiente_hijo.getWinner() == this->jugador))
        {
            me_quedo_con_esta_accion = true;
            c_piece = it.getMovedColor();
            id_piece = it.getMovedPieceId();
            dice = it.getMovedDiceValue();
        }
        // En caso contrario, me voy quedando con el que me dé más energía.
        else
        {
            int new_power = siguiente_hijo.getPower(this->jugador);

            if (new_power - current_power > max_power)
            {
                c_piece = it.getMovedColor();
                id_piece = it.getMovedPieceId();
                dice = it.getMovedDiceValue();
                max_power = new_power - current_power;
            }
        }
    }

    // Si he encontrado una acción que me interesa, la guardo en las variables pasadas por referencia.
    // (Ya lo he hecho antes, cuando les he asignado los valores con el iterador).
}



