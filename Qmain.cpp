#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <list>
#include <tuple>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <iomanip>




// RIGHT = DOWN
// LEFT = UP
// UP = LEFT
// DOWN = RIGHT



using namespace std;

double getMax(double arr[4]);
int getMaxIndex(double* q_board);
int stepsTaken = 0;

class Snake
{
private:
    int snake_x, snake_y, snake_length, prevInput, board_length, board_height;
    int apple_x, apple_y;
    int points;
    int board[12][12];
    int h_board[12][12];
    char path_board[12][12];
	double q_board [12][12][4];
    bool up, left, down, right, exit, endProgram, eaten;
    vector<char> path;


public:
    Snake()
    {
        up = false;
        left = false;
        down = false;
        right = false;
        exit = false;
        endProgram = false;
        eaten = false;
        prevInput = 2;

        board_height = 12;
        board_length = 12;

        snake_x = 1;
        snake_y = 1;

        apple_x = 5;
        apple_y = 5;

        points = 0;

        snake_length = 3;

        path = {};

        for (int i = 0; i < board_height; i++)
        {
            for (int j = 0; j < board_length; j++)
            {
                if(i == 0 || i == 11)
                {
                    board[i][j] = -1;
                }
                else if(j == 0 || j == 11)
                {
                    board[i][j] = -1;
                }
                else
                {
                    board[i][j] = 0;
                }
            }
        }

        // test snake body
        // board[3][3] = 1;
        // board[3][4] = 2;
        // board[3][5] = 3;
        // board[3][6] = 4;



        for (int i = 0; i < board_height; i++)
        {
            for (int j = 0; j < board_length; j++)
            {
                path_board[i][j] = 'x';
            }
        }

		for (int i = 0; i < board_height; i++)
		{
			for (int j = 0; j < board_length; j++)
            {
                for (int k = 0; k < 4; k++){
					q_board[i][j][k] = 0;
				}
            }
		}

        board[snake_x][snake_y] = snake_length;
        board[apple_x][apple_y] = -10;


    }
    ~Snake()
    {

    }

    void displayBoard();
    void play();
    void getInput();
    void updatePlayer();
    void updateBoard();
    void calc_h_board(); // A*
    void displayHBoard(); // A*
    void displayPathBoard(); // A*
    void calcAStar(); // A*
    bool checkChild(tuple<int, int> child, tuple<int, int> lowest_tup, list<tuple<int, int> > &open_list, list<tuple<int, int> > &close_list, char dir); // A*
    void getPathBoard(); 
    void resetPath(); // A*
    void displayPath(); // A*
    void resetPathBoard(); // A*
	bool existsInList(tuple<int, int> x, list<tuple<int, int> > myList); // A*

    double reward(tuple<int, int> coord, int action); // QL
    void calc_q_board(int episodes); // QL
    void display_q_board(); // QL

    int getPoints();


};

tuple<int, int> getTuple(list<tuple<int, int> >, int);

int main(){
    srand(time(NULL));

    // instructions
    //cout << "To play use 'wasd' to move." << endl;
    //cout << "To exit use 'x'." << endl;
    //cout << "Press enter to start." << endl;
    Snake my_snake;
    my_snake.play();

    cout << "Final Score: " << my_snake.getPoints() << endl;
    cout << "Final Steps Taken " << stepsTaken << endl;
    
    return 0;
}

void Snake::displayBoard()
{
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            if (board[i][j] == -1){
                cout << 'X';
            } else if(board[i][j] == snake_length){
                cout << 'e';
            } else if(board[i][j] > 0){
                cout << 'o';
            } else if (board[i][j] == 0){
                cout << '_';
            } else if(board[i][j] == -10){
                cout << 'a';
            } else cout << '_';
        }
        cout << endl;
    }
}

void Snake::displayPathBoard()
{
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            cout << path_board[i][j];
            cout << "\t";
        }
        cout << endl;
    }
}

void Snake::play()
{
    while(!endProgram)
    {
        exit = false;
        resetPathBoard();
        calc_q_board(1000000);
        //display_q_board();
        getPathBoard();
        //displayPathBoard();
        //return;
        while(exit != true)
        {
            stepsTaken++;
			getInput();

            updatePlayer();
            updateBoard();
            cout << "Steps taken: " << stepsTaken << endl;
			cout << "Points scored: " << points << endl;
            displayBoard();
            
            // sleep and clear the input after each display
            // avoids multiple moves in one turn
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            fseek(stdin,0,SEEK_END);
            
            //exit = true;
        }
    }

}

void Snake::getInput()
{
    up = false;
    left = false;
    down = false;
    right = false;
    exit = false;

//    // Set terminal to raw mode
//    system("stty raw");
//    // Wait for single character
//    input = getchar();
//    // Reset terminal to normal "cooked" mode
//    system("stty cooked");
    
    // if(path.size() == 0)
    // {
    //     exit = true;
    //     return;
    // }

    char input = path_board[snake_x][snake_y];

    if (input == 'w')
    {
        up = true;
    }
    else if (input == 'a')
    {
        left = true;
    }
    else if (input == 's')
    {
        down = true;
    }
    else if (input == 'd')
    {
        right = true;
    }
    else if (input == 'x')
    {
        exit = true;
    }
    else {
        switch(prevInput){
            case 1:
                up = true;
                break;
            case 2:
                right = true;
                break;
            case 3:
                down = true;
                break;
            case 4:
                left = true;
                break;
        }
    }
    
    //path.erase(path.begin());
}

void Snake::updatePlayer()
{
    if (up == true)
    {
        // move the snake
        snake_x -= 1;
        // make sure we're not going out of bounds
        if(snake_x == 0){
            snake_x += 1;
        }
    }
    else if (left == true)
    {
        snake_y -= 1;
        if(snake_y == 0){
            snake_y += 1;
        }
    }
    else if (down == true)
    {
        snake_x += 1;
        if(snake_x == 11){
            snake_x -= 1;
        }
    }
    else if (right == true)
    {
        snake_y += 1;
        if(snake_y == 11){
            snake_y -= 1;
        }
    }

    // record if we are eating an apple
    if(snake_x == apple_x && snake_y == apple_y){
        eaten = true;
        points++;
    }
}

void Snake::updateBoard()
{
    for (int i = 1; i < board_height-1; i++)
    {
        for (int j = 1; j < board_length-1; j++)
        {
            if(board[i][j] > 0)
            {
                board[i][j]--;
            }
        }
    }
    if(up){
        prevInput = 1;
    } else if(right){
        prevInput = 2;
    } else if(down){
        prevInput = 3;
    } else if(left){
        prevInput = 4;
    }

    if(eaten)
    {
        snake_length++;
        eaten = false;
		exit = true;
        // increment all snake body
        for (int i = 1; i < board_height-1; i++)
        {
            for (int j = 1; j < board_length-1; j++)
            {
                if(board[i][j] > 0)
                {
                    board[i][j]++;
                }
            }
        }

        //generate a new apple
        int randX;
        int randY;
        bool collision = true;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(1, 10);

        // make sure the apple appears on an empty spot
        do{
            randX = distr(gen);
            randY = distr(gen);
            if(board[randX][randY] == 0){
                apple_x = randX;
                apple_y = randY;
                board[apple_x][apple_y] = -10;
                collision = false;
            }
        }while(collision);
    }
    if(board[snake_x][snake_y] != 0 && board[snake_x][snake_y] != -10){
        exit = true;
        endProgram = true;
    }
    board[snake_x][snake_y] = snake_length;
}

void Snake::calc_h_board()
{
    for (int i = 0; i < board_height; i++)
    {
        for (int j = 0; j < board_length; j++)
        {
            if(i == 0 || i == 11 || j == 0 || j == 11)
            {
                h_board[i][j] = 999;
            }
            else
            {
                int x_dif = 0;
                int y_dif = 0;

                int man_dist = 0;

                x_dif = apple_x - i;
				y_dif = apple_y - j;

				if (x_dif < 0)
					x_dif = -x_dif;

				if (y_dif < 0)
					y_dif = -y_dif;

				h_board[i][j] = x_dif + y_dif;
            }
        }
    }
}

void Snake::displayHBoard()
{
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            cout << h_board[i][j];
            cout << "\t";
        }
        cout << endl;
    }
}

void Snake::calcAStar()
{
    list<tuple<int, int> > open_list = {};
    list<tuple<int, int> > close_list = {};

    tuple<int, int> temp = make_tuple (snake_x, snake_y);

    open_list.push_back(temp);

    // int lowest_val = h_board[get<0>(getTuple(open_list, 0))][get<1>(getTuple(open_list, 0))];
    // tuple<int, int> lowest_tup = getTuple(open_list, 0);

    while (open_list.size() != 0)
    {
		int lowest_val = h_board[get<0>(getTuple(open_list, 0))][get<1>(getTuple(open_list, 0))];
    	tuple<int, int> lowest_tup = getTuple(open_list, 0);

		// get the lowest value state on the list
        for (int i = 0; i < open_list.size(); i++)
        {
            if(lowest_val >= h_board[get<0>(getTuple(open_list, i))][get<1>(getTuple(open_list, i))])
            {
                lowest_val = h_board[get<0>(getTuple(open_list, i))][get<1>(getTuple(open_list, i))];
                lowest_tup = getTuple(open_list, i);
            }
        }
		// remove the state from the open list
        //open_list.remove(lowest_tup);

		// auto unwanted = [&lowest_tup](const tuple<int,int>& t) {return get<0>(t)==get<0>(lowest_tup) or get<1>(t)==get<1>(lowest_tup);};
		// open_list.remove_if(unwanted);

		// std::erase_if(open_list, [&lowest_tup](const tuple<int,int>& elem) {
		// return get<0>(elem) == get<0>(lowest_tup) || get<1>(elem) == get<1>(lowest_tup); });

		// cout << "TESTING REMOVE: removing " << get<0>(lowest_tup) << ", " << get<1>(lowest_tup) << endl;

		// cout << "FULL OPEN LIST BEFORE: " << endl;
		// for(int i = 0; i < open_list.size(); i++){
		// 	cout << "\t" << get<0>(getTuple(open_list, i)) << ", " << get<1>(getTuple(open_list, i));
		// } 
		// cout << endl;


		open_list.remove_if([&lowest_tup](tuple<int, int> elem){ 
			return get<0>(elem) == get<0>(lowest_tup) && get<1>(elem) == get<1>(lowest_tup); });

		// cout << "FULL OPEN LIST AFTER: " << endl;
		// for(int i = 0; i < open_list.size(); i++){
		// 	cout << "\t" << get<0>(getTuple(open_list, i)) << ", " << get<1>(getTuple(open_list, i));
		// }
		// cout << endl;




		//cout << get<0>(lowest_tup) << ", " << get<1>(lowest_tup) << endl;





		// check all of current states children
        // tuple<int, int> down_child = make_tuple (get<0>(lowest_tup)+1, get<1>(lowest_tup));
        // if(checkChild(down_child, lowest_tup, open_list, close_list, 's'))
        //     break;
        // tuple<int, int> up_child = make_tuple (get<0>(lowest_tup)-1, get<1>(lowest_tup));
        // if(checkChild(up_child, lowest_tup, open_list, close_list, 'w'))
        //     break;
        // tuple<int, int> right_child = make_tuple (get<0>(lowest_tup), get<1>(lowest_tup)+1);
        // if(checkChild(right_child, lowest_tup, open_list, close_list, 'd'))
        //     break;
        // tuple<int, int> left_child = make_tuple (get<0>(lowest_tup), get<1>(lowest_tup)-1);
        // if(checkChild(left_child, lowest_tup, open_list, close_list, 'a'))
        //     break;

		tuple<int, int> down_child = make_tuple (get<0>(lowest_tup)+1, get<1>(lowest_tup));
        checkChild(down_child, lowest_tup, open_list, close_list, 's');
        tuple<int, int> up_child = make_tuple (get<0>(lowest_tup)-1, get<1>(lowest_tup));
        checkChild(up_child, lowest_tup, open_list, close_list, 'w');
        tuple<int, int> right_child = make_tuple (get<0>(lowest_tup), get<1>(lowest_tup)+1);
        checkChild(right_child, lowest_tup, open_list, close_list, 'd');
        tuple<int, int> left_child = make_tuple (get<0>(lowest_tup), get<1>(lowest_tup)-1);
        checkChild(left_child, lowest_tup, open_list, close_list, 'a');
		// add current state to closed list
        close_list.push_back(lowest_tup);
		if(board[get<0>(lowest_tup)][get<1>(lowest_tup)] == -10){
			break;
		}
			
    }

    // for (auto v : open_list)
    //     std::cout << get<0>(v) << " "<< get<1>(v) << "\n";

    // for (auto v : close_list)
    //     std::cout << get<0>(v) << " "<< get<1>(v) << "\n";
}

bool Snake::checkChild(tuple<int, int> child, tuple<int, int> lowest_tup, list<tuple<int, int> > &open_list, list<tuple<int, int> > &close_list, char dir)
{
    // bool found_closed = (find(close_list.begin(), close_list.end(), child) != close_list.end());
    // bool found_open = (find(open_list.begin(), open_list.end(), child) != open_list.end());

	bool found_closed = existsInList(child, close_list);
	bool found_open = existsInList(child, open_list);

    int test_child = board[get<0>(child)][get<1>(child)];

    if(test_child != 0 && test_child != -10)
    {
        return false;
    }
    // else if(test_child == -10)
    // {
	// 	open_list.push_back(child);
    //     path_board[get<0>(child)][get<1>(child)] = dir;
    //     return true;
    // }

    if(!found_open && !found_closed && (test_child == 0 || test_child == -10))
    {
        open_list.push_back(child);
        path_board[get<0>(child)][get<1>(child)] = dir;
        
		if(test_child == -10)
		{
			return true;
		}
        return false;
    }
    
    return false;

}

bool Snake::existsInList(tuple<int, int> x, list<tuple<int, int> > myList){

	for(int i = 0; i < myList.size(); i++){
		if(get<0>(x) == get<0>(getTuple(myList,i)) && get<1>(x) == get<1>(getTuple(myList, i))){
			return true;
		}
	} return false;
}


void Snake::getPathBoard()
{
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 12; j++){
            char move;
            int dir = 0;
            dir = getMaxIndex(q_board[i][j]);
            switch(dir){
                case 0:
                    move = 'w';
                    break;
                case 1:
                    move = 'd';
                    break;
                case 2:
                    move = 's';
                    break;
                case 3:
                    move = 'a';
                    break;
                default:
                    move = 'd';
                    break;
            }
            path_board[i][j] = move;
        }
    }
}

void Snake::displayPath()
{
    for(int i = 0; i < path.size(); i++)
    {
        cout << path[i];
    }
}

void Snake::resetPath()
{
    path.clear();
}

void Snake::resetPathBoard()
{
    for(int i = 0; i < 12; i++)
    {
        for(int j = 0; j < 12; j++)
        {
            path_board[i][j] = 'x';
        }
    }
}

tuple<int, int> getTuple(list<tuple<int, int> > _list, int _i)
{
    list<tuple<int, int> >::iterator it = _list.begin();
    for(int i=0; i<_i; i++){
        ++it;
    }
    return *it;
}

void Snake::calc_q_board(int episodes){







    
    double gamma = 0.9;
    double lRate = 0.1;







    for(int i =0; i < episodes; i++){
        int randX2 = rand()%12;
        int randY2 = rand()%12;
        int randAction = rand()%4;

        int myX = randX2;
        int myY = randY2;
        switch(randAction){
            case 0: // up
                myX -= 1;
                break;
            case 1: // right
                myY += 1;
                break;
            case 2: // down
                myX += 1;
                break;
            case 3: // left
                myY -= 1;
                break;
            default:
                break;
        }

        if(myX == -1 || myX == 12 || myY == -1 || myY == 12){
            q_board[randX2][randY2][randAction] = -999;
            //display_q_board();
            continue;
        }

        double currValue = q_board[randX2][randY2][randAction];
        //double val1maxQ = (gamma * *max_element(q_board[myX][myY] , q_board[myX][myY] + 3));
        double val1maxQ = (gamma * getMax(q_board[myX][myY]));
        double val1Reward = reward(make_tuple (myX, myY), randAction);
        q_board[randX2][randY2][randAction] = (currValue) + lRate * (val1Reward + val1maxQ - currValue);


        // double val1Reward = reward(make_tuple (myX, myY), randAction);
        // double val1maxQ = (gamma * *max_element(q_board[myX][myY] , q_board[myX][myY] + 3));
        // double val1 = val1Reward + val1maxQ;


        //q_board[randX2][randY2][randAction] = val1;

        //display_q_board();
    }
}

double Snake::reward(tuple<int, int> coord, int action)
{
    int myVal = board[get<0>(coord)][get<1>(coord)];

    switch ( myVal ){
        case -10:
            return 1;
            break;
        case -1:
            return -10;
            break;
        case 0:
            return -1;
            break;
        default:
            return -10;
            break;
    }
    return -10;
}

void Snake::display_q_board(){

    char dummy;
    //cin >> dummy;
    cout << endl;
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            cout << "[";
            for (int k = 0; k < 4; k++){
                if (!(q_board[i][j][k] > 9)){
                    cout << setprecision(1) << std::llround(q_board[i][j][k]);
                    if(k < 3) cout << ", ";
                } else {
                    cout << "x";
                    if(k < 3) cout << ", ";
                }
            }
            cout << "] ";
        }
        cout << endl;
    }
}

int Snake::getPoints(){
    return points;
}

double getMax(double arr[4]){
    double myMax = arr[0];
    for(int i = 0; i < 4; i++){
        if(arr[i] > myMax){
            myMax = arr[i];
        }
    }
    return myMax;
}

int getMaxIndex(double* q_board){
    int myIndex = 0;
    for(int i = 0; i < 4; i++){
        if(q_board[i] > q_board[myIndex]){
            myIndex = i;
        }
    }
    return myIndex;
}