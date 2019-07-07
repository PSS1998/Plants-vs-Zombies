#include "rsdl.hpp"
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

int main(){
	window win(800, 600);
	win.draw_bg("./menu.png", 0, 0);
	while(true){
		HANDLE({
			QUIT({return 0;})
			KEY_PRESS(q, {return 0;})
			LCLICK({
				cout << "(" << mouse_x << "," << mouse_y << ")" << endl;
			})
		})
	win.update_screen();
	DELAY(15);
	}
}
