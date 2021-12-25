#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "ChessBoard.h"
using namespace sf;
const float w = 650;
const float h = 650;
class play
{
private:
  sf::Texture texture[3];
  sf::Sprite sprite[3];
  sf::RectangleShape rectangle;

public:
  play(int playerId, json_object *parsed_json)
  {
    json_object *jturn;
    json_object *jpos;
    json_object *jboard;
    json_object *jspritePositions;
    json_object *jneedUpdate;

    json_object_object_get_ex(parsed_json, "board", &jboard);
    json_object_object_get_ex(parsed_json, "spritePositions", &jspritePositions);
    json_object_object_get_ex(parsed_json, "turn", &jturn);

    // printf("My player id is : %d\n", playerId);
    sf::RenderWindow window(sf::VideoMode(w, h), "Chess");

    texture[0].loadFromFile("Chess/images/play-button.png");
    texture[2].loadFromFile("Chess/images/board.png");

    rectangle.setSize(sf::Vector2f(w, h));
    sprite[0].setTexture(texture[0]);
    sprite[2].setTexture(texture[2]);
    sprite[0].setScale(2.0f, 2.0f);
    sprite[2].setScale(0.73f, 0.73f);
    sprite[0].setPosition(200, 200);
    sprite[2].setPosition(0, 0);
    rectangle.setFillColor(sf::Color::White);

    while (window.isOpen())
    {
      Vector2i pos = Mouse::getPosition(window);
      sf::Event event;
      while (window.pollEvent(event))
      {
        if (event.type == sf::Event::Closed)
        {
          window.close();
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right) || sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
          if (sprite[0].getGlobalBounds().contains(pos.x, pos.y))
          {
            for (int i = 0; i < 64; i++)
            {
              jpos = json_object_array_get_idx(jboard, i);
              board[i] = json_object_get_int(jpos);

              jpos = json_object_array_get_idx(jspritePositions, i);
              spritepositions[i] = json_object_get_int(jpos);
            }
            jneedUpdate = json_object_new_int(0);
            json_object_object_add(parsed_json, "needUpdate", jneedUpdate);
            json_object_to_file("send.json", parsed_json);  
            json_object_to_file("recv.json", parsed_json);
            printf("Play.h : This is the json: \n %s \n", json_object_to_json_string(parsed_json));
            window.close();
            ChessBoard graphics;
            graphics.MainFunctions(1, playerId);
          }
        }
      }

      window.clear();
      window.draw(rectangle);
      window.draw(sprite[2]);
      window.draw(sprite[0]);
      window.display();
    }
  }
};