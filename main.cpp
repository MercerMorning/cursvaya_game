#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;

class Player {
public:
    float x, y, w, h, dx, dy, speed = 0;
    int dir = 0;
    String File;
    Image image;
    Texture texture;
    Sprite sprite;
    Player(String F){
     File = F;
     //image.loadFromFile("images/" + File);
     image.loadFromFile("images/" + 'hero.png');
     image.createMaskFromColor(Color(41, 33, 59));
     texture.loadFromImage(image);
     sprite.setTexture(texture);
    }
    void update (float time)
    {
        switch(dir)
        {
            case 0: dx = speed; dy = 0; break;
            case 1: dx = -speed; dy = 0; break;
            case 2: dx = 0; dy = speed; break;
            case 3: dx = 0; dy = -speed; break;
        }

        x += dx*time;
        y += dy*time;

        speed = 0;
        sprite.setPosition(x, y);
    }
};

int main()
{
std::cout << 'hi';
    RenderWindow window(VideoMode(500, 500), "SFML works!");

    Player p("hero.png");

    //float currentFlame = 0;

    Clock clock;

    CircleShape shape(100.f);
    shape.setFillColor(Color::Green);

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asMicroseconds();
        //clock.restart();
        //time = time/800;
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        //double speed = 0.1;

        if (Keyboard::isKeyPressed(Keyboard::Q)) { p.speed = 0.9; p.sprite.setColor(Color::Red); } else {
            p.sprite.setColor(Color::White);
        }

        if (Keyboard::isKeyPressed(Keyboard::Left)) { p.dir = 0; p.speed = 0.1; p.sprite.setScale(-1, 1); }
        if (Keyboard::isKeyPressed(Keyboard::Right)) { p.dir = 1; p.speed = 0.1; p.sprite.setScale(1, 1); }
        if (Keyboard::isKeyPressed(Keyboard::Up)) { p.dir = 3; p.speed = 0.1; }
        if (Keyboard::isKeyPressed(Keyboard::Down)) { p.dir = 4; p.speed = 0.1; }

        p.update(time);

        window.clear();
        window.draw(p.sprite);
        //window.draw(shape);
        window.display();
    }

    return 0;
}
