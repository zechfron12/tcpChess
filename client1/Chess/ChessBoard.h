#include <SFML/Graphics.hpp>
#include <iostream>
#include "utilityBox.h"
#include "save.h"
#include <fstream>
#include <string.h>
#include <json-c/json.h>
using namespace sf;
using namespace std;

sf::Color greenish(116,151,84);
sf::Color paleWhite(238,238,210);
sf::Color redish(215,72,64);

int spritepositions[64] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15,
    64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64,
    48, 49, 50, 51, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61, 62, 63};
int board[64] = {
    -1, -2, -3, -4, -5, -3, -2, -1,
    -6, -6, -6, -6, -6, -6, -6, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    6, 6, 6, 6, 6, 6, 6, 6,
    1, 2, 3, 4, 5, 3, 2, 1};

class ChessBoard
{
private:
  const float WIDTH = 650;
  const float HEIGTH = 650;

public:
  void loadtextures(Texture texture[64]);
  void loadboard(Texture texture[64], RectangleShape rectangle[64], Sprite sprite[64]);
  void MainFunctions(int u, int playerId);
};

void ChessBoard::loadtextures(Texture texture[64])
{
  for (int i = 0; i < 64; i++)
  {
    if (spritepositions[i] == 0 || spritepositions[i] == 7)
      texture[i].loadFromFile("Chess/images/blackRook.png");
    if (spritepositions[i] == 1 || spritepositions[i] == 6)
      texture[i].loadFromFile("Chess/images/blackKnight.png");
    if (spritepositions[i] == 2 || spritepositions[i] == 5)
      texture[i].loadFromFile("Chess/images/blackBishop.png");
    if (spritepositions[i] == 3)
      texture[i].loadFromFile("Chess/images/blackQueen.png");
    if (spritepositions[i] == 4)
      texture[i].loadFromFile("Chess/images/blackKing.png");
    if (spritepositions[i] >= 8 && spritepositions[i] <= 15)
      texture[i].loadFromFile("Chess/images/blackPawn.png");
    if (spritepositions[i] == 63 || spritepositions[i] == 56)
      texture[i].loadFromFile("Chess/images/whiteRook.png");
    if (spritepositions[i] == 62 || spritepositions[i] == 57)
      texture[i].loadFromFile("Chess/images/whiteKnight.png");
    if (spritepositions[i] == 61 || spritepositions[i] == 58)
      texture[i].loadFromFile("Chess/images/whiteBishop.png");
    if (spritepositions[i] == 59)
      texture[i].loadFromFile("Chess/images/whiteQueen.png");
    if (spritepositions[i] == 60)
      texture[i].loadFromFile("Chess/images/whiteKing.png");
    if (spritepositions[i] >= 48 && spritepositions[i] <= 55)
      texture[i].loadFromFile("Chess/images/whitePawn.png");
  }
}
void ChessBoard::loadboard(Texture texture[64], RectangleShape rectangle[64], Sprite sprite[64])
{
  for (int j = 0; j < 64; j++)
  {
    sprite[j].setTexture(texture[j]);
    sprite[j].setScale(1.7f, 1.7f);
  }
  for (int j = 0; j < 64; ++j)
  {
    rectangle[j].setSize(sf::Vector2f(WIDTH / 8.0f, HEIGTH / 8.0f));
  }
  int counter = 0;
  for (int i = 0; i < 8; ++i)
  {
    for (int j = 0; j < 8; ++j)
    {
      rectangle[counter].setPosition(j * rectangle[counter].getSize().y, i * rectangle[counter].getSize().x); /// x,y
      sprite[counter].setPosition(j * rectangle[counter].getSize().y, i * rectangle[counter].getSize().x);
      if ((i + j) % 2 == 0)
        rectangle[counter].setFillColor(paleWhite);
      else
        rectangle[counter].setFillColor(greenish);
      counter++;
    }
  }
}
void ChessBoard::MainFunctions(int u, int playerId)
{
  const char *title;
  if(playerId % 2 == 1)
  title =  "You represent white pieces!";
  else title =  "You represent black pieces!";

  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGTH), title);
  Identity box;
  bool isMove;
  Vector2f oldPos, newPos;
  int n;
  int cc, turn = 1;
  int alience, position;
  Vector2f firstpos, secondpos;
  int v;
  int q[64];
  int needUpdate, newTurn, idFromFile;
  static int cap = 0;
  size_t nPos;

  json_object *parsed_json;
  json_object *parsing_json;
  json_object *jstatus;

  json_object *jturn;
  json_object *jplayerId;

  json_object *jneedUpdate;
  json_object *jisGameOver;

  json_object *jboard;
  json_object *jspritePositions;

  json_object *jpos;

  while (window.isOpen())
  {
    parsed_json = json_object_from_file("recv.json");

    json_object_object_get_ex(parsed_json, "turn", &jturn);
    json_object_object_get_ex(parsed_json, "board", &jboard);
    json_object_object_get_ex(parsed_json, "spritePositions", &jspritePositions);
    json_object_object_get_ex(parsed_json, "playerId", &jplayerId);
    json_object_object_get_ex(parsed_json, "needUpdate", &jneedUpdate);

    idFromFile = json_object_get_int(jplayerId);
    needUpdate = json_object_get_int(jneedUpdate);

    if (idFromFile != playerId && needUpdate == 1)
    {

      printf("ChessBoard.h : Update needed! This is the json: \n %s \n", json_object_to_json_string(parsed_json));

      turn = json_object_get_int(jturn);

      enum json_type type1 = json_object_get_type(jboard);
      enum json_type type2 = json_object_get_type(jspritePositions);

      if (type1 == json_type_array && type2 == json_type_array)
      {
        for (int i = 0; i < 64; i++)
        {
          jpos = json_object_array_get_idx(jboard, i);
          board[i] = json_object_get_int(jpos);

          jpos = json_object_array_get_idx(jspritePositions, i);
          spritepositions[i] = json_object_get_int(jpos);
        }
      }
      needUpdate = 0;
      jneedUpdate = json_object_new_int(needUpdate);
      json_object_object_del(parsed_json, "needUpdate");
      json_object_object_add(parsed_json, "needUpdate", jneedUpdate);
      json_object_to_file("recv.json", parsed_json);
      json_object_to_file("send.json", parsed_json);
    }

      sf::RectangleShape rectangle[64];
      sf::Texture texture[65];
      sf::Sprite sprite[65];
      loadtextures(texture);
      loadboard(texture, rectangle, sprite);

      for (int j = 0; j < 64; ++j)
        q[j] = 64;
      Vector2i pos = Mouse::getPosition(window);
      sf::Event event;
      while (window.pollEvent(event))
      {

        if (event.type == sf::Event::Closed)
        {
          save s;
          window.close();
          if (s.smain())
          {
            // ofstream out, out2, out3;
            // out.open("spritepositions.txt");
            // out2.open("boardpositions.txt");
            // out3.open("qpositions.txt");
            // for (int i = 0; i < 64; i++)
            // {
            //   out << spritepositions[i] << ",";
            //   out2 << board[i] << ",";
            //   out3 << q[i] << ",";
            // }
            // out.close();
            // out2.close();
            // out3.close();
          }
        }
        if (u != 0)
        {
          if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
          {
            for (int j = 0; j < 64; ++j)
            {
              if (turn % 2 == 0 && board[j] < 0)
              {
                if (rectangle[j].getGlobalBounds().contains(pos.x, pos.y))
                {
                  n = j;
                  firstpos = rectangle[j].getPosition();
                  v = spritepositions[j];
                  rectangle[n].setFillColor(redish);
                  if (spritepositions[n] != 64)

                    cap++;
                }
              }
            }
            for (int j = 0; j < 64; ++j)
            {
              if (turn % 2 != 0 && board[j] > 0)
              {
                if (rectangle[j].getGlobalBounds().contains(pos.x, pos.y))
                {
                  n = j;
                  firstpos = rectangle[j].getPosition();
                  v = spritepositions[j];
                  rectangle[n].setFillColor(redish);
                  if (spritepositions[n] != 64)
                    cap++;
                }
              }
            }
          }
          if (cap != 0)
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
              for (int j = 0; j < 64; ++j)
              {
                // cout<<box.identifier(n,j,board[j],board)<<"=identity"<<endl;
                if (rectangle[j].getGlobalBounds().contains(pos.x, pos.y))
                {
                  // if(n==8 && j==16 && board[n]==-6){
                  isMove = box.identifier(n, j, board[n], board); //<<"=identity"<<endl;
                  secondpos = rectangle[j].getPosition();
                  int spritepos = spritepositions[n];
                  // cout<<"shaheer"<<endl;
                  if (isMove && turn % 2 == playerId % 2)
                  {
                    turn++;
                    needUpdate = 1;

                    // ofstream out, out2, out3;
                    // out.open("spritepositions.txt");
                    // out2.open("boardpositions.txt");
                    // out3.open("qpositions.txt");
                    // for (int i = 0; i < 64; i++)
                    // {
                    //   out << spritepositions[i] << ",";
                    //   out2 << board[i] << ",";
                    //   out3 << q[i] << ",";
                    // }
                    // out.close();
                    // out2.close();
                    // out3.close();

                    cc = q[j] = spritepositions[j];
                    if (j != n)
                    {
                      sprite[spritepos].setPosition(secondpos);
                      sprite[cc].setPosition(100000000, 100000000);
                      int suppos = spritepositions[j];
                      spritepositions[j] = spritepositions[n];
                      spritepositions[n] = 64;
                      if (board[j] == -5 || board[j] == 5)
                      {
                        save s;
                        window.close();
                        if (s.smain())
                        {
                          // ofstream out, out2, out3;
                          // out.open("spritepositions.txt");
                          // out2.open("boardpositions.txt");
                          // out3.open("qpositions.txt");
                          // for (int i = 0; i < 64; i++)
                          // {
                          //   out << spritepositions[i] << ",";
                          //   out2 << board[i] << ",";
                          //   out3 << q[i] << ",";
                          // }
                          // out.close();
                          // out2.close();
                          // out3.close();
                        }
                      }
                      if (j <= 63 && j >= 56 && board[n] == -6)
                      {
                        board[j] = -4;
                      }
                      else if (j >= 0 && j <= 7 && board[n] == 6)
                      {
                        board[j] = 4;
                      }
                      else
                      {
                        board[j] = board[n];
                        board[n] = 0;
                      }
                      n = j;
                    }
                  }
                  int counter = 0;
                  for (int i = 0; i < 8; ++i)
                  {
                    for (int j = 0; j < 8; ++j)
                    {
                      if ((i + j) % 2 == 0)
                        rectangle[counter].setFillColor(paleWhite);
                      else
                        rectangle[counter].setFillColor(greenish);
                      counter++;
                    }
                  }
                }
              }
              cap = 0;
            }
        }
      }

      if (needUpdate == 1)
      {

        parsing_json = json_object_new_object();

        /*Creating a json string*/
        jstatus = json_object_new_string("connected");

        /*Creating a json integer*/
        jturn = json_object_new_int(turn);
        jplayerId = json_object_new_int(playerId);

        /*Creating a json boolean*/
        jneedUpdate = json_object_new_int(needUpdate);
        jisGameOver = json_object_new_int(0);

        /*Creating a json array*/
        jboard = json_object_new_array();
        jspritePositions = json_object_new_array();

        /*Creating json strings*/
        for (int i = 0; i < 64; i++)
        {
          jpos = json_object_new_int(board[i]);
          json_object_array_add(jboard, jpos);

          jpos = json_object_new_int(spritepositions[i]);
          json_object_array_add(jspritePositions, jpos);
        }

        /*Form the json object*/
        /*Each of these is like a key value pair*/
        json_object_object_add(parsing_json, "turn", jturn);
        json_object_object_add(parsing_json, "playerId", jplayerId);
        json_object_object_add(parsing_json, "board", jboard);
        json_object_object_add(parsing_json, "spritePositions", jspritePositions);
        json_object_object_add(parsing_json, "status", jstatus);
        json_object_object_add(parsing_json, "needUpdate", jneedUpdate);
        json_object_object_add(parsing_json, "isGameOver", jisGameOver);

        /* Now printing the json object in the file */
        json_object_to_file("send.json", parsing_json);
        json_object_to_file("recv.json", parsing_json);

        needUpdate = 0;
      }

      window.clear();
      for (int j = 0; j < 64; ++j)
        window.draw(rectangle[j]);
      for (int j = 0; j < 65; j++)
      {
        if (q[j] == 64)
          window.draw(sprite[j]);
      }

      window.display();
    }
  }
