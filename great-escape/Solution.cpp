#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;


enum LOGLEVEL { TRACE, DEBUG, INFO, WARN, ERROR };
const LOGLEVEL currentLogLevel = DEBUG;

bool isLogLevelActive(LOGLEVEL const& logLevel)
{
    return logLevel >= currentLogLevel;
}

void log(string const& str, LOGLEVEL const& logLevel)
{
    if(isLogLevelActive(logLevel))
    {
        cerr << str << endl;
    }
}

void log(string const& str)
{
    log(str, DEBUG);
}




// ---------------- CONSTANTS ---------------- //
const int boardSize(9);
const int nbWallsPerPlayer(10);


// ------------------ CLASSES ---------------- //
enum MOVE { UP, DOWN, LEFT, RIGHT};

class Box
{
private:
    Box *_left, *_top, *_bottom, *_right;

public:
    Box()
        : _left(NULL), _top(NULL), _bottom(NULL), _right(NULL)
    {}

    //GETTERS
    Box* getLeft() const
    {
        return this->_left;
    }
    Box* getTop() const
    {
        return this->_top;
    }
    Box* getBottom() const
    {
        return this->_bottom;
    }
    Box* getRight() const
    {
        return this->_right;
    }

    bool isAccessible(MOVE const& move) const
    {
        bool result = false;

        switch(move)
        {
        case UP :
            result = getTop();
            break;
        case DOWN:
            result = getBottom();
            break;
        case LEFT:
            result = getLeft();
            break;
        case RIGHT:
            result = getRight();
            break;
        }
        return result;
    }

    //SETTERS
    void setLeft(Box *left)
    {
        this->_left = left;
    }
    void setTop(Box *top)
    {
        this->_top = top;
    }
    void setBottom(Box *bottom)
    {
        this->_bottom = bottom;
    }
    void setRight(Box *right)
    {
        this->_right = right;
    }
};



class BoardObject
{
private:
    int _x;
    int _y;

public:
    BoardObject(int x, int y): _x(x), _y(y) {}
    ~BoardObject() {}

    int getX() const
    {
        return this->_x;
    }
    int getY() const
    {
        return this->_y;
    }
    void setX(int x)
    {
        this->_x = x;
    }
    void setY(int y)
    {
        this->_y = y;
    }
};



class Player : public BoardObject
{
private:
    int _board[boardSize][boardSize];
    int _id;
    int _wallsLeft;

public:
    Player(int id, int myId)
        : BoardObject(0, 0), _id(id), _wallsLeft(nbWallsPerPlayer)
    {
        buildBoard();
    }

    ~Player() {}


    string getBestAction(Box const gameBoard[boardSize][boardSize]) const
    {
        log("Player.getBestAction");
        cerr << "( id= " << _id << "| x=" << getX() << " | y=" << getY() << ")" <<endl;
        Box currentBox = gameBoard[this->getX()][this->getY()];
        multimap<int,string> accessibleBox;

        if(currentBox.isAccessible(RIGHT))
        {
            log(" |- RIGHT Accessible");
            accessibleBox.insert(pair<int,string>(_board[this->getX()+1][this->getY()],"RIGHT"));
        }
        if(currentBox.isAccessible(LEFT))
        {
            log(" |- LEFT Accessible");
            accessibleBox.insert(pair<int,string>(_board[this->getX()-1][this->getY()],"LEFT"));
        }
        if(currentBox.isAccessible(DOWN))
        {
            log(" |- DOWN Accessible");
            accessibleBox.insert(pair<int,string>(_board[this->getX()][this->getY()+1],"DOWN"));
        }
        if(currentBox.isAccessible(UP))
        {
            log(" |- UP Accessible");
            accessibleBox.insert(pair<int,string>(_board[this->getX()][this->getY()-1],"UP"));
        }

        return accessibleBox.begin()->second;
    }

    int getId() const
    {
        return _id;
    }

    void update(int x, int y, int wallsLeft)
    {
        this->setX(x);
        this->setY(y);
        this->setWallsLeft(wallsLeft);
    }

    int getWallsLeft() const
    {
        return this->_wallsLeft;
    }

    void setWallsLeft(int wallsLeft)
    {
        this->_wallsLeft = wallsLeft;
    }

    void getBoard(ostream &oss) const
    {

        for(int y(0); y<boardSize; y++)
        {
            for(int x(0); x<boardSize; x++)
            {
                oss << _board[x][y] << " | ";
            }
            oss << endl;
        }
    }

private:
    void buildBoard()
    {
        int distance(0);
        switch(this->_id)
        {
        case 0:
            for(int x(boardSize-1); x>=0; x--)
            {
                for(int y(0); y<boardSize; y++)
                {
                    _board[x][y]=distance;
                }
                distance++;
            }
            break;
        case 1:
            for(int x(0); x<boardSize; x++)
            {
                for(int y(0); y<boardSize; y++)
                {
                    _board[x][y]=distance;
                }
                distance++;
            }
            break;
        case 2:
            for(int y(boardSize-1); y>=0; y--)
            {
                for(int x(0); x<boardSize; x++)
                {
                    _board[x][y]=distance;
                }
                distance++;
            }
            break;
        }
    }
};



class Wall : public BoardObject
{
private:
    string _orientation; // H or V

public:
    Wall(int x, int y, string orientation) : BoardObject(x, y), _orientation(orientation) {}
    ~Wall() {}

    string getOrientation() const
    {
        return _orientation;
    }

    bool equals(Wall const& wall) const
    {
        return this->getX() == wall.getX()
               && this->getY() == wall.getY()
               && this->getOrientation() == wall.getOrientation();
    }
};


class Game
{
private:
    Box _board[boardSize][boardSize];
    vector<Wall> _walls;
    vector<Player> _players;
    int _myId;

public:
    Game( int const playerCount, int const myId )
        : _myId (myId)
    {
        this->buildPlayers(myId, playerCount);
        this->buildBoard();
    }

    string play()
    {
        return getMyPlayer()->getBestAction(_board);
    }

    int getMyId() const
    {
        return _myId;
    }

    Player* getPlayer(int id)
    {
        return &_players[id];
    }

    Player* getMyPlayer()
    {
        return getPlayer(getMyId());
    }

    void updateWalls(Wall const& wall)
    {
        if(find(_walls.begin(), _walls.end(), wall)==_walls.end())  // if walls doesn't exists
        {
            this->_walls.push_back(wall);
        }
    }

private:
    void buildPlayers(int const myId, int const nbPlayers)
    {
        for(int id(0); id<nbPlayers; id++)
        {
            _players.push_back(Player(id, myId));
        }
    }

    void buildBoard()
    {
        Box *currentBox;

        for(int x(0); x<boardSize; x++)
        {
            for(int y(0); y<boardSize; y++)
            {
                _board[x][y] = Box();
            }
        }
        for(int x(0); x<boardSize; x++)
        {
            for(int y(0); y<boardSize; y++)
            {
                currentBox = &_board[x][y];
                if(y > 0)
                {
                    currentBox->setBottom(&_board[x][y-1]);
                }
                if(x > 0)
                {
                    currentBox->setLeft(&_board[x-1][y]);
                }
                if(y < (boardSize-1))
                {
                    currentBox->setTop(&_board[x][y+1]);
                }
                if(x < (boardSize-1))
                {
                    currentBox->setRight(&_board[x+1][y]);
                }
            }
        }
    }
};


// ----------------- OPERATORS --------------- //
ostream &operator<<( ostream &strm, BoardObject const& boardObject)
{
    return strm << "BoardObject[ x = " << boardObject.getX() << ", y = " << boardObject.getY() << " ]";
}


bool operator==(Wall const& a, Wall const& b)
{
    return a.equals(b);
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int w; // width of the board
    int h; // height of the board
    int playerCount; // number of players (2 or 3)
    int myId; // id of my player (0 = 1st player, 1 = 2nd player, ...)
    cin >> w >> h >> playerCount >> myId;
    cin.ignore();

    Game game(playerCount, myId);

    // game loop
    while (1)
    {
        for (int i = 0; i < playerCount; i++)
        {
            int x; // x-coordinate of the player
            int y; // y-coordinate of the player
            int wallsLeft; // number of walls available for the player
            cin >> x >> y >> wallsLeft;
            cin.ignore();
            game.getPlayer(i)->update(x, y, wallsLeft);
        }

        int wallCount; // number of walls on the board
        cin >> wallCount;
        cin.ignore();
        for (int i = 0; i < wallCount; i++)
        {
            int wallX; // x-coordinate of the wall
            int wallY; // y-coordinate of the wall
            string wallOrientation; // wall orientation ('H' or 'V')
            cin >> wallX >> wallY >> wallOrientation;
            cin.ignore();
            game.updateWalls(Wall(wallX, wallY, wallOrientation));
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        // action: LEFT, RIGHT, UP, DOWN or "putX putY putOrientation" to place a wall
        cout << game.play() << endl;
    }
}
