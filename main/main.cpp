#include <SFML/Graphics.hpp>
#include <iostream>
//#include <Menu.h> 


int main(){

    sf::RenderWindow window(sf::VideoMode(800,600),"Odissey Radio");
    sf::CircleShape shape(100);
    shape.setFillColor(sf::Color::Green);

    while(window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)){

            if(event.type == sf::Event::Closed){
                window.close();
            }


        }
        window.clear();
        window.draw(shape);
        window.display();
    }
    return 0;
}