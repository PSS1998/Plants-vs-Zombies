#include "rsdl.hpp"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>

using namespace std;

#define FRONTYARD_X 30
#define FRONTYARD_Y 70
#define FRONTYARD_SQUERE_LENGTH 80
#define FRONTYARD_SQUERE_HEIGHT 98


//row_x hamon mokhtastae x dar row yi ke tosh hast ro moshakhas mikone
//column_y hamon mokhtastae y dar column yi ke tosh hast ro moshakhas mikone
struct pea_struct {
	int row_x;
	int column_y;
	int damage;
	int speed;
	int row;
};

struct sun_struct {
	int row_x;
	int column_y;
	int value;
};

struct nokhodplant_struct {
	int row_x;
	int column_y;
	int row;
	int column;
	int health;
	int cost;
	clock_t start;
    double duration;//baraye moshakhas kardane zamnae shelike dobare
    int pea_counter;
};

struct Wallnut_struct {
	int row_x;
	int column_y;
	int row;
	int column;
	int health;
	int cost;
};

struct sun_flower_struct {
	int row_x;
	int column_y;
	int row;
	int column;
	int health;
	int cost;
	clock_t start;
    double duration;
    int sun_counter;
};

struct zombies_struct {
	int row_x;
	int column_y;
	int row;
	int stoped_at;
	int speed;
	int bite;
	int health;
	int block;
};

struct point {
	int row;
	int column;
	int active;
};

struct zombies_waves {
	int waves;//tedade wave ha
	vector<int> zombies_num;//tedade zombie dar har wave
	vector<int> waves_length;//tole zamane har wave
};
//board hamon chamane 5*9 jelo khone hast
struct board {
	int selected;//baraye moshkahas kardane in ke pore ya na
	int left_threshold;
    int right_threshold;
    int top_threshold;
    int bottom_threshold;
    int pointer_x;//mokhtasate pishfarz dar khone baraye aks ha
    int pointer_y;
};
//baraye dokme shoro menu
bool start_game(int x,int y){
    int left_threshold = 250;
    int right_threshold = 550;
    int top_threshold = 545 ;
    int bottom_threshold = 575;
    if(x > left_threshold && x < right_threshold && y < bottom_threshold && y > top_threshold){
      return true;
    }
    return false;
}
//tamame aks haye roye safe ra be heman sorat ke hastand refresh mikonad
void reload(vector< vector<board> > &frontyard, window &win, vector< vector<zombies_struct> > &zombie, vector<int> &zombie_count, vector<nokhodplant_struct> &peashooter, vector<sun_flower_struct> &sun_flower, vector<Wallnut_struct> &Wallnut, vector<sun_struct> &sun, int &tick,int with_pointer, point pointer, vector<pea_struct> &pea, string &sun_avalible, string wave_string, int wave){
	win.draw_bg("./frontyard.png", 235, 0);
	for(int wave=0; wave<zombie.size(); wave++){
		for(int i=0; i<zombie_count[wave]; i++){
			win.draw_png(string("./Zombie.png"), zombie[wave][i].row_x, zombie[wave][i].column_y, 70, 112);
		}
	}
	for(int i=0; i<peashooter.size(); i++){
		win.draw_png(string("./Peashooter.png"), peashooter[i].row_x, peashooter[i].column_y, 70, 70);
	}
	for(int i=0; i<sun_flower.size(); i++){
		win.draw_png(string("./Sunflower.png"), sun_flower[i].row_x, sun_flower[i].column_y, 70, 80);
	}
	for(int i=0; i<Wallnut.size(); i++){
		if(Wallnut[i].health > 48)
			win.draw_png(string("./Wallnut_body.png"), Wallnut[i].row_x, Wallnut[i].column_y, 70, 70);
		if(Wallnut[i].health > 24 && Wallnut[i].health < 48)
			win.draw_png(string("./Wallnut_cracked1.png"), Wallnut[i].row_x, Wallnut[i].column_y, 70, 70);
		if(Wallnut[i].health < 24)
			win.draw_png(string("./Wallnut_cracked2.png"), Wallnut[i].row_x, Wallnut[i].column_y, 70, 70);
	}
	for(int i=0; i<sun.size(); i++){
		int x = sun[i].row_x;
		int y = sun[i].column_y;
		win.draw_png(string("./Sun3.png"), x,y,116, 116,tick);
		win.draw_png(string("./Sun2.png"), x+20,y+20,77, 79,-1*tick%360);
		win.draw_png(string("./Sun1.png"), x+43,y+43,32, 32);
	}
	for(int i=0; i<pea.size(); i++){
		win.draw_png(string("./pea.png"), pea[i].row_x, pea[i].column_y, 20, 20);
	}
	if(pointer.active==1 && with_pointer==1){
		win.draw_png(string("./pointer.png"), frontyard[pointer.row][pointer.column].pointer_x, frontyard[pointer.row][pointer.column].pointer_y, 30, 48);
	}
	win.show_text(sun_avalible, 0, 0);
	win.show_text(wave_string, 375, 0);
	tick += 5;
}		

void initializing_board(vector< vector<board> > &frontyard, int i , int j){
	frontyard[i][j].selected=0;
	frontyard[i][j].left_threshold=FRONTYARD_X+(FRONTYARD_SQUERE_LENGTH*j);
    frontyard[i][j].right_threshold=FRONTYARD_X+(FRONTYARD_SQUERE_LENGTH*(j+1));
    frontyard[i][j].top_threshold=FRONTYARD_Y+(FRONTYARD_SQUERE_HEIGHT*i);
    frontyard[i][j].bottom_threshold=FRONTYARD_Y+(FRONTYARD_SQUERE_HEIGHT*(i+1));
    frontyard[i][j].pointer_x=(((frontyard[i][j].left_threshold+frontyard[i][j].right_threshold)/2)+frontyard[i][j].left_threshold)/2;
    frontyard[i][j].pointer_y=(((frontyard[i][j].top_threshold+frontyard[i][j].bottom_threshold)/2)+frontyard[i][j].top_threshold)/2;
}
//baraye khondane etelate level ha
bool read_level(zombies_waves &zombies, string level_num, int level){
	ifstream levelfile;
	string levelfile_name;
	stringstream convert;
	convert << level;
	level_num = convert.str();
	levelfile_name = level_num + ".level";
	levelfile.open (levelfile_name.c_str());
	if(levelfile){
    	levelfile.seekg(7, levelfile.cur);
    	levelfile >> zombies.waves;
    	zombies.zombies_num.resize(zombies.waves);
    	levelfile.seekg(20, levelfile.cur);
    	for(int i=0; i<zombies.waves; i++){
    		levelfile >> zombies.zombies_num[i];
    	}
    	zombies.waves_length.resize(zombies.waves);
    	levelfile.seekg(14, levelfile.cur);
    	for(int i=0; i<zombies.waves; i++){
    		levelfile >> zombies.waves_length[i];
    	}
		levelfile.close();
		return 0;
	}
	else {
		cout << "Unable to open file" << endl;
		return 1;
	}
}

void save_level(int level){
	ofstream writesave;
	writesave.open ("savedata.txt");
	writesave << level;
}

int read_save(){
	int level_num;
	ifstream readsave;
	readsave.open ("savedata.txt");
	if(!readsave)
		return -1;
	readsave >> level_num;
	return level_num;
}
//bara taine randome inke zombie har wave dar che saniyeyi be zamin biyayad
vector<double> zombies_time(int wave, zombies_waves &zombies){
    int time_sum=0;
    double relative_time=0;
	vector<double> random_time;
	random_time.resize(zombies.zombies_num[wave]);
	for(int j = 0; j<zombies.zombies_num[wave]; j++)
    	random_time[j] = rand() % zombies.waves_length[wave] + 1;
    sort(random_time.begin(),random_time.end());
    return random_time;
}

void zombie_initialize(int wave, vector< vector<zombies_struct> > &zombie, vector<int> &zombie_count, int row){
	zombie[wave][zombie_count[wave]].row = row;
	zombie[wave][zombie_count[wave]].row_x = FRONTYARD_X+(9*FRONTYARD_SQUERE_LENGTH);
	zombie[wave][zombie_count[wave]].column_y = FRONTYARD_Y+(row*FRONTYARD_SQUERE_HEIGHT)-20;
	zombie[wave][zombie_count[wave]].stoped_at = -1;
	zombie[wave][zombie_count[wave]].speed = 16;
	zombie[wave][zombie_count[wave]].bite = 1;
	zombie[wave][zombie_count[wave]].health = 10;
}

void zombie_attack(int wave, zombies_waves &zombies, window &win, vector<double> &random_time, vector< vector<zombies_struct> > &zombie, vector<int> &zombie_count){
    int row = rand() % 5;
    win.draw_png(string("./Zombie.png"), FRONTYARD_X+(9*FRONTYARD_SQUERE_LENGTH),FRONTYARD_Y+(row*FRONTYARD_SQUERE_HEIGHT)-20,70, 112);
    win.update_screen();
    zombie_initialize(wave, zombie, zombie_count, row);
}

void zombie_move(window &win, vector< vector<zombies_struct> > &zombie, vector< vector<board> > &frontyard, int wave){
	for(int wave=0; wave<zombie.size(); wave++){
		for(int i=0; i<zombie[wave].size(); i++){
			for(int j=0; j<9; j++){
				if(zombie[wave][i].row_x < (frontyard[zombie[wave][i].row][j].right_threshold+10) && (zombie[wave][i].row_x - frontyard[zombie[wave][i].row][j].right_threshold) > -60 && frontyard[zombie[wave][i].row][j].selected == 1){
    				zombie[wave][i].stoped_at = 1;
    				zombie[wave][i].block = j;
    				break;
    			}
    			else {
    				zombie[wave][i].stoped_at = 0;
    			}
    		}
			if(zombie[wave][i].stoped_at != 1){
    			zombie[wave][i].row_x = zombie[wave][i].row_x - (zombie[wave][i].speed/8);
    		}
    	}
    }
}

void pea_move(window &win, vector<pea_struct> &pea){
	for(int i=0; i<pea.size(); i++){
    		pea[i].row_x = pea[i].row_x + (pea[i].speed/32);
    }
}

void pointer_move(window &win, vector< vector<board> > &frontyard, point &pointer, int arrow, vector< vector<zombies_struct> > &zombie, vector<int> &zombie_count, vector<nokhodplant_struct> &peashooter, vector<sun_flower_struct> &sun_flower, vector<Wallnut_struct> &Wallnut, vector<sun_struct> &sun, int tick, vector<pea_struct> &pea, string &sun_avalible, string wave_string, int wave){
	if(arrow==1 && pointer.row !=0)
    	pointer.row -=1;
    if(arrow==2 && pointer.row !=4)
    	pointer.row +=1;
    if(arrow==3 && pointer.column !=0)
    	pointer.column -=1;
    if(arrow==4 && pointer.column !=8)
    	pointer.column +=1;
    reload(frontyard, win, zombie, zombie_count, peashooter, sun_flower, Wallnut, sun, tick, 0, pointer , pea, sun_avalible, wave_string, wave);
	win.draw_png(string("./pointer.png"), frontyard[pointer.row][pointer.column].pointer_x, frontyard[pointer.row][pointer.column].pointer_y, 30, 48);
    win.update_screen();
    pointer.active = 1;
}

void nokhod_plant(window &win, vector< vector<board> > &frontyard, point &pointer, vector<nokhodplant_struct> &peashooter, int &peashooter_count, int &sun_reserved, stringstream &convert, string &sun_avalible){
	if(sun_reserved >= 100){
		if(frontyard[pointer.row][pointer.column].selected == 0){
			sun_reserved -= 100;
			peashooter.resize(peashooter_count+1);
			peashooter[peashooter_count].start = clock();
			peashooter[peashooter_count].row_x = frontyard[pointer.row][pointer.column].pointer_x-20;
			peashooter[peashooter_count].column_y = frontyard[pointer.row][pointer.column].pointer_y-20;
			peashooter[peashooter_count].row = pointer.row;
			peashooter[peashooter_count].column = pointer.column;
			peashooter[peashooter_count].health = 5;
			peashooter[peashooter_count].cost = 100;
			peashooter[peashooter_count].duration = 0;
			peashooter[peashooter_count].pea_counter=1;
			peashooter_count++;
			frontyard[pointer.row][pointer.column].selected = 1;
			win.draw_png(string("./Peashooter.png"), frontyard[pointer.row][pointer.column].pointer_x-20, frontyard[pointer.row][pointer.column].pointer_y-20, 70, 70);
    		convert.str("");
    		convert << sun_reserved;
			sun_avalible = convert.str();
    		win.show_text(sun_avalible, 0, 0);
    		win.update_screen();
    		pointer.active = 0;
		}
	}
}

void Wallnut_plant(window &win, vector< vector<board> > &frontyard, point &pointer, vector<Wallnut_struct> &Wallnut, int &Wallnut_count, int &sun_reserved, stringstream &convert, string &sun_avalible){
	if(sun_reserved >= 50){
		if(frontyard[pointer.row][pointer.column].selected == 0){
			sun_reserved -= 50;
			Wallnut.resize(Wallnut_count+1);
			Wallnut[Wallnut_count].row_x = frontyard[pointer.row][pointer.column].pointer_x-20;
			Wallnut[Wallnut_count].column_y = frontyard[pointer.row][pointer.column].pointer_y-20;
			Wallnut[Wallnut_count].row = pointer.row;
			Wallnut[Wallnut_count].column = pointer.column;
			Wallnut[Wallnut_count].health = 72;
			Wallnut[Wallnut_count].cost = 50;
			Wallnut_count++;
			frontyard[pointer.row][pointer.column].selected = 1;
			win.draw_png(string("./Wallnut_body.png"), frontyard[pointer.row][pointer.column].pointer_x-20, frontyard[pointer.row][pointer.column].pointer_y-20, 70, 70);
    		convert.str("");
    		convert << sun_reserved;
			sun_avalible = convert.str();
    		win.show_text(sun_avalible, 0, 0);
    		win.update_screen();
    		pointer.active = 0;
		}
	}
}

void sun_flower_planting(window &win, vector< vector<board> > &frontyard, point &pointer, vector<sun_flower_struct> &sun_flower, int &sun_flower_count, int &sun_reserved, stringstream &convert, string &sun_avalible){
	if(sun_reserved >= 50){
		if(frontyard[pointer.row][pointer.column].selected == 0){
			sun_reserved -= 50;
			sun_flower.resize(sun_flower_count+1);
			sun_flower[sun_flower_count].start = clock();
			sun_flower[sun_flower_count].row_x = frontyard[pointer.row][pointer.column].pointer_x-20;
			sun_flower[sun_flower_count].column_y = frontyard[pointer.row][pointer.column].pointer_y-20;
			sun_flower[sun_flower_count].row = pointer.row;
			sun_flower[sun_flower_count].column = pointer.column;
			sun_flower[sun_flower_count].health = 5;
			sun_flower[sun_flower_count].cost = 50;
			sun_flower[sun_flower_count].duration = 0;
			sun_flower[sun_flower_count].sun_counter=1;
			sun_flower_count++;
			frontyard[pointer.row][pointer.column].selected = 1;
			win.draw_png(string("./Sunflower.png"), frontyard[pointer.row][pointer.column].pointer_x-20, frontyard[pointer.row][pointer.column].pointer_y-20, 70, 80);
    		convert.str("");
    		convert << sun_reserved;
			sun_avalible = convert.str();
    		win.show_text(sun_avalible, 0, 0);
    		win.update_screen();
    		pointer.active = 0;
		}
	}
}

void initializing_sun(vector<sun_struct> &sun, window &win){
	sun_struct s;
	s.value = 25;
	s.row_x = rand()%640+10;
    s.column_y = rand()%420+40;
    sun.push_back(s);
}

void draw_sun(window &win,vector<sun_struct> &sun,int tick){
	int x = sun[sun.size()-1].row_x;
	int y = sun[sun.size()-1].column_y;
	win.draw_png(string("./Sun3.png"), x,y,116, 116,tick);
	win.draw_png(string("./Sun2.png"), x+20,y+20,77, 79,-1*tick%360);
	win.draw_png(string("./Sun1.png"), x+43,y+43,32, 32);
	win.update_screen();
}
//baraye check kardane inke khorshida click shodan ya na
void check_suns(window &win, int x,int y,vector<sun_struct> &sun, int &sun_reserved, int &reload_flag, stringstream &convert, string &sun_avalible){
	for (int i = sun.size()-1; i >= 0 ;i--){
		int left_threshold = sun[i].row_x;
		int right_threshold = sun[i].row_x + 116 - 10;
		int top_threshold = sun[i].column_y ;
		int bottom_threshold = sun[i].column_y + 116 - 10;
		if(x>left_threshold && x<right_threshold && y<bottom_threshold && y>top_threshold){
			sun.erase(sun.begin()+i);
			sun_reserved += 25;
			convert.str("");
			convert << sun_reserved;
			sun_avalible = convert.str();
			win.show_text(sun_avalible, 0, 0);
			reload_flag = 1;
			return;
		}
	}
}

void initializing_sunforflower(vector< vector<board> > &frontyard, vector<sun_struct> &sun, vector<sun_flower_struct> &sun_flower, int wich_flower){
	sun_struct s;
	s.value = 25;
	s.row_x = frontyard[sun_flower[wich_flower].row][sun_flower[wich_flower].column].pointer_x-40;
    s.column_y = frontyard[sun_flower[wich_flower].row][sun_flower[wich_flower].column].pointer_y-40;
    sun.push_back(s);
}

void initializing_pea(vector< vector<board> > &frontyard, vector<pea_struct> &pea, vector<nokhodplant_struct> &peashooter, int wich_peashooter){
	pea_struct p;
	p.row = peashooter[wich_peashooter].row;
	p.damage = 1;
	p.speed = 640;
	p.row_x = frontyard[peashooter[wich_peashooter].row][peashooter[wich_peashooter].column].pointer_x+40;
    p.column_y = frontyard[peashooter[wich_peashooter].row][peashooter[wich_peashooter].column].pointer_y;
    pea.push_back(p);
}

void draw_pea(window &win,vector<pea_struct> &pea){
	int x = pea[pea.size()-1].row_x;
	int y = pea[pea.size()-1].column_y;
	win.draw_png(string("./pea.png"), x,y,20, 20);
	win.update_screen();
}

void zombie_eat(window &win, vector< vector<board> > &frontyard, vector< vector<zombies_struct> > &zombie, vector<nokhodplant_struct> &peashooter, vector<sun_flower_struct> &sun_flower, vector<Wallnut_struct> &Wallnut, vector<int> &zombie_count, int wave, int &peashooter_count, int &sun_flower_count, int &Wallnut_count){
	for(int wave=0; wave<zombie.size(); wave++){
		for(int i=0; i<zombie[wave].size(); i++){
			if(zombie[wave][i].stoped_at == 1){
				for(int j=0; j<peashooter.size(); j++){
					if(peashooter[j].row == zombie[wave][i].row && peashooter[j].column == zombie[wave][i].block){
						peashooter[j].health -= zombie[wave][i].bite;
						if(peashooter[j].health == 0){
							frontyard[peashooter[j].row][peashooter[j].column].selected = 0;
							peashooter.erase(peashooter.begin()+j);
							peashooter_count--;
						}
					}
				}
				for(int j=0; j<sun_flower.size(); j++){
					if(sun_flower[j].row == zombie[wave][i].row && sun_flower[j].column == zombie[wave][i].block){
						sun_flower[j].health -= zombie[wave][i].bite;
						if(sun_flower[j].health == 0){
							frontyard[sun_flower[j].row][sun_flower[j].column].selected = 0;
							sun_flower.erase(sun_flower.begin()+j);
							sun_flower_count--;
						}
					}
				}
				for(int j=0; j<Wallnut.size(); j++){
					if(Wallnut[j].row == zombie[wave][i].row && Wallnut[j].column == zombie[wave][i].block){
						Wallnut[j].health -= zombie[wave][i].bite;
						if(Wallnut[j].health == 0){
							frontyard[Wallnut[j].row][Wallnut[j].column].selected = 0;
							Wallnut.erase(Wallnut.begin()+j);
							Wallnut_count--;
						}
					}
				}
			}
		}
	}
}

void pea_hit(vector< vector<zombies_struct> > &zombie, vector<pea_struct> &pea, vector<int> &zombie_count, int wave){
	for(int i=0; i<pea.size(); i++){
		for(int wave=0; wave<zombie.size(); wave++){
			for(int j=0; j<zombie_count[wave]; j++){
				if(pea[i].row == zombie[wave][j].row && (zombie[wave][j].row_x - pea[i].row_x) < 20 && (zombie[wave][j].row_x - pea[i].row_x) > -20){
					zombie[wave][j].health -= pea[i].damage;
					pea.erase(pea.begin()+i);
					if(zombie[wave][j].health == 0){
						zombie[wave].erase(zombie[wave].begin()+j);
						zombie_count[wave]--;
					}
					return;
				}
			}
		}
	}
}

bool check_win(zombies_waves &zombies, vector< vector<zombies_struct> > &zombie, int wave, int level, int total_zombiez_num, int zombie_finished){
	if(wave == zombies.waves-1 && total_zombiez_num == 0 && zombie_finished == 1)
		return 1;
	return 0;
}

bool check_lose(vector< vector<zombies_struct> > &zombie, vector<int> &zombie_count, int wave){
	for(int wave=0; wave<zombie.size(); wave++)
		for(int j=0; j<zombie_count[wave]; j++)
			if(zombie[wave][j].row_x < -5)
				return 1;
	return 0;
}

int main(){
	int finished=0;/*bara taine inke zambiehaye ye level hame mordan ya na*/
	int zombie_finished=0;/*baraye taine inke akharin zambie wave omad ya na*/
	string level_num = "1";
	bool winner=0;
	bool lose=0;
	stringstream convert;
	string sun_avalible;
	stringstream convert2;
	string wave_string;
	int reload_flag=0;
	int tick = 0;
	point pointer;
	pointer.row=0;
	pointer.column=0;
	int time_zombie=1;
	int time_pea=1;
	int time_sun=1;
	int time_eat=1;
	srand(time(NULL));
	int sun_reserved = 50;
	convert.str("");
	convert << sun_reserved;
	sun_avalible = convert.str();
	vector<int> zombie_count;
	int total_zombiez_num=0;
	int peashooter_count=0;
	int sun_flower_count=0;
	int Wallnut_count=0;
	int sun_count=0;
	clock_t start;
	clock_t end;
    double duration;
    clock_t start2;
    clock_t end2;
    double duration2;
	vector<nokhodplant_struct> peashooter;
	vector<Wallnut_struct> Wallnut;
	vector<sun_flower_struct> sun_flower;
	vector< vector<zombies_struct> > zombie;
	zombies_waves zombies;
	vector< vector<board> > frontyard;
	vector<sun_struct> sun;
	vector<pea_struct> pea;
	vector<double> random_time;
    frontyard.resize(5);
    for(int i = 0; i<5; i++)
    	frontyard[i].resize(9);
    
    for(int i = 0; i<5; i++)
    	for(int j=0; j<9; j++)
    		initializing_board(frontyard, i, j);

//safe menu aval
	window win(800, 600);
	win.draw_bg("./menu.png", 0, 0);
	win.update_screen();
	bool next_page = true;
	while(next_page){
        HANDLE({
        	QUIT({return 0;})
  		    KEY_PRESS(q, {return 0;})
        	LCLICK({
              if(start_game(mouse_x, mouse_y))
              	next_page = false;
            })
        })
    }

    convert.str("");
    convert << sun_reserved;
	sun_avalible = convert.str();
    win.show_text(sun_avalible, 0, 0);

//omadane safe shoro bazi
    bool initializing_screen = true;
    int x = 0;
    while(initializing_screen){
        win.draw_bg("./frontyard.png", x, 0);
        x += 5;
        if(x >= 240)
        {
            initializing_screen = false;
        }
    	win.update_screen();
    	DELAY(15);
    }

    start = clock();
    start2 = clock();

    int saved_level = read_save();
    if(saved_level == -1)
    	saved_level = 0;

//shoroe level ha
    for(int level=saved_level; true; level++){
    	if(winner){
			win.draw_bg("./black.png", 0, 0);
			win.show_text("You Won!", 375, 200);
			win.update_screen();
    		DELAY(1500);
    		winner = 0;
		}
		if(lose){
			win.draw_bg("./black.png", 0, 0);
			win.show_text("You Lost!", 375, 200);
			win.update_screen();
    		DELAY(1500);
    		lose = 0;
			level -= 2;
    		continue;
		}
    	peashooter_count=0;
		sun_flower_count=0;
		Wallnut_count=0;
    	for(int i = 0; i<5; i++)
    		for(int j=0; j<9; j++)
    			initializing_board(frontyard, i, j);
    	random_time.clear();
    	zombies.zombies_num.clear();
    	zombies.waves_length.clear();
    	finished = 0;
    	zombie_finished = 0;
    	bool end_game = read_level(zombies, level_num, level);
    	if(end_game)
    		break;

    	zombie.clear();
    	zombie_count.clear();
    	pea.clear();
    	peashooter.clear();
    	Wallnut.clear();
    	sun_flower.clear();
    	sun.clear();
    	sun_reserved = 50;
    	zombie.resize(zombies.waves);
    	zombie_count.resize(zombies.waves);
    	for(int i = 0; i<zombie_count.size(); i++)
    		zombie_count[i] = 0;

//shoro har wave    	
    	for(int wave=0; wave<zombies.waves; wave++){
    		start = clock();
    		convert2.str("");
    		convert2 << wave;
			wave_string = convert2.str();
    		win.show_text(wave_string, 375, 0);
    		if(winner || lose)
    			break;
    		random_time.clear();
    		random_time = zombies_time(wave, zombies);
	    	zombie[wave].resize(zombies.zombies_num[wave]);    		

//omadane har zombie va halghe asliye bazi    		
    		for(; zombie_count[wave]<zombies.zombies_num[wave];){
    			reload_flag=0;
    			//har duration bad if bara taine ine ke zame anjame on amal reside ya na
    			duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    			if(duration >= random_time[zombie_count[wave]] && finished==0){
    				zombie_attack(wave, zombies, win, random_time, zombie, zombie_count);
    				zombie_count[wave]++;
    			}
    			duration2 = ( clock() - start2 ) / (double) CLOCKS_PER_SEC;
    			if(duration2 >= 0.125*time_zombie){
    				zombie_move(win, zombie, frontyard, wave);
    				reload(frontyard, win, zombie, zombie_count, peashooter, sun_flower, Wallnut, sun, tick, 1, pointer , pea, sun_avalible, wave_string, wave);
    				win.update_screen();
    				time_zombie++;
    			}
    			if(duration2 >= 0.03125*time_pea){
    				pea_move(win, pea);
    				pea_hit(zombie, pea, zombie_count, wave);
    				reload(frontyard, win, zombie, zombie_count, peashooter, sun_flower, Wallnut, sun, tick, 1, pointer , pea, sun_avalible, wave_string, wave);
    				win.update_screen();
    				time_pea++;
    			}
    			if(duration2 >= 1*time_eat){
    				zombie_eat(win, frontyard, zombie, peashooter, sun_flower, Wallnut, zombie_count, wave, peashooter_count, sun_flower_count, Wallnut_count);
    				reload(frontyard, win, zombie, zombie_count, peashooter, sun_flower, Wallnut, sun, tick, 1, pointer , pea, sun_avalible, wave_string, wave);
    				win.update_screen();
    				time_eat++;
    			}
    			if(duration2 >= 1*time_sun){
    				int x = rand()%5;
    				if(x==4){
    					initializing_sun(sun, win);
    					draw_sun(win, sun, tick);
    				}
    				time_sun++;
    			}    			
    			for(int i=0; i<sun_flower.size(); i++){
    				sun_flower[i].duration = ( clock() - sun_flower[i].start ) / (double) CLOCKS_PER_SEC;
    			}
    			for(int i=0; i<sun_flower.size(); i++){
    				if(sun_flower[i].duration >= sun_flower[i].sun_counter*25){
    					sun_flower[i].sun_counter++;
    					initializing_sunforflower(frontyard, sun, sun_flower, i);
    					draw_sun(win, sun, tick);
    				}
    			}
    			for(int i=0; i<peashooter.size(); i++){
    				peashooter[i].duration = ( clock() - peashooter[i].start ) / (double) CLOCKS_PER_SEC;
    			}
    			//baresiye inke aya zobie ham radifash hast ya na ke shoro be shelick konad
    			for(int i=0; i<peashooter.size(); i++){
    				if(peashooter[i].duration >= peashooter[i].pea_counter*1.5){
    					peashooter[i].pea_counter++;
    					for(int wave=0; wave<zombie.size(); wave++){
    						for(int j=0; j<zombie_count[wave]; j++){
    							if(peashooter[i].row == zombie[wave][j].row && (zombie[wave][j].row_x - peashooter[i].row_x) > -30){
    								initializing_pea(frontyard, pea, peashooter, i);
    								draw_pea(win, pea);
    							}
    						}
    					}
    				}
    			}
    			winner = check_win(zombies, zombie, wave, level, total_zombiez_num, zombie_finished);
    			lose = check_lose(zombie, zombie_count, wave);
    			if(winner || lose)
    				break;

    			HANDLE({
    				KEY_PRESS(UP, {pointer_move(win, frontyard, pointer, 1, zombie, zombie_count, peashooter, sun_flower, Wallnut, sun, tick, pea, sun_avalible, wave_string, wave);})
    				KEY_PRESS(DOWN, {pointer_move(win, frontyard, pointer, 2, zombie, zombie_count, peashooter, sun_flower, Wallnut, sun, tick, pea, sun_avalible, wave_string, wave);})
    				KEY_PRESS(LEFT, {pointer_move(win, frontyard, pointer, 3, zombie, zombie_count, peashooter, sun_flower, Wallnut, sun, tick, pea, sun_avalible, wave_string, wave);})
    				KEY_PRESS(RIGHT, {pointer_move(win, frontyard, pointer, 4, zombie, zombie_count, peashooter, sun_flower, Wallnut, sun, tick, pea, sun_avalible, wave_string, wave);})
    				KEY_PRESS(p, {nokhod_plant(win, frontyard, pointer, peashooter, peashooter_count, sun_reserved, convert, sun_avalible);})
    				KEY_PRESS(s, {sun_flower_planting(win, frontyard, pointer, sun_flower, sun_flower_count, sun_reserved, convert, sun_avalible);})
    				KEY_PRESS(w, {Wallnut_plant(win, frontyard, pointer, Wallnut, Wallnut_count, sun_reserved, convert, sun_avalible);})
    	     		QUIT({return 0;})
  		      		KEY_PRESS(q, {return 0;})
  		      		LCLICK(
  		      			{check_suns(win, mouse_x, mouse_y, sun, sun_reserved, reload_flag, convert, sun_avalible);})
    	    	})
    			if(reload_flag==1){
    				reload(frontyard, win, zombie, zombie_count, peashooter, sun_flower, Wallnut, sun, tick, 0, pointer , pea, sun_avalible, wave_string, wave);
    				win.update_screen();
    			}
    			//baraye edame dashtane bazi dar akharin wave ta zombie ha bemirand
    			total_zombiez_num = 0;
    			for(int i=0; i<zombies.waves; i++){
    				total_zombiez_num += zombie_count[i];
    			}
    			if(wave == zombies.waves-1 && zombie_finished == 1 && total_zombiez_num == 0 && finished == 1){
    				zombies.zombies_num[wave]--;
    			}
    			if(zombie_finished == 0 && wave == zombies.waves-1 && zombie_count[wave] == zombies.zombies_num[wave]-1){
    				zombie_finished = 1;
    				zombies.zombies_num[wave]++;
    			}
    			if(wave == zombies.waves-1 && zombie_finished == 1 && total_zombiez_num != 0){
    				finished = 1;
    				continue;
    			}
    		}
		}
		save_level(level);
	}

    while(true){
    	    	HANDLE({
    	     		QUIT({return 0;})
  		      		KEY_PRESS(q, {return 0;})
    	    	})
        
        		win.update_screen();
        		DELAY(15);
    		}
	
}