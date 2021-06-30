#include <climits>
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>
using namespace std;
struct Point {
    int x, y;
    Point() : Point(0, 0) {}

	Point(float x, float y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};
std::array<array<int,8>, 8> weight{
    {

    {30000,  -200, 100, 50, 50, 100,  -200,30000},
    {-200 , -3000, -10,  3,  3, -10, -3000, -200},
    { 100 ,   -10, 300, 10, 10, 300,   -10,  100},
    {  50 ,     3,  10,  3,  3,  10,     3,   50},
    {  50 ,     3,  10,  3,  3,  10,     3,   50},
    { 100 ,   -10, 300, 10, 10, 300,   -10,  100},
    {-200 , -3000, -10,  3,  3, -10, -3000, -200},
    {30000,  -200, 100, 50, 50, 100,  -200,30000},

 /*
    {1, 2, 3, 4, 5, 6, 7, 8 },
    {11, 12, 13, 14, 15, 16, 17, 18 },
    {21, 22, 23, 24, 25, 26, 27, 28 },
    {31, 32, 33, 34, 35, 36, 37, 38 },
    {41, 42, 43, 44, 45, 46, 47, 48 },
    {51, 52, 53, 54, 55, 56, 57, 58 },
    {61, 62, 63, 64, 65, 66, 67, 68 },
    {71, 72, 73, 74, 75, 76, 77, 78 },*/
    }
};

std::array<Point, 4> corner{
    {
        Point(0,0), Point(0,7), Point(7,0), Point(7,7)
    }
};

std::array<Point, 8> beside_c{
    { // 左右, 上下
        Point(0,1), Point(1, 0), Point(0,6), Point(1,7), Point(7,1), Point(6,0), Point(7,6), Point(6,7)
    }
};

std::array<Point, 4> x_c{
    {
        Point(1,1), Point(1, 6), Point(6, 1), Point(6,6)
    }
};


int player;
const int SIZE = 8;
int MaxD = 3;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;
std::map<int, Point> vmap;

class HYPOthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
    int H;

    HYPOthelloBoard(std::array<std::array<int, SIZE>, SIZE> b): cur_player(player),done(false), winner(-1), H(0) {
        for(int i = 0; i < SIZE; i++)
            for(int j = 0; j < SIZE; j++)
                board[i][j] = b[i][j];
    }

private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    HYPOthelloBoard() {
        reset();
    }
    void reset() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        board[3][4] = board[4][3] = BLACK;
        board[3][3] = board[4][4] = WHITE;
        cur_player = BLACK;
        disc_count[EMPTY] = 8*8-4;
        disc_count[BLACK] = 2;
        disc_count[WHITE] = 2;
        next_valid_spots = get_valid_spots();
        done = false;
        winner = -1;

    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }
        return true;
    }
    void update(Point np){
        set_disc(np, cur_player);
        flip_discs(np);
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        if (next_valid_spots.size()==0){
            std::vector<Point>tmp_valid_spots = next_valid_spots;
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size()==0){
                done = true;
            }
            else{
                cur_player = get_next_player(cur_player);
                next_valid_spots = tmp_valid_spots;
            }
        }

    }

    int empty_disc(){
        return disc_count[EMPTY];
    }

    int player_disc(){
        return disc_count[player];
    }

    int oppo_disc(){
        return disc_count[3-player];
    }
};

int statevalue(HYPOthelloBoard cur){
    int v = 0;
    for (int i = 0; i<8; i++){
        for (int j = 0; j<8; j++){
            if (cur.board[i][j]==player) v+=weight[i][j];
            else if (cur.board[i][j]==(3-player)) v-=weight[i][j];
        }
    }
    return v;
}


int edge(HYPOthelloBoard cur){
    int v = 0;


    for (int i = 0; i<4; i++){
        Point c = corner[i];
        int mul = (cur.board[c.x][c.y]==player)?1:-1;
        mul*=1;
        /*
         if (cur.board[c.x][c.y] == player){
            if (board[beside_c[2*i].x][beside_c[2*i].y]== 0 ) weight[beside_c[2*i].x][beside_c[2*i].y]+=1500;
            if (board[beside_c[2*i+1].x][beside_c[2*i+1].y]== 0) weight[beside_c[2*i+1].x][beside_c[2*i+1].y]+=1500;
        }

        if (c.x==0 || c.x==SIZE-1 ){
            for (int i = 1; i<SIZE - 1; i++){
                if (board[c.x][c.y]==board[c.x][i]) v+= mul * 500;
            }
        }

        if (c.y==0 || c.y==SIZE-1){
            for (int i= 1; i<SIZE-1; i++){
                if (board[c.x][c.y]==board[i][c.y]) v+=mul*500;
            }
        }
*/
    }

    return v;
}

int disc_c(HYPOthelloBoard cur){
    int mul = (cur.cur_player==player)?1:-1;

    return mul * 200* (cur.player_disc() - cur.oppo_disc()) / (cur.player_disc() + cur.oppo_disc());
}

int mobility(HYPOthelloBoard cur){
    int mul = (cur.cur_player==player)?1:-1;
    return (mul * 20* cur.next_valid_spots.size());
}

int corner_stability(HYPOthelloBoard cur){
    int mul = (cur.cur_player==player)?1:-1;
    int v = 0;
    int cnt = 0;
    for (int i = 0; i<4; i++){
        Point c = corner[i];
        if (cur.board[c.x][c.y]!=0){ // if corner not empty, weight is positive
            if (board[c.x][c.y]==player){
                weight[beside_c[2*i].x][beside_c[2*i].y] = 300;
                weight[beside_c[2*i+1].x][beside_c[2*i+1].y] = 300;
                weight[x_c[i].x][x_c[i].y] = 400;
            }
            continue;
        }
        if (cur.board[c.x][c.y]==player) v+=2, cnt++;
        else if (cur.board[c.x][c.y]==(3-player)) v-=2;
    }
    if (cnt==4) v+=20;
    return mul * 1000 * v;

}

int total_value(HYPOthelloBoard cur){
    int v = 0;

    if (cur.empty_disc()<15){
        v = v + statevalue(cur) + edge(cur) + disc_c(cur)*5 + mobility(cur)*5 + corner_stability(cur)*10;
    }
    else if (cur.empty_disc()<30){
        v = v + statevalue(cur)*3+ edge(cur)*3 + disc_c(cur)*2 + mobility(cur)*2 + corner_stability(cur)*10;
    }
    else if (cur.empty_disc()<50){
        v = v + statevalue(cur)* 7+ edge(cur)*4+ disc_c(cur)+ mobility(cur) + corner_stability(cur)*10;
    }
    else{
        v = v +statevalue(cur)*20+ edge(cur)*5 + corner_stability(cur)*100;
    }
    return v;
}

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    float x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

std::ofstream mfout("out.txt", std::ios_base::app);
int minimax(HYPOthelloBoard node, int depth, int alpha, int beta){
    if (depth==0 || node.next_valid_spots.size()==0){
        return total_value(node);
    }
    if (node.cur_player==player){
        mfout<<"New Node:"<<endl;
        int maxv = INT_MIN;
        for (auto child: node.next_valid_spots){ //vector
            HYPOthelloBoard nextnode = node;
            nextnode.update(child);

            mfout<<"player\n";
            mfout << "+---------------+\n";
            for (int i = 0; i < SIZE; i++) {
                mfout << "|";
                for (int j = 0; j < SIZE-1; j++) {
                    mfout << nextnode.board[i][j]<< " ";
                }
                mfout << nextnode.board[i][SIZE-1]<< "|\n";
            }
            mfout << "+---------------+\n";

            int v = minimax(nextnode , depth-1, alpha, beta);
            mfout<<child.x<<" " << child.y<<":"<<v<<endl;
            maxv = max(maxv, v);
            alpha = max(alpha, v);
            mfout << "maxv:"<<maxv << endl;
            if (depth==MaxD) vmap[v] = child; //vmap.insert(pair<int, Point> (v, child));
            if (alpha>=beta) break;
        }
        mfout<<"MAXV:"<<maxv<<endl;
        node.H = maxv;
        return maxv;
    }
    else if(node.cur_player== (3-player)) {
        int minv = INT_MAX;
        for (auto child: node.next_valid_spots){
            HYPOthelloBoard nextnode = node;
            nextnode.update(child);
            mfout << "oppo\n";
            mfout << "+---------------+\n";
            for (int i = 0; i < SIZE; i++) {
                mfout << "|";
                for (int j = 0; j < SIZE-1; j++) {
                    mfout << nextnode.board[i][j]<< " ";
                }
                mfout << nextnode.board[i][SIZE-1]<< "|\n";
            }
            mfout << "+---------------+\n";

            int v = minimax(nextnode , depth-1, alpha, beta);
            mfout<<child.x<<" " << child.y<<":"<<v<<endl;
            minv = min(minv, v);
            beta = min(beta, v);
            if (depth==MaxD) vmap[v] = child; //vmap.insert(pair<int, Point> (v, child))
            if (alpha>=beta) break;
        }
        mfout<<"MINV:"<<minv<<endl;
        node.H = minv;
        return minv;
    }
    return 0;
}

void write_valid_spot(std::ofstream& fout) {
    //int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    // Choose random spot. (Not random uniform here)

    HYPOthelloBoard ob(board);
    ob.cur_player = player;
    ob.next_valid_spots = next_valid_spots;

    /*int v = INT_MIN;
    Point p;
    for (int i = 0; i< n_valid_spots; i++){
        ob.board[next_valid_spots[i].x][next_valid_spots[i].y] = player;
        if (statevalue(ob)>v) v = statevalue(ob), p=next_valid_spots[i];
        ob.board[next_valid_spots[i].x][next_valid_spots[i].y] = 0;
        fout << p.x << " " << p.y << std::endl;
        fout.flush();
    }*/
    int v ;
    Point choose;

    /*
    MaxD = 1;
    v = minimax(ob, 1, INT_MIN, INT_MAX);
    choose = vmap[v];
    vmap.clear();
    mfout<<"choose: "<< choose.x << " " << choose.y <<endl;
    fout << choose.x << " " << choose.y << std::endl;
    fout.flush();*/

    MaxD = 2;
    v = minimax(ob, 2, INT_MIN, INT_MAX);
    choose = vmap[v];
    vmap.clear();
    mfout<<"2CHOOSE: "<< choose.x << " " << choose.y <<endl;
    fout << choose.x << " " << choose.y << std::endl;
    fout.flush();


    MaxD = 5;
    v = minimax(ob, 5, INT_MIN, INT_MAX);
    choose = vmap[v];
    vmap.clear();
    mfout<<"5CHOOSE: "<< choose.x << " " << choose.y <<endl;
    fout << choose.x << " " << choose.y << std::endl;
    fout.flush();

    // Remember to flush the output to ensure the last action is written to file.
    //fout << p.x << " " << p.y << std::endl;
    //fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
//.\main.exe .\try2.exe ..\baseline\windows\baseline2.exe

