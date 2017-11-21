#include <iostream>
#include <unordered_map>
#include <string>
#include <limits>
#include <array>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <math.h>
#include <numeric>
#include <queue>
#include <iterator>
#include <SFML/Graphics.hpp>


const unsigned WINDOW_WIDTH = 1152;
const unsigned WINDOW_HEIGHT = 864;
const int NUM_COLUMNS = 19;
const int NUM_ROWS = 19;
const int NUM_SQUARES_ON_BOARD = NUM_COLUMNS * NUM_ROWS;
const int BOARD_WIDTH = (NUM_COLUMNS) * 40;
const int BOARD_HEIGHT = (NUM_ROWS) * 40;
int X_MARGIN = (WINDOW_WIDTH - BOARD_WIDTH) / 2;
int Y_MARGIN = (WINDOW_HEIGHT - BOARD_HEIGHT) / 2;
int SQUARE_SIZE = BOARD_WIDTH / NUM_COLUMNS;
int DISC_RADIUS = static_cast<int>(SQUARE_SIZE / 2 * 0.75);
int TEXT_SIZE = static_cast<int>(SQUARE_SIZE * 0.65);

//#define DEBUG


const int EXPLORE_CONST = 2;
const double EPSILON = 0.001;
const int NUM_PLAYOUTS = 4 * 1000000;
const double m_INFINITY = std::numeric_limits<double>::infinity();
const int TIME_PER_MOVE = 3 * 1000;//in milliseconds
const int NUM_GAMES = 1;
const int TIME_PER_GAME = 10000;



class GameState {
public:
    int player_just_moved;
    std::array<int, NUM_SQUARES_ON_BOARD> micro_board;
    std::array<int, NUM_SQUARES_ON_BOARD> micro_board_prev;

    GameState();
    GameState *clone();
    GameState(int l_player_just_moved, std::array<int, NUM_SQUARES_ON_BOARD> l_micro_board
                     , std::array<int, NUM_SQUARES_ON_BOARD> l_micro_board_prev, int l_micro_move);
    int get_player_just_moved();
    std::array<int, NUM_SQUARES_ON_BOARD> get_micro_board();
    std::array<int, NUM_SQUARES_ON_BOARD> get_micro_board_prev();
    void update_captures(int micro_move);
    void set_player_just_moved(int player);
    std::vector <int> gen_valid_moves(int color);
    double is_terminal_node(int l_color, int bot_color);
    int make_move(int micro_move);
    inline void unmake_move(int micro_move, int macro_move_index);
    inline void unmake_move_on_microboard(int l_index);
    inline int is_suicide(int l_move, int color);
    inline int is_ko(int l_move, int color);
    inline std::vector<int> get_neighbours(int l_move);
    inline void make_move_on_microboard(int l_index, int color);
    inline std::vector<int> get_group(int stone);
    inline void remove_group(const std::vector<int> &l_group);
};


GameState::GameState() {
    for(int i = 0; i < NUM_COLUMNS * NUM_ROWS; i++) {
        micro_board[i] = 0;
        micro_board_prev[i] = 0;
    }
}

GameState::GameState(int l_player_just_moved, std::array<int, NUM_SQUARES_ON_BOARD> l_micro_board
                     , std::array<int, NUM_SQUARES_ON_BOARD> l_micro_board_prev, int l_micro_move) {
    player_just_moved = l_player_just_moved;
    micro_board = l_micro_board;
    micro_board_prev = l_micro_board_prev;
}

GameState * GameState::clone() {
    GameState * st = new GameState();
    st->player_just_moved = get_player_just_moved();
    st->micro_board = get_micro_board();
    return st;
}



int GameState::get_player_just_moved(){return player_just_moved;}
std::array<int, NUM_SQUARES_ON_BOARD> GameState::get_micro_board(){return micro_board;}
std::array<int, NUM_SQUARES_ON_BOARD> GameState::get_micro_board_prev(){return micro_board_prev;}
inline void GameState::set_player_just_moved(int player){player_just_moved = player;}

inline int GameState::make_move(int micro_move) {
    set_player_just_moved(3 - player_just_moved);
    micro_board[micro_move] = player_just_moved;
    return 0;
}

inline void GameState::unmake_move(int micro_move, int macro_move_index) {
    unmake_move_on_microboard(micro_move);
    set_player_just_moved(3 - player_just_moved);
}

inline void GameState::unmake_move_on_microboard(int l_index){micro_board[l_index] = 0;}


inline double GameState::is_terminal_node(int player_just_moved, int bot_color) {
    //double return_val_first = (player_just_moved == 1) ? (1) : (0);
    //double return_val_second = 1 - return_val_first;//-l_color * SMALL_INFINITY;
    return 0;
}



inline std::vector <int> GameState::gen_valid_moves(int color) {
    std::vector <int> valid_moves;
    valid_moves.reserve(NUM_SQUARES_ON_BOARD);
    for(int i = 0; i < NUM_SQUARES_ON_BOARD; i++) {
        if((micro_board[i] == 0) && (is_suicide(i, color) == 0) && (is_ko(i, color) == 0)) {
            valid_moves.push_back(i);
        }
    }
    return valid_moves;
}

inline std::vector<int> GameState::get_neighbours(int l_move) {
    std::vector<int> neighbours;
    neighbours.reserve(4);
    if((l_move > 18) && (l_move < 342)) {
        if((l_move % 19 > 0) && (l_move % 19 < 18)) {
            neighbours.push_back(l_move - 1);
            neighbours.push_back(l_move + 1);
            neighbours.push_back(l_move - 19);
            neighbours.push_back(l_move + 19);
        } else if(l_move % 19 == 0) {
            neighbours.push_back(l_move + 1);
            neighbours.push_back(l_move + 19);
            neighbours.push_back(l_move + 19);
        } else {
            neighbours.push_back(l_move - 1);
            neighbours.push_back(l_move + 19);
            neighbours.push_back(l_move + 19);
        }
    } else {
        if(l_move == 0) {
            neighbours.push_back(1);
            neighbours.push_back(19);
        } else if(l_move == 18) {
            neighbours.push_back(17);
            neighbours.push_back(37);
        } else if(l_move == 342) {
            neighbours.push_back(323);
            neighbours.push_back(343);
        } else if(l_move == 360) {
            neighbours.push_back(341);
            neighbours.push_back(359);
        } else if(l_move < 18) {
            neighbours.push_back(l_move - 1);
            neighbours.push_back(l_move + 1);
            neighbours.push_back(l_move + 19);
        } else {
            neighbours.push_back(l_move - 1);
            neighbours.push_back(l_move + 1);
            neighbours.push_back(l_move - 19);
        }
    }
    return neighbours;
}

inline int GameState::is_suicide(int l_move, int color) {
    micro_board[l_move] = color;
    int is_suicide = (get_group(l_move).back() == 0) ? 1 : 0;
    micro_board[l_move] = 0;
    return is_suicide;
}

inline int GameState::is_ko(int l_move, int color) {
    int result = 1;
    std::array<int, NUM_SQUARES_ON_BOARD> micro_board_copy = micro_board;
    make_move_on_microboard(l_move, color);
    if(micro_board == micro_board_prev) {
        result = 0;
    }
    micro_board = micro_board_copy;
    return result;
}


inline std::vector<int> GameState::get_group(int l_move) {
    int num_liberties = 0;
    int color = micro_board[l_move];
    std::queue<int> frontier;
    frontier.push(l_move);
    std::vector<int> seen_so_far;
    seen_so_far.reserve(NUM_SQUARES_ON_BOARD);
    seen_so_far.push_back(l_move);
    std::vector<int> liberties;
    liberties.reserve(NUM_SQUARES_ON_BOARD);

    while(!frontier.empty())
    {
        int curr_stone = frontier.front();
        frontier.pop();
        std::vector<int> curr_neighbours = get_neighbours(curr_stone);
        curr_neighbours.reserve(4);
        int curr_neighbours_size = curr_neighbours.size();
        for(int i = 0; i < curr_neighbours_size; i++)
        {
            int next_stone = curr_neighbours[i];
            if(micro_board[next_stone] == color)
            {
                std::vector<int>::iterator pos = find(seen_so_far.begin(), seen_so_far.end(), next_stone);
                if(pos == seen_so_far.end())
                {
                    frontier.push(next_stone);
                    seen_so_far.push_back(next_stone);
                }
            }
            else if(micro_board[next_stone] == 0)
            {
                std::vector<int>::iterator pos = find(liberties.begin(), liberties.end(), next_stone);
                if(pos == liberties.end())
                {
                    //std::cout << "found liberty at " << next_stone << std::endl;
                    liberties.push_back(next_stone);
                    num_liberties++;
                }
            }
        }
    }
    seen_so_far.push_back(num_liberties);//last element in the vector will be the number of group's liberties
    return seen_so_far;
    //return num_liberties;
}

inline void GameState::remove_group(const std::vector<int> &l_group) {
    int group_size = l_group.size() - 1;//minus one because last element of the group is its number of liberties
    for(int i = 0; i < group_size; i++) {
        micro_board[l_group[i]] = 0;
    }
}


inline void GameState::make_move_on_microboard(int l_index, int color) {
    micro_board[l_index] = color;
    std::vector<int> neighbours = get_neighbours(l_index);
    int neighbours_size = neighbours.size();
    for(int i = 0; i < neighbours_size; i++) {
        int curr_neighbour = neighbours[i];
        std::vector<int> curr_neighbour_group = get_group(curr_neighbour);
        int curr_neighbour_group_liberties = curr_neighbour_group.back();
        if(curr_neighbour_group_liberties == 0) {
            remove_group(curr_neighbour_group);
        }
    }
    micro_board_prev = micro_board;
}




//=============================================================================================================================//
//=============================================================================================================================//
//=============================================================================================================================//
//=============================================================================================================================//
//=============================================================================================================================//
class Node {
private:
    int micro_move;
    int num_visits = 0;
    int num_wins = 0;
    double value;
    Node * parent_node;
    std::vector<Node *> child_nodes;
    GameState * game_state;
    std::vector<int> untried_moves;// = game_state->gen_valid_moves(game_state->get_target_macro_move(micro_move));

public:
    int player_just_moved;
    std::vector<int> get_untried_moves();
    int get_num_visits();
    double get_value();
    void back_propagate(int value, Node *node);
    Node *get_best_child_one();
    Node *get_best_child_two();
    //Node * add_child(const int &l_micro_move, GameState * l_game_state);
    Node *add_child(int l_micro_move, GameState *l_game_state);
    Node();
    Node(Node *l_parent_node, int l_micro_move, GameState *l_game_state);
    //~Node();
    Node *get_parent_node();
    void set_untried_moves(std::vector<int> valid_moves);
    int get_player_just_moved();
    void set_player_just_moved(int l_player_just_moved);
    std::vector<Node *> get_child_nodes();
    void update(int outcome);
    int get_micro_move();
    int get_num_wins();
    GameState * get_game_state();
    inline void delete_tree();
};



inline std::vector<int> Node::get_untried_moves(){return untried_moves;}
inline void Node::set_untried_moves(std::vector<int> valid_moves){std::vector<int> utried_moves = valid_moves;}
inline Node * Node::get_parent_node(){return parent_node;}
inline int Node::get_player_just_moved(){return player_just_moved;}
inline void Node::set_player_just_moved(int l_player_just_moved){player_just_moved = l_player_just_moved;}
inline int Node::get_micro_move(){return micro_move;}
inline int Node::get_num_wins(){return num_wins;}
inline std::vector<Node *> Node::get_child_nodes(){return child_nodes;}
inline GameState * Node::get_game_state(){return game_state;}

inline int Node::get_num_visits(){return num_visits;}

inline double Node::get_value(){return value;}

Node::Node(){};

Node::Node(Node *l_parent_node, int l_micro_move, GameState *l_game_state) {
    parent_node = l_parent_node;
    micro_move = l_micro_move;
    game_state = l_game_state;
    untried_moves = l_game_state->gen_valid_moves(player_just_moved);
}

inline Node * Node::get_best_child_one() {
    double best_score = -m_INFINITY;
    double exploit;
    double explore;
    int parent_num_visits = get_num_visits();
    //double curr_child_value;
    double curr_score;
    int curr_child_num_visits;
    int curr_child_num_wins;
    Node * best_child;
    std::vector<Node *> children = get_child_nodes();
    int children_size = (children.size());
    for(int i = 0; i < children_size; i++) {
        Node * curr_child = children[i];
        curr_child_num_wins = curr_child->get_num_wins();
        curr_child_num_visits = curr_child->get_num_visits();
        exploit = static_cast<double>(curr_child_num_wins) / curr_child_num_visits;
        explore = sqrt(log(parent_num_visits) /curr_child_num_visits);
        curr_score = exploit + explore / 2;
        if(curr_score > best_score) {
            best_child = curr_child;
            best_score = curr_score;
        }
    }
    return best_child;
}


inline Node *Node::get_best_child_two() {
    //std::cout << "entered get best child" << std::endl;
    double best_score = -m_INFINITY;
    double exploit;
    double explore;
    //double parent_value = parent->get_value();
    //std::cout << "best child test" << std::endl;
    //std::cout << "parent info: " << this << ", " << num_visits << ", " << num_wins << ", " << micro_move << ", " << parent_node << ", " << game_state << std::endl;
    int parent_num_visits = get_num_visits();
    //std::cout << "parent num visits: " << parent_num_visits << std::endl;
    //double curr_child_value;
    double curr_score;
    int curr_child_num_visits;
    int curr_child_num_wins;
    //Node curr_child;
    //std::vector <Node *> best_children;
    //best_children.reserve(81);
    Node * best_child;
    std::vector<Node *> children = get_child_nodes();
    int children_size = (children.size());
    //std::cout << "child nodes size: " << children_size << std::endl;
    for(int i = 0; i < children_size; i++) {
        Node * curr_child = children[i];
        //curr_child_value = curr_child->get_value();
        curr_child_num_wins = curr_child->get_num_wins();
        curr_child_num_visits = curr_child->get_num_visits();
        exploit = static_cast<double>(curr_child_num_wins) / curr_child_num_visits;
        explore = sqrt(log(parent_num_visits) /curr_child_num_visits);
        curr_score = exploit + explore / 2;
        //std::cout << "AAA: " << curr_child << ", " << static_cast<double>(curr_child_num_wins) << static_cast<double>(parent_num_visits) << ", " << log(2 * static_cast<double>(parent_num_visits)) << ", " << static_cast<double>(curr_child_num_visits) << ", " << (log(2 * static_cast<double>(parent_num_visits)) / static_cast<double>(curr_child_num_visits)) << ", " << sqrt(log(2 * static_cast<double>(parent_num_visits)) / static_cast<double>(curr_child_num_visits)) << std::endl;
        //std::cout << "BBB: " << i << ", " << curr_child_num_wins << ", " << curr_child_num_visits << ", " << exploit << ", " << explore << ", " << curr_score << best_score << std::endl;


        if(curr_score > best_score) {
            best_child = curr_child;
            best_score = curr_score;
        }
        //std::cout << "curr best child: " << best_child << ", curr best score: " << best_score << std::endl;
    }
    //std::cout << "exited get best child, returned " << best_child << std::endl;
    return best_child;
}


inline Node *Node::add_child(int l_micro_move, GameState *l_game_state) {
    //std::cout << "entered add child" << std::endl;
    //std::cout << "parent node: " << this << std::endl;
    //std::cout << "parent node info: " << this << ", " << this->get_num_visits() << ", " << this->get_num_wins() << ", " << this->get_micro_move() << ", " << this->get_parent_node() << ", " << this->get_game_state() << std::endl;
    Node * node = new Node(this, l_micro_move, l_game_state);
    //std::cout << "player just moved parent: " << get_player_just_moved() << std::endl;
    node->set_player_just_moved(3 - get_player_just_moved());
    //std::cout << "player just moved child: " << (node->get_player_just_moved()) << std::endl;
    //node->set_player_just_moved();
    //std::vector <int> valid_moves = gen_valid_moves(l_macro_move_index);
    untried_moves.erase(std::remove(untried_moves.begin(), untried_moves.end(), l_micro_move), untried_moves.end());
    child_nodes.push_back(node);
    //std::cout << "exited add child" << std::endl;
    return node;
}

inline void Node::update(int outcome) {
    num_visits++;
    num_wins += outcome;
}

inline void Node::delete_tree() {
    int l_child_size = (get_child_nodes()).size();
    //std::cout << "child size: " << l_child_size << std::endl;
    if(l_child_size != 0) {
        std::vector<Node *> l_child_nodes = get_child_nodes();
        for(int i = 0; i < l_child_size; i++) {
            l_child_nodes[i]->delete_tree();
        }
    }
    delete this;
}



inline void Node::back_propagate(int value, Node * node) {
    while(node != nullptr) {
        (node->num_visits)++;
        (node->value) += value;
        node = (node->parent_node);
    }
}
//=============================================================================================================================//
//=============================================================================================================================//
//=============================================================================================================================//
//=============================================================================================================================//
//=============================================================================================================================//


typedef struct l_move_score_pair {
    int my_move;
    double my_score;
} move_score_pair;

typedef struct my_node {
    int num_visits;
    double score;
} node;

bool compare_by_eval_descending(const move_score_pair &score_move_1, const move_score_pair score_move_2) {
    return score_move_1.my_score > score_move_2.my_score;
}


class Bot {
public:
    Bot();
    void run_console();
    void update_board_from_input(std::string &l_string);
    void make_move();
    std::vector <int> gen_valid_moves(int macro_index);
    double eval_board(int color);
    void unmake_move_on_microboard(int move_index, int l_color);
    void draw_board();
    void create_board();
    int get_input();
    int get_side_to_move();
    void switch_side_to_move();
    void print_info_to_console();
    int get_move_number();
    int check_is_finished();
    void set_color(int l_color);    
    void inc_move_number();
    void dec_move_number();
    void add_move_to_history(int l_move);
    void gen_opening_book_first();
    void gen_opening_book_second();
    void update_microboard_from_input(std::string &l_string);
    std::array <int, 2> gen_naive_compute_move();
    void set_tile(int l_tile);    
    inline int get_target_macro_move(int move_index);
    int RandomComputerMove();
    int simulate(int current_move, int color);
    //int make_next_move(const int &l_macro_move_index, const int &color);
    int monte_carlo_search();
    int monte_carlo_search_two();
    int root_simulate(int l_macro_move_index, int color);
    int root_policy(const std::vector<int> &valid_moves_playouts, const std::vector<int> &valid_moves_scores, const int &valid_moves_size);    
    void init_boards();
    inline double is_terminal_node(int player_just_moved);    
    void draw_board_update();
    double check_is_finished(int color_to_move);
    int get_is_finished();
    void run();
    void test();

    int opp_micro_move;
    sf::RenderWindow * get_game_window();
    sf::RectangleShape * get_board_rect();
    std::array <int, NUM_SQUARES_ON_BOARD> micro_board;
    std::array <int, NUM_SQUARES_ON_BOARD> micro_board_prev;


private:
    //GameState * root_state;
    //Node * root_node;
    //int id;
    //int m_color_to_move = 1;
    //int m_side_to_move;
    //int m_color_one = 2;
    //int m_tile_one = 2;
    int m_color_two = 2;
    int m_tile_two = 2;
    char opp_tile;
    int m_color = 2;//bot's color
    int player_just_moved;
    int m_minimax_depth = 7;
    //bool is_finished = 0;
    int m_move_number = 0;
    //std::array <int, 42> m_move_history;
    int think_time;
    std::chrono::steady_clock::time_point begin;
    int timeout_triggered_flag = 0;
    //Bot();
    //~Bot();
    //void set_player_just_moved(const int &player);
    sf::RenderWindow m_window;
    sf::RectangleShape m_board_rect;
    sf::RectangleShape m_screen_rect;
    int m_side_to_move = 42;// equals -42 if bot moves first, 42 otherwise
    int m_color_to_move = 1;//1 is black, 2 is white
    bool is_finished = 0;

};


Bot::Bot() {
    for (int i = 0; i < NUM_SQUARES_ON_BOARD; i++) {
        micro_board[i] = 0;
        micro_board_prev[i] = 0;
    }
    m_move_number = 0;
    //gen_opening_book_first();
    //gen_opening_book_second();
}


sf::RenderWindow * Bot::get_game_window(){return &m_window;}
sf::RectangleShape * Bot::get_board_rect(){return &m_board_rect;}

void Bot::create_board() {
    //for (int i = 0; i < NUM_COLUMNS * NUM_ROWS; i++)
    //{
        //micro_board[i] = 0;
    //}
    auto style = sf::Style::Default;
    sf::ContextSettings m_settings;
    m_settings.antialiasingLevel = 8;
    std::string m_window_title = "my Go game";
    m_window.create({WINDOW_WIDTH, WINDOW_HEIGHT}, m_window_title, style, m_settings);
    m_window.setFramerateLimit(30);
    //m_window.clear(sf::Color(0, 255, 150));
    //m_window.clear(sf::Color(255, 150, 50));
    m_window.clear(sf::Color::White);
    //m_window.setVerticalSyncEnabled(true);
    m_screen_rect.setSize(sf::Vector2f(BOARD_WIDTH, BOARD_HEIGHT));
    m_screen_rect.setFillColor(sf::Color::White);
    m_screen_rect.setOrigin(-X_MARGIN, -Y_MARGIN);
    m_screen_rect.setPosition(0, 0);
    get_game_window()->draw(m_screen_rect);

    m_board_rect.setSize(sf::Vector2f((NUM_COLUMNS - 1) * SQUARE_SIZE, (NUM_ROWS - 1) * SQUARE_SIZE));
    m_board_rect.setFillColor(sf::Color(255, 150, 50));
    m_board_rect.setOrigin(-X_MARGIN, -Y_MARGIN);
    m_board_rect.setPosition(0, 0);
    //m_board_rect.setPosition(X_MARGIN, Y_MARGIN);
    get_game_window()->draw(m_board_rect);

    double black_line_thickness = 1.25;

    sf::RectangleShape black_line_rect_horiz;
    black_line_rect_horiz.setSize(sf::Vector2f(BOARD_WIDTH - SQUARE_SIZE, 1));
    black_line_rect_horiz.setFillColor(sf::Color::Black);
    black_line_rect_horiz.setOutlineColor(sf::Color::Black);
    black_line_rect_horiz.setOutlineThickness(black_line_thickness);
    black_line_rect_horiz.setOrigin(0, 0);

    sf::RectangleShape black_line_rect_vert;
    black_line_rect_vert.setSize(sf::Vector2f(1, BOARD_HEIGHT - SQUARE_SIZE));
    black_line_rect_vert.setFillColor(sf::Color::Black);
    black_line_rect_vert.setOutlineColor(sf::Color::Black);
    black_line_rect_vert.setOutlineThickness(black_line_thickness);
    black_line_rect_vert.setOrigin(0, 0);


    //draw board grid (vertical and perpendicular lines)
    for (int i = 0; i < NUM_COLUMNS; i++) {

        //sf::Vertex horiz_line[] =
        //{
            //sf::Vertex(sf::Vector2f(X_MARGIN, Y_MARGIN + i * SQUARE_SIZE), sf::Color::Black),
            //sf::Vertex(sf::Vector2f(WINDOW_WIDTH - X_MARGIN * 0, Y_MARGIN + i * SQUARE_SIZE), sf::Color::Black)
        //};
        black_line_rect_horiz.setPosition(X_MARGIN, Y_MARGIN + i * SQUARE_SIZE);
        get_game_window()->draw(black_line_rect_horiz);

        //sf::Vertex vert_line[] =
        //{
            //sf::Vertex(sf::Vector2f(X_MARGIN + i * SQUARE_SIZE, Y_MARGIN), sf::Color::Black),
            //sf::Vertex(sf::Vector2f(X_MARGIN + i * SQUARE_SIZE, WINDOW_HEIGHT - Y_MARGIN), sf::Color::Black)
        //};
        black_line_rect_vert.setPosition(X_MARGIN + i * SQUARE_SIZE, Y_MARGIN);
        (*get_game_window()).draw(black_line_rect_vert);
    }
    sf::CircleShape white_circle;
    white_circle.setRadius(DISC_RADIUS);
    white_circle.setFillColor(sf::Color::White);
    white_circle.setOutlineThickness(1);
    white_circle.setOutlineColor(sf::Color::Black);

    std::string num_array[NUM_COLUMNS] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "10", "11", "12", "13", "14", "15", "16", "17"};
    sf::Font font;
    font.loadFromFile("resources/fonts/ubuntu.ttf");

    //draw board coordinates
    for (int j = 0; j < NUM_COLUMNS; j++) {
        sf::Text num_text("", font, TEXT_SIZE);
        num_text.setColor(sf::Color::Black);
        num_text.setString(num_array[j]);
        num_text.setPosition(X_MARGIN + SQUARE_SIZE * j + static_cast<int>((SQUARE_SIZE - TEXT_SIZE) * 0.75), Y_MARGIN + static_cast<int>((SQUARE_SIZE - TEXT_SIZE) * 0.2));
    }
    m_window.display();
}

void Bot::draw_board() {
    m_window.display();
}


void Bot::draw_board_update() {
    //std::cout << "entered update" << std::endl;
    sf::CircleShape black_circle;
    black_circle.setRadius(DISC_RADIUS);
    black_circle.setFillColor(sf::Color::Black);
    black_circle.setOutlineThickness(2.35);
    black_circle.setOutlineColor(sf::Color::Black);

    sf::CircleShape white_circle;
    white_circle.setRadius(DISC_RADIUS);
    white_circle.setFillColor(sf::Color::White);
    white_circle.setOutlineThickness(2.35);
    white_circle.setOutlineColor(sf::Color::Black);

    for (int i = 0; i < NUM_COLUMNS; i++) {
        for (int j = 0; j < NUM_ROWS; j++) {
            if (micro_board[NUM_COLUMNS * i + j] == 1) {
                black_circle.setPosition(X_MARGIN + (j - 0.5) * SQUARE_SIZE + (SQUARE_SIZE - 2 * DISC_RADIUS) / 2,
                                         Y_MARGIN + (i - 0.5) * SQUARE_SIZE + (SQUARE_SIZE - 2 * DISC_RADIUS) / 2);
                //(*(get_game_window())).draw(black_circle);
                get_game_window()->draw(black_circle);
            } else if (micro_board[NUM_COLUMNS * i + j] == 2) {
                white_circle.setPosition(X_MARGIN + (j - 0.5) * SQUARE_SIZE + (SQUARE_SIZE - 2 * DISC_RADIUS) / 2,
                                         Y_MARGIN + (i - 0.5) * SQUARE_SIZE + (SQUARE_SIZE - 2 * DISC_RADIUS) / 2);
                //(*(get_game_window())).draw(white_circle);
                get_game_window()->draw(white_circle);
            }
        }
    }
    //m_window.display();
}


int Bot::get_input() {
    int x_coord;
    int y_coord;
    int input_move;
    sf::Event player_event;
    while (m_window.pollEvent(player_event)) {
        //std::cout << "in while" << std::endl;   
        if (player_event.type == sf::Event::Closed) {
            m_window.close();
        } else if (player_event.type == sf::Event::MouseButtonPressed) {
            if (player_event.mouseButton.button == sf::Mouse::Left) {
                std::cout << "left click" << std::endl;
                sf::Vector2f cursor_pos(player_event.mouseButton.x, player_event.mouseButton.y);
                x_coord = (cursor_pos.x - X_MARGIN + SQUARE_SIZE / 2) / SQUARE_SIZE;
                y_coord = (cursor_pos.y - Y_MARGIN + SQUARE_SIZE / 2) / SQUARE_SIZE;
                sf::FloatRect containing_rect((*get_board_rect()).getLocalBounds().left + X_MARGIN - SQUARE_SIZE / 2,
                                              (*get_board_rect()).getLocalBounds().top + Y_MARGIN - SQUARE_SIZE / 2,
                                              (*get_board_rect()).getLocalBounds().width + SQUARE_SIZE,
                                              (*get_board_rect()).getLocalBounds().height + SQUARE_SIZE);
                if (containing_rect.contains(cursor_pos)) {                    
                    int input_x_coord = x_coord;
                    int input_y_coord = y_coord;
                    input_move = input_x_coord + input_y_coord * NUM_COLUMNS;
                    return input_move;
                }
            }
        }
        //m_window.display();
    }
    //std::cout << "out while " << std::endl;
    return -1;
}


inline void Bot::inc_move_number(){m_move_number++;}
inline void Bot::dec_move_number(){m_move_number--;}
int Bot::get_move_number(){return m_move_number;}
int Bot::get_side_to_move(){return m_side_to_move;}


int Bot::RandomComputerMove() {
    GameState * curr_state = new GameState(3 - m_color_to_move, micro_board, micro_board_prev, opp_micro_move);
    std::vector<int> valid_moves = curr_state->gen_valid_moves(m_color);
    int computer_move = valid_moves[rand() % valid_moves.size()];    
    delete curr_state;
    return computer_move;
}

int Bot::monte_carlo_search() {

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    GameState * root_state = new GameState(3 - m_color, micro_board, micro_board_prev, opp_micro_move);
    Node * root_node = new Node(nullptr, opp_micro_move, root_state);
    Node * node = nullptr;
    //int result;
    //int best_result;
    int best_move;
    int random_move;
    int rand_index;
    //int counter = 0;

    for(int i = 0; i < NUM_PLAYOUTS; i++) {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        think_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        //std::cout << "think time: " << think_time << std::endl;
        if(think_time > TIME_PER_MOVE) {
            std::cout << "mcts stopped at iteration " << i << std::endl;
            break;
        }
        node = root_node;

        GameState * state = new GameState(3 - m_color, micro_board, micro_board_prev, opp_micro_move);
        //state->set_player_just_moved(3 - m_color_one);
        node->set_player_just_moved(3 - m_color);

        while(((node->get_untried_moves().size()) == 0) && ((state->is_terminal_node(state->get_player_just_moved(), m_color)) == 77))
        {
            #ifdef DEBUG
            std::cout << "_____________entered select_____________" << std::endl;
            std::cout << "node: " << node << ", root node: " << root_node << std::endl;
            std::cout << "node info: " << node << ", " << node->get_num_visits() << ", " << node->get_num_wins() << ", " << node->get_micro_move() << ", " << node->get_parent_node() << ", " << node->get_game_state() << std::endl;
            #endif // DEBUG
            node = node->get_best_child_one();
            #ifdef DEBUG
            std::cout << "child node chosen: " << node << ", root node: " << root_node << std::endl;
            std::cout << "child node info: " << node << ", " << node->get_num_visits() << ", " << node->get_num_wins() << ", " << node->get_micro_move() << ", " << node->get_parent_node() << ", " << node->get_game_state() << std::endl;
            std::cout << "state: " << state << ", root state: " << root_state << std::endl;
            #endif // DEBUG
            state->make_move(node->get_micro_move());
            #ifdef DEBUG
            std::cout << "state: " << state << ", root state: " << root_state << std::endl;
            std::cout << "exited select" << std::endl;
            #endif // DEBUG
        }

        int untried_size = node->get_untried_moves().size();
        if(untried_size != 0) {
            rand_index = rand() % untried_size;
            random_move = node->get_untried_moves()[rand_index];
            state->make_move(random_move);
            node = node->add_child(random_move, state);
        }

        while((state->is_terminal_node(state->get_player_just_moved(), m_color)) == 77) {
            std::vector<int> valid_moves = state->gen_valid_moves(3 - (state->get_player_just_moved()));
            int valid_size = valid_moves.size();
            random_move = valid_moves[rand() % valid_size];
            state->make_move(random_move);
        }
        while(node != nullptr) {
            node->update(state->is_terminal_node(node->get_player_just_moved(), m_color));
            node = node->get_parent_node();
        }
        if(state){delete state;}
    }

    int best_num_visits = -1;
    std::vector<Node *> root_child_nodes = root_node->get_child_nodes();
    int root_child_size = root_child_nodes.size();
    for(int i = 0; i < root_child_size; i++) {
        int num_vis = root_child_nodes[i]->get_num_visits();
        if(num_vis > best_num_visits) {
            best_move = root_child_nodes[i]->get_micro_move();
            best_num_visits = num_vis;
        }
    }
    root_node->delete_tree();
    delete root_state;

    return best_move;
}



int Bot::monte_carlo_search_two() {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    GameState * root_state = new GameState(-m_color_two, micro_board, micro_board_prev, opp_micro_move);
    root_state->set_player_just_moved(3 - m_color_two);
    Node * root_node = new Node(nullptr, opp_micro_move, root_state);
    //int result;
    //int best_result;
    int best_move;
    int random_move;
    Node * node = nullptr;


    for(int i = 0; i < NUM_PLAYOUTS; i++) {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        think_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        if(think_time > TIME_PER_MOVE) {
            std::cout << "mcts stopped at iteration " << i << std::endl;
            break;
        }
        node = root_node;

        GameState * state = new GameState(-m_color_two, micro_board, micro_board_prev, opp_micro_move);
        state->set_player_just_moved(3 - m_color_two);
        node->set_player_just_moved(3 - m_color_two);

        while(((node->get_untried_moves().size()) == 0) && ((state->is_terminal_node(state->get_player_just_moved(), m_color_two)) == 77))
        {
            #ifdef DEBUG
            std::cout << "_____________entered select_____________" << std::endl;
            std::cout << "node: " << node << ", root node: " << root_node << std::endl;
            std::cout << "node info: " << node << ", " << node->get_num_visits() << ", " << node->get_num_wins() << ", " << node->get_micro_move() << ", " << node->get_parent_node() << ", " << node->get_game_state() << std::endl;
            #endif // DEBUG
            node = node->get_best_child_two();
            #ifdef DEBUG
            std::cout << "child node chosen: " << node << ", root node: " << root_node << std::endl;
            std::cout << "child node info: " << node << ", " << node->get_num_visits() << ", " << node->get_num_wins() << ", " << node->get_micro_move() << ", " << node->get_parent_node() << ", " << node->get_game_state() << std::endl;
            std::cout << "state: " << state << ", root state: " << root_state << std::endl;
            #endif // DEBUG
            state->make_move(node->get_micro_move());
            #ifdef DEBUG
            std::cout << "state: " << state << ", root state: " << root_state << std::endl;
            std::cout << "exited select" << std::endl;
            #endif // DEBUG
        }

        int untried_size = node->get_untried_moves().size();
        if(untried_size != 0) {
            int rand_index = rand() % untried_size;
            int random_move = node->get_untried_moves()[rand_index];
            state->make_move(random_move);
            node = node->add_child(random_move, state);
        }
        while((state->is_terminal_node(state->get_player_just_moved(), m_color_two)) == 77) {
            std::vector<int> valid_moves = state->gen_valid_moves(3 - (state->get_player_just_moved()));
            int valid_size = valid_moves.size();
            random_move = valid_moves[rand() % valid_size];
            state->make_move(random_move);
        }
        while(node != nullptr)
        {
            node->update(state->is_terminal_node(node->get_player_just_moved(), m_color_two));
            node = node->get_parent_node();
        }
        if(state){delete state;}
    }
    int best_num_visits = -1;
    std::vector<Node *> root_child_nodes = root_node->get_child_nodes();
    int root_child_size = root_child_nodes.size();
    for(int i = 0; i < root_child_size; i++) {
        int num_vis = root_child_nodes[i]->get_num_visits();
        if(num_vis > best_num_visits) {
            best_move = root_child_nodes[i]->get_micro_move();
            best_num_visits = num_vis;
        }
    }
    root_node->delete_tree();
    delete root_state;
    return best_move;
}


inline double Bot::is_terminal_node(int player_just_moved) {
    //double return_val_first = (player_just_moved == 1) ? (1) : (0);
    //double return_val_second = 1 - return_val_first;//-l_color * SMALL_INFINITY;
    return 0;
}


int Bot::get_is_finished(){return is_finished;}


void Bot::make_move() {
    if (get_side_to_move() == 42) {
        std::cout << "=============================== " << m_move_number << " ===================================" << std::endl;
        std::cout << "it's player move" << std::endl;
        std::cout << "color_to_move: " << m_color_to_move << ", side to move: " <<
            m_side_to_move << std::endl;
        GameState * curr_state = new GameState(3 - m_color_to_move, micro_board, micro_board_prev, opp_micro_move);
        std::vector<int> valid_moves = curr_state->gen_valid_moves(3 - m_color);
        int valid_size = valid_moves.size();
        bool got_input = false;
        bool found_move = false;
        int input_move;
        while (!got_input) {
            input_move = get_input();
            //std::cout << "input_move: " << input_move << std::endl;
            if (input_move > -1) {
                for(int i = 0; i < valid_size; i++) {
                    if(input_move == valid_moves[i]) {
                        found_move = true;
                        break;
                    }
                }
                if(found_move == true) {
                    std::cout << "found player move: " << input_move << std::endl;
                    micro_board[input_move] = m_color_to_move;
                    opp_micro_move = input_move;
                    inc_move_number();
                    m_color_to_move = 3 - m_color_to_move;
                    m_side_to_move *= -1;
                    got_input = true;
                }
            }
            //m_window.clear(sf::Color(255, 255, 255));
            //m_window.display();
        }
        delete curr_state;
        //for(int i = 0; i < 81; i++) {
            //if((i > 0) && (i % 9 == 0))
                //std::cout << std::endl;
            //std::cout << micro_board[i] << ", ";
        //}
        //std::cout << std::endl;
    } else {
        std::cout << "=============================== " << m_move_number << " ===================================" << std::endl;
        std::cout << "it's computer turn" << std::endl;
        std::cout << "color_to_move: " << m_color_to_move << ", side to move: " <<
            m_side_to_move << std::endl;
        //int computer_move = monte_carlo_search();
        int computer_move = RandomComputerMove();
        std::cout << "computer made move: " << computer_move << std::endl;
        std::cout << "after: color_to_move: " << m_color_to_move << ", side to move: " <<
            m_side_to_move << std::endl;
        //m_window.display();
        micro_board[computer_move] = m_color_to_move;
        //draw_board_update();
        //m_window.display();
        opp_micro_move = computer_move;
        inc_move_number();
        m_color_to_move = 3 - m_color_to_move;
        m_side_to_move *= -1;
        micro_board_prev = micro_board;
    }
}

double Bot::check_is_finished(int color_to_move) {
    return is_terminal_node(m_color_to_move);
}

void Bot::run() {
    create_board();
    //GameState * state = new GameState(root_state->player_just_moved, root_state->micro_board, root_state->macro_board,
                                      //root_state->macro_target, root_state->micro_move);
    if (m_side_to_move == 42) {
        m_color = 2;
    } else {
        m_color = 1;
    }

    while(m_window.isOpen()) {
        //m_window.display();
        make_move();
        //m_window.clear(sf::Color(255, 255, 255));
        draw_board_update();
        m_window.display();
        //double outcome = check_is_finished(3 - m_color_to_move);
        //if (abs(outcome) < 77)
        if(0)
        {
            draw_board_update();
            m_window.display();
            std::cout << std::endl;
            std::cout << "********************" << std::endl;
            std::cout << "GAME ENDED!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(4));
            break;
        }
    }
}


void Bot::test()
{
    micro_board[0] = 1;
    micro_board[1] = 1;
    micro_board[2] = 1;
    micro_board[10] = 1;
    micro_board[5] = 1;
    micro_board[7] = 1;
    micro_board[25] = 1;
    micro_board[13] = 1;
    micro_board[12] = 2;
    micro_board[30] = 1;
    micro_board[31] = 1;
    micro_board[17] = 1;
    micro_board[18] = 1;
    micro_board[19] = 1;
    micro_board[20] = 1;
    micro_board[22] = 1;
    micro_board[30] = 1;
    micro_board[40] = 1;
    micro_board[41] = 1;
    micro_board[341] = 1;
    micro_board[359] = 1;

    /*

    micro_board[20] = 1;
    micro_board[21] = 2;
    micro_board[40] = 1;
    micro_board[100] = 1;
    micro_board[200] = 1;
    micro_board[201] = 1;
    micro_board[220] = 1;
    micro_board[221] = 1;
    micro_board[222] = 1;
    */
    create_board();
    draw_board_update();
    for(int i = 0; i < 81; i++) {
        if((i > 0) && (i % 9 == 0))
            std::cout << std::endl;
        std::cout << micro_board[i] << ", ";
    }
    std::cout << std::endl;
    GameState * root_state = new GameState(3 - m_color, micro_board, micro_board_prev, opp_micro_move);
    std::vector<int> a = root_state->get_group(0);
    std::this_thread::sleep_for(std::chrono::seconds(3000));
    delete root_state;
}

int main() {
    std::srand(std::time(nullptr));
    Bot my_bot;
    //my_bot.test();
    my_bot.run();
    return 0;
}

