#include "MenuPrincipal.h"

using namespace std;
using namespace sf;

MenuPrincipal::MenuPrincipal(float ancho, float largo){
    if(!font.loadFromFile("arial.ttf")){
        //maneja el error
    }

    menuPrincipal[0].setFont(font);
    menuPrincipal[0].setColor(Color::Green);
    menuPrincipal[0].setString("Reproducir");
    menuPrincipal[0].setPosition(Vector2f(ancho / 2, largo / (MAX_NUMBER_ITEMS + 1)*1));


    menuPrincipal[1].setFont(font);
    menuPrincipal[1].setColor(Color::Green);
    menuPrincipal[1].setString("Retroceder");
    menuPrincipal[1].setPosition(Vector2f(ancho / 2, largo / (MAX_NUMBER_ITEMS + 1)*2));

    menuPrincipal[2].setFont(font);
    menuPrincipal[2].setColor(Color::Green);
    menuPrincipal[2].setString("Avanzar");
    menuPrincipal[2].setPosition(Vector2f(ancho / 2, largo / (MAX_NUMBER_ITEMS + 1)*3));

}

MenuPrincipal::~MenuPrincipal(){

}
void MenuPrincipal::draw(RenderWindow &window){
    for(int i = 0; i < MAX_NUMBER_ITEMS; i++){
        window.draw(menuPrincipal(i));
    }
}