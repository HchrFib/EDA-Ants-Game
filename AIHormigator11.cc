#include "Player.hh"


/**
* Write the name of your player and save this file
* with the same name and .cc extension.
*/
#define PLAYER_NAME Hormigator11



struct PLAYER_NAME : public Player {


    /**
    * Factory: returns a new instance of this class.
    * Do not modify this function.
    */
    static Player* factory () {
        return new PLAYER_NAME;
    }

    /**
    * Types and attributes for your player can be defined here.
    */
    
    const int UNDEF = -1;
    
    int DI[20] = {0 , 1 , 0, -1,     0 , 2 , 0, -2,        0 , 3 , 0, -3,    0 , 4 , 0, -4,     0 ,  5 , 0, -5};
    int DJ[20] = {-1 , 0 , 1,  0,   -2 , 0 , 2,  0 ,      -3 , 0 , 3,  0 ,  -4 , 0 , 4,  0 ,   -5 ,  0 , 5,  0};
    typedef vector<vector<int>> D;
    typedef vector<vector<Pos>> P;
    typedef vector<vector<bool>> MOVIMENTS;
    typedef vector<vector<bool>> CHECK_BONUS;
    typedef vector<vector<bool>> CHECK_ENEMIE;

    

    struct Info{
        Pos actual;
        Pos previo;
        int d;
        Dir moviment;
        Info(Pos act , Pos pre , int d1, Dir mov ) : actual(act), previo(pre), d(d1), moviment(mov){}  // Constructor
        Info(){}
    };
    typedef stack<Info> PILA;

    
    struct My_player_Info{
        Pos position;
        string name;
        string funcion;
        My_player_Info(Pos pos, string na, string func) : position(pos), name(na),funcion(func){}  // Constructor
        My_player_Info(){}
    };
    struct My_enemy_Info{
        Pos position;
        string name;
        My_enemy_Info(Pos pos, string na) : position(pos), name(na){}  // Constructor
        My_enemy_Info(){}
    };
    const vector<Dir> dirs =  {  Up, Down, Left, Right };


    /**
    * Play method, invoked once per each round.
    */

    bool move_ok(Pos p) {
        for(Dir d : dirs) {
            if(pos_ok(p + d) and cell(p + d).id == -1) return true; //el orden las condiciones evita warnings
        }
        return false;
    }
    
    PILA bfs_bread(Pos p , int id_worker, CHECK_BONUS& CB, Pos& pos_found_bonus, vector<vector<bool>> DELIVERED) { 
        
        PILA Informacio;
        PILA Ruta;
        Info coord;

        D dist(board_rows(), vector<int>(board_cols(), UNDEF));
        Pos INF = Pos(-1,-1); 
        P prev(board_rows(), vector<Pos>(board_cols(), INF));
        queue<Pos> Q;
        Q.push(p);
        dist[p.i][p.j] = 0;

        while(not Q.empty()) {
            Pos p1 = Q.front();
            Q.pop();
            int i = p1.i;
            int j = p1.j;
            if((cell(p1).bonus == Bread or cell(p1).bonus == Leaf or cell(p1).bonus == Seed) and not CB[p1.i][p1.j] and not DELIVERED[p1.i][p1.j]) {
                //cerr << p1.i << "  AQUI HAY PAN  "<< p1.j << " y hormigator: " << id_worker << " dist : "<< dist[p1.i][p1.j] <<endl;
                pos_found_bonus = p1;
                Info coordenada;
                Pos previo = p1;
                bool parar = false;
                
                while(not Informacio.empty() and not parar) {

                    coordenada = Informacio.top();
                    Informacio.pop();
                   
                    if(coordenada.actual.i == previo.i and coordenada.actual.j == previo.j) {
                       
                        Ruta.push(coordenada);
                        previo = coordenada.previo;   

                    }
                }
        
                CB[p1.i][p1.j] = true;
                return Ruta;
            } else {
                for(Dir d : dirs) { 
                    if(pos_ok(p1 + d) and cell(p1 + d).id == -1 and dist[(p1+d).i][(p1+d).j] == UNDEF and cell(p1 + d).type != Water) {
                        dist[(p1+d).i][(p1+d).j] = dist[(p1).i][(p1).j] + 1;
                        Info linea(p1 + d, p1, dist[(p1 + d).i][(p1 + d).j], d);
                        Informacio.push(linea);
                        Q.push(p1 + d);

                    }
                }
            }
        }    
        return Ruta;
    }
    PILA bfs_attack(Pos p, int id_soldier, CHECK_ENEMIE& CE, map<int, My_enemy_Info>& citizens_enemies , map<int, My_player_Info>& citizens ) {
        PILA Informacio;
        PILA Ruta;
        Info coord;

        D dist(board_rows(), vector<int>(board_cols(), UNDEF));
        Pos INF = Pos(-1,-1); 
        P prev(board_rows(), vector<Pos>(board_cols(), INF));
        queue<Pos> Q;
        Q.push(p);
        dist[p.i][p.j] = 0;
        
        while(not Q.empty()) {
            Pos p1 = Q.front();
            Q.pop();
            int i = p1.i;
            int j = p1.j;
            
            if(citizens_enemies[cell(p1).id].name == "worker_enemy" and not CE[p1.i][p1.j]) {
                //cerr << p1.i << "  AQUI HAY PAN  "<< p1.j << " y hormigator: " << id_soldier << " dist : "<< dist[p1.i][p1.j] <<endl;
            
                Info coordenada;
                Pos previo = p1;
                bool parar = false;
                
                while(not Informacio.empty() and not parar) {

                    coordenada = Informacio.top();
                    Informacio.pop();
                   
                    if(coordenada.actual.i == previo.i and coordenada.actual.j == previo.j) {
                       
                        Ruta.push(coordenada);
                        previo = coordenada.previo;   

                    }
                }
        
                CE[p1.i][p1.j] = true;
                return Ruta;
            } else {
                for(Dir d : dirs) { 
                    if(pos_ok(p1 + d)  and citizens.find(cell(p1 + d).id) == citizens.end() and citizens_enemies[cell(p1 + d).id].name != "soldier_enemy" and citizens_enemies[cell(p1 + d).id].name != "queen_enemy" and dist[(p1+d).i][(p1+d).j] == UNDEF and cell(p1 + d).type != Water) {
                        dist[(p1+d).i][(p1+d).j] = dist[(p1).i][(p1).j] + 1;
                        Info linea(p1 + d, p1, dist[(p1 + d).i][(p1 + d).j], d);
                        Informacio.push(linea);
                        Q.push(p1 + d);

                    }
                }
            }
        }    
        return Ruta;
    }
    PILA bfs_leave_bonus_to_queen(Pos p , int id_worker, map<int, My_player_Info>& citizens, map<int, My_enemy_Info>& citizens_enemies, Pos& pos_found_queen, vector<vector<bool>> DELIVERED) { 
        
        PILA Informacio;
        PILA Ruta;
        Info coord;

        D dist(board_rows(), vector<int>(board_cols(), UNDEF));
        Pos INF = Pos(-1,-1); 
        P prev(board_rows(), vector<Pos>(board_cols(), INF)); 
        queue<Pos> Q;
        Q.push(p);
        dist[p.i][p.j] = 0;
        bool first = false;
        while(not Q.empty()) {
            Pos p1 = Q.front();
            Q.pop();
            int i = p1.i;
            int j = p1.j;
            
            int ii, jj;
            
           for(int k = 0; k < 20; ++k) {
                ii = p1.i + DI[k];
                jj = p1.j + DJ[k];
                
                if(pos_ok(ii,jj) and cell(p1).bonus == None and citizens[cell(Pos(ii,jj)).id].name == "queen" and not DELIVERED[p1.j][p1.j] and not first)  {
                    //cerr << "SIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII " << DELIVERED[p1.j][p1.j] << " Pos: " << p1 << endl;
                    first = true;         
                } 
            }

            if(first) {
                Pos previo = p1;
                //cerr << "Punto a buscarrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr: " << p1 << endl; 
                pos_found_queen = p1;      
                Info coordenada;
                /*if(pos_ok(p1 + Down + Down) and cell(p1 + Down + Down).id == -1) previo = p1 + Down + Down;
                else if(pos_ok(p1 + Right + Right) and cell(p1 + Right +  Right).id == -1) previo = p1 + Right+ Right;
                else if(pos_ok(p1 + Up + Up) and cell(p1 + Up + Up).id == -1) previo = p1 + Up + Up;
                else if(pos_ok(p1  + Left + Left) and cell(p1 + Left + Left).id == -1) previo = p1 + Left + Left;
                Info coordenada;*/
                
                bool parar = false;
                


                while(not Informacio.empty() and not parar) {

                    coordenada = Informacio.top();
                    Informacio.pop();
                   
                    if(coordenada.actual.i == previo.i and coordenada.actual.j == previo.j) {
                        
                        Ruta.push(coordenada);
                        previo = coordenada.previo;
                                        
                    }
                }
                return Ruta;
            } else {
                for(Dir d : dirs) { 
                    if(pos_ok(p1 + d)  //and citizens[cell(p1 +d).id].name != "soldier" 
                                       //and citizens[cell(p1 + d).id].name != "worker" 
                                       //and citizens_enemies.find(cell(p1 + d).id) == citizens_enemies.end() 
                                       and cell(p1 + d).bonus == None
                                       and cell(p1 + d).id == -1
                                       and dist[(p1+d).i][(p1+d).j] == UNDEF 
                                       and cell(p1 + d).type != Water) {
                        
                        dist[(p1+d).i][(p1+d).j] = dist[(p1).i][(p1).j] + 1;
                        Info linea(p1 + d, p1, dist[(p1 + d).i][(p1 + d).j], d);
                        Informacio.push(linea);
                        Q.push(p1 + d);

                    }
                }
            }
        }    
        return Ruta;
    }
    vector<int> players_enemies() {
        vector<int> enemies(num_players());
        for (int pl = 0; pl < num_players(); ++pl) {
            if (pl != me()) enemies[pl] = pl;  
        }    
        return enemies;    
    }
    bool bonus_cerca(Pos p, int id_my_queen) {
        for(int k = 0; k < 20; ++k) {
            int ii = p.i + DI[k];
            int jj = p.j + DJ[k];
                
            if(pos_ok(Pos(ii,jj)) and cell(Pos(ii,jj)).id == id_my_queen) return true; 
        }
        return false;
    }
    void bonus_cerca2(Pos p, vector<vector<bool>>& DELIVERED) {
        for(int k = 0; k < 20; ++k) {
            int ii = p.i + DI[k];
            int jj = p.j + DJ[k];
                
            if(pos_ok(Pos(ii,jj)) and cell(Pos(ii,jj)).bonus != None)  {
                DELIVERED[ii][jj] = 1;  
            } 
        }
    }
    virtual void play () {

        //cerr << board_rows() << " " << board_cols() << endl;
        MOVIMENTS M(board_rows(), vector<bool>(board_cols(), false));

        bool mov = false;

        /* ------------------- MY PLAYER INFO --------------------*/
        vector<int> queen1_ids = queens(me());
      
        map<int, My_player_Info> citizens;

        vector<int> workers_id = workers(me());
        vector<int> soldiers_id = soldiers(me());
        My_player_Info inf;

        vector<pair<int, AntType>>Hormigators;
        vector<vector<bool>> DELIVERED(board_cols(), vector<bool>(board_cols(), false));
        int id_my_queen = 0;
        Pos pos_my_queen;
        if (not queen1_ids.empty()) { 
            Ant queen1 = ant(queen1_ids[0]);
            inf.position = queen1.pos;
            id_my_queen = queen1_ids[0];
            pos_my_queen = queen1.pos;
            inf.name = "queen"; 
            citizens.insert(make_pair(queen1_ids[0],inf));
            Hormigators.push_back(make_pair(queen1_ids[0], Queen));
        }
        for(auto s_id : soldiers_id) {
            Ant my_soldier = ant(s_id);
            inf.name = "soldier";
            inf.position = my_soldier.pos;
            citizens.insert(make_pair(s_id, inf));
            Hormigators.push_back(make_pair(s_id, Soldier));
        }
        for(auto w_id : workers_id) {
            Ant my_worker = ant(w_id);
            inf.name = "worker";
            inf.position = my_worker.pos;
            inf.funcion = "desocupado";
            citizens.insert(make_pair(w_id, inf));
            Hormigators.push_back(make_pair(w_id, Worker));
        }
       
        //for(auto c : citizens) cerr << c.first << " " << c.second.name << " " << c.second.position << endl;
        
        /* ------------------- (FINAL) OF MY INFO PLAYER -------------------- */
        

        /* -------------------- MY ENEMIES INFO ----------------------------- */
        vector<int> players_enemies_id = players_enemies();
        
        vector<int> enemies_queens_id;
        vector<int> enemies_workers_id;
        vector<int> enemies_soldiers_id;
        
        map<int, My_enemy_Info> citizens_enemies;
        My_enemy_Info inf_enemy;
        
        for(auto enemy_id : players_enemies_id) {
            enemies_queens_id = queens(enemy_id);
            for(auto q_id : enemies_queens_id) {
                Ant queen1 = ant(q_id);
                inf_enemy.name = "queen_enemy";
                inf_enemy.position = queen1.pos;
                citizens_enemies.insert(make_pair(q_id, inf_enemy));
            } 
            enemies_workers_id = workers(enemy_id);
            for(auto w_id : enemies_workers_id) {
                Ant worker = ant(w_id);
                inf_enemy.name = "worker_enemy";
                inf_enemy.position = worker.pos;
                citizens_enemies.insert(make_pair(w_id, inf_enemy));
            }
            enemies_soldiers_id = soldiers(enemy_id);
            for(auto s_id : enemies_soldiers_id) {
                Ant soldier = ant(s_id);
                inf_enemy.name = "soldier_enemy";
                inf_enemy.position = soldier.pos;
                citizens_enemies.insert(make_pair(s_id, inf_enemy));
                    
            }
        }
        //cerr << "ENEMIES" << endl;
        //for(auto c : citizens_enemies) cerr << c.first << " " << c.second.name << " " << c.second.position << endl;
        /* --------------------(FINAL)OF MY ENEMIES INFO -------------------- */
        
        CHECK_BONUS  CB(board_rows(), vector<bool>(board_cols(), false));
        CHECK_ENEMIE CE(board_rows(), vector<bool>(board_cols(), false));
        
        
        Pos pos_found_bonus = Pos(-1,-1); 
        Pos pos_found_queen = Pos(-1,-1);
        
        /* ---------------------------CREACION MAPA -------------------------------- */
        /*vector<vector<char>> mapa(board_rows(), vector<char>(board_cols(), 'I'));
        for(int i = 0; i < board_rows(); ++i) {
            for(int j = 0; j < board_cols(); ++j) {
                 
                if(mapa[i][j] == 'E') mapa[i][j];
                else if(mapa[i][j] == 'F') mapa[i][j];
                else if(mapa[i][j] == 'G') mapa[i][j];
                
            }
        }*/
        /* ------------------------------------------------------------------------- */
         
                 
        bonus_cerca2(pos_my_queen, DELIVERED);
                
    
        /*for(int i = 0; i < board_rows(); ++i) {
            for(int j = 0; j < board_cols(); ++j) {
                cerr << DELIVERED[i][j] << " ";    
            }
            cerr << endl;
        }*/

        for(auto h_id : Hormigators) {
            
            

            Ant gladiator;
            Pos p;
            gladiator = ant(h_id.first);
            p = gladiator.pos;
            
            if(move_ok(p)) {
                PILA Ruta;
                Info coordenada;
               //cerr << bonus_cerca(p, id_my_queen) << endl;
                if(h_id.second == Worker) {
                    
                   

                    if(gladiator.bonus == None and cell(p).bonus != None and not bonus_cerca(p, id_my_queen)) {
                        take(h_id.first); 
                    
                    } else if(gladiator.bonus != None and bonus_cerca(p, id_my_queen)) {
                        leave(h_id.first);
                        DELIVERED[p.i][p.j] = 1;
      
                    } else {
                        if(gladiator.bonus == None)  Ruta = bfs_bread(p, h_id.first, CB, pos_found_bonus, DELIVERED);
                        else Ruta = bfs_leave_bonus_to_queen(p, h_id.first, citizens, citizens_enemies, pos_found_queen, DELIVERED); 

                        if(not Ruta.empty()) {
                            coordenada = Ruta.top();
                            Ruta.pop();
                            
                            Pos aux = p + coordenada.moviment;
                            if(not M[aux.i][aux.j]) {
                                move(h_id.first, coordenada.moviment);
                                //cerr << "Pos_dejar_reina: " << pos_found_queen << " pos gladiator " << p <<endl;
                                //cerr << "Posicion activa: " << p << " id worker que se mueve: " << h_id.first << " movimiento: "<< coordenada.moviment << endl;
                                M[aux.i][aux.j] = 1;
                                
                            }  
                        }
                    }


                   /* for(int i = 0; i < board_rows(); ++i) {
                        for(int j = 0; j < board_cols(); ++j) {
                           cerr << DELIVERED[i][j] << " ";    

                        }
                        cerr << endl;
                    }*/
            
                   
                                             
                    //cerr << pos_found_queen << " " << pos_found_bonus << " " << p << endl;
                }
                else if(h_id.second == Soldier) {
                    Ruta = bfs_attack(p, h_id.first, CE, citizens_enemies, citizens);
                    if(not Ruta.empty()) {
                        coordenada = Ruta.top();
                        Ruta.pop();

                        Pos aux = p + coordenada.moviment;
                        
                        if(not M[aux.i][aux.j]) {
                            move(h_id.first, coordenada.moviment);
                            M[aux.i][aux.j] = 1;
                        } 
                    }
                }
                
            }

        }  
    }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
