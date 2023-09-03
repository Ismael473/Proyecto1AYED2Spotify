#include <SFML/Graphics.hpp>
#include "MenuPrincipal.h"


int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Spotify");

    MenuPrincipal menuPrincipal(window.getSize().x, window.getSize().y);
    
    sf::CircleShape shape(20);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch(event.type){
                case Event::Closed:
                window.close();
                break;
            }
        }

        window.clear();
        MenuPrincipal.draw(window);
        window.display();
    
    }
    return 0;
}