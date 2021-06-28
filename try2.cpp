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

    {100000, -200, 100, 50, 50, 100, -200, 100000},
    {-200, -5000,  10,  3,  3,  10, -5000, -200},
    { 100,  -10, 150, 10, 10, 150,  -10,  100},
    {  50,    3,  10,  3,  3,  10,    3,   50},
    {  50,    3,  10,  3,  3,  10,    3,   50},
    { 100,  -10, 150, 10, 10, 150,  -10,  100},
    {-200, -5000,  10,  3,  3,  10, -5000, -200},
    {100000, -200, 100, 15, 15, 100, -200, 100000},

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
};

bool on_board(Point p) {
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
}

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

int minimax(HYPOthelloBoard node, int depth, int alpha, int beta, int maximizingplayer){
    if (depth==0 || node.next_valid_spots.size()==0) return statevalue(node);

    if (maximizingplayer==player){
        int maxv = INT_MIN;
        for (auto child: node.next_valid_spots){ //vector
            HYPOthelloBoard nextnode = node;

            //nextnode.update(child.x, child.y);
            //if (nextnode.get_H() > maxv)
            int v = minimax(nextnode , depth-1, alpha, beta, 3-maximizingplayer);
            maxv = max(maxv, v);
            alpha = max(alpha, v);
            if (depth==MaxD) vmap[v] = child; //vmap.insert(pair<int, Point> (v, child));
            if (alpha>=beta) break;
        }
        node.H = maxv;
        return maxv;
    }
    else if(maximizingplayer== (3-player)) {
        int minv = INT_MAX;
        for (auto child: node.next_valid_spots){
            HYPOthelloBoard nextnode = node;

            int v = minimax(nextnode , depth-1, alpha, beta, 3-maximizingplayer);
            minv = min(minv, v);
            beta = min(beta, v);
            if (depth==MaxD) vmap[v] = child; //vmap.insert(pair<int, Point> (v, child))
            if (alpha>=beta) break;
        }
        node.H = minv;
        return minv;
    }
    return 0;
}

void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    // Choose random spot. (Not random uniform here)

    HYPOthelloBoard ob(board);
    ob.cur_player = player;
    ob.next_valid_spots = next_valid_spots;

    int v = INT_MIN;
    Point p;
    for (int i = 0; i< n_valid_spots; i++){
        ob.board[next_valid_spots[i].x][next_valid_spots[i].y] = player;
        if (statevalue(ob)>v) v = statevalue(ob), p=next_valid_spots[i];
        ob.board[next_valid_spots[i].x][next_valid_spots[i].y] = 0;
        fout << p.x << " " << p.y << std::endl;
        fout.flush();
    }

/*    int vv = INT_MIN;
    Point pp;
    for (int i = 0; i<n_valid_spots; i++){
        if (weight[next_valid_spots[i].x][next_valid_spots[i].y] > vv)
        {
            vv = weight[next_valid_spots[i].x][next_valid_spots[i].y];
            pp = next_valid_spots[i];
        }
        cout << pp.x << " " << pp.y << std::endl;
        fout << pp.x << " " << pp.y << std::endl;
        fout.flush();
    }


    int v = minimax(ob, 1, INT_MIN, INT_MAX, ob.cur_player);
    Point choose = vmap[v];
    //vmap.clear();
    fout << choose.x << " " << choose.y << std::endl;
    fout.flush();

    v = minimax(ob, MaxD, INT_MIN, INT_MAX, ob.cur_player);
    choose = vmap[v];
    //vmap.clear();
    fout << choose.x << " " << choose.y << std::endl;
    fout.flush();
*/
    //cout<< choose.x << " " << choose.y << std::endl;
    /*for (int i = 1; i<=MaxD; i+=2){

        int v = minimax(ob, i, INT_MAX, INT_MIN, ob.player);
        for (int i = 0; i<next_valid_spots.size(); i++){
            if (next_valid_spots[i].point_value==v) p = next_valid_spots[i];
        }
        fout << p.x << " " << p.y << std::endl;
        fout.flush();
    }*/
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
