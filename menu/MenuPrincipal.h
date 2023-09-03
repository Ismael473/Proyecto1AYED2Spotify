#pragma once
#include <SFML/Graphics.hpp>


#define MAX_NUMBER_ITEMS 3

using namespace std;
using namespace sf;

class MenuPrincipal{

    public:
        MenuPrincipal(float ancho, float largo);
        ~MenuPrincipal();

        void draw(RenderWindow &window);
        void Play();
        void Retroceder();
        void Avanzar();

    private:
        int selectedItemIndex;
        Font font;
        Text menuPrincipal[MAX_NUMBER_ITEMS];





};