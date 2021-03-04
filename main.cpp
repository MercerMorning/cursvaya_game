///////////////////////Урок 8////////////////////////////////////

#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include "map.h"
#include "view.h"
#include "mission.h"
#include <sstream>


using namespace sf;
using namespace std;


////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player { // класс Игрока
private: float x, y;//объявили переменные, в конструкторе Player ниже дадим им значения
public:
	float w, h, dx, dy, speed = 0; //координаты игрока х и у, высота ширина, ускорение (по х и по у), сама скорость
	int playerScore, health = 40; //направление (direction) движения игрока
	bool life = true, onGround, isMove = false;
	enum StateObject { left, right, up, down, jump, stay} state;
	std::string  File; //файл с расширением
	Image image;//сфмл изображение
	Texture texture;//сфмл текстура
	Sprite sprite;//сфмл спрайт

	Player(std::string F, float X, float Y, float W, float H){  //Конструктор с параметрами(формальными) для класса Player. При создании объекта класса мы будем задавать имя файла, координату Х и У, ширину и высоту
		File = F;//имя файла+расширение
		w = W; h = H;//высота и ширина
		image.loadFromFile("images/" + File);//запихиваем в image наше изображение вместо File мы передадим то, что пропишем при создании объекта. В нашем случае "hero.png" и получится запись идентичная 	image.loadFromFile("images/hero/png");
		image.createMaskFromColor(Color(41, 33, 59));//убираем ненужный темно-синий цвет, эта тень мне показалась не красивой.
		texture.loadFromImage(image);//закидываем наше изображение в текстуру
		sprite.setTexture(texture);//заливаем спрайт текстурой
		x = X; y = Y;//координата появления спрайта
		sprite.setTextureRect(IntRect(0, 0, w, h));  //Задаем спрайту один прямоугольник для вывода одного льва, а не кучи львов сразу. IntRect - приведение типов
	}



	void update(float time) //функция "оживления" объекта класса. update - обновление. принимает в себя время SFML , вследствие чего работает бесконечно, давая персонажу движение.
	{
	    control();
		switch (state)//реализуем поведение в зависимости от направления. (каждая цифра соответствует направлению)
		{
            case right: dx = speed;break;//состояние идти вправо
            case left: dx = -speed;break;//состояние идти влево
            case up: break;//будет состояние поднятия наверх (например по лестнице)
            case down: dx=0;break;//будет состояние во время спуска персонажа (например по лестнице)
            case jump: break;//здесь может быть вызов анимации
            case stay: break;//и здесь тоже
		}

		x += dx*time;
		checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
		y += dy*time;
		checkCollisionWithMap(0, dy);//обрабатываем столкновение по Y
		if (!isMove) speed = 0;
		sprite.setPosition(x+w/2, y+h/2); //задаем позицию спрайта в место его центра
		if (health <= 0){life = false;}
		dy = dy + 0.0015*time;


		/*x += dx*time;//то движение из прошлого урока. наше ускорение на время получаем смещение координат и как следствие движение
		y += dy*time;//аналогично по игреку

		speed = 0;//зануляем скорость, чтобы персонаж остановился.
		interactionWithMap();
		sprite.setPosition(x,y); //выводим спрайт в позицию x y , посередине. бесконечно выводим в этой функции, иначе бы наш спрайт стоял на месте.
        if (health <= 0){ life = false; speed = 0; }//если жизней меньше либо равно 0, то умираем и исключаем движение игрока после смерти*/
	}

    void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
	{
		for (int i = y / 32; i < (y + h) / 32; i++)//проходимся по элементам карты
		for (int j = x / 32; j<(x + w) / 32; j++)
		{
			if (TileMap[i][j] == '0')//если элемент наш тайлик земли? то
			{
				if (Dy>0){ y = i * 32 - h;  dy = 1; onGround = true; }//по Y вниз=>идем в пол(стоим на месте) или падаем. В этот момент надо вытолкнуть персонажа и поставить его на землю, при этом говорим что мы на земле тем самым снова можем прыгать
				if (Dy<0){y = i * 32 + 32;  dy = 0;}//столкновение с верхними краями карты(может и не пригодиться)
				if (Dx>0){x = j * 32 - w;}//с правым краем карты
				if (Dx<0){x = j * 32 + 32;}// с левым краем карты
			} else {onGround=false;}
		}
	}

	void control(){
            if (Keyboard::isKeyPressed(Keyboard::Left)) {
                state = left;
                speed = 0.1;
            }

            if (Keyboard::isKeyPressed(Keyboard::Right)) {
                state = right;
                speed = 0.1;
            }

            if (Keyboard::isKeyPressed(Keyboard::Up)) {
                state = jump;
                dy = -0.4; onGround = false;
            }

            if (Keyboard::isKeyPressed(Keyboard::Down)) {
                 state = down;
                speed = 0.1;

            }
	}

void interactionWithMap()//ф-ция взаимодействия с картой
	{

			for (int i = y / 32; i < (y + h) / 32; i++)//проходимся по тайликам, контактирующим с игроком, то есть по всем квадратикам размера 32*32, которые мы окрашивали в 9 уроке. про условия читайте ниже.
			for (int j = x / 32; j<(x + w) / 32; j++)//икс делим на 32, тем самым получаем левый квадратик, с которым персонаж соприкасается. (он ведь больше размера 32*32, поэтому может одновременно стоять на нескольких квадратах). А j<(x + w) / 32 - условие ограничения координат по иксу. то есть координата самого правого квадрата, который соприкасается с персонажем. таким образом идем в цикле слева направо по иксу, проходя по от левого квадрата (соприкасающегося с героем), до правого квадрата (соприкасающегося с героем)
			{
				if (TileMap[i][j] == '0')//если наш квадратик соответствует символу 0 (стена), то проверяем "направление скорости" персонажа:
				{
					if (dy>0)//если мы шли вниз,
					{
						y = i * 32 - h;//то стопорим координату игрек персонажа. сначала получаем координату нашего квадратика на карте(стены) и затем вычитаем из высоты спрайта персонажа.
					}
					if (dy<0)
					{
						y = i * 32 + 32;//аналогично с ходьбой вверх. dy<0, значит мы идем вверх (вспоминаем координаты паинта)
					}
					if (dx>0)
					{
						x = j * 32 - w;//если идем вправо, то координата Х равна стена (символ 0) минус ширина персонажа
					}
					if (dx < 0)
					{
						x = j * 32 + 32;//аналогично идем влево
					}
				}

				if (TileMap[i][j] == 's') { //если символ равен 's' (камень)
                    playerScore++;//если взяли камень, переменная playerScore=playerScore+1;
                    TileMap[i][j] = ' ';
				}
				if (TileMap[i][j] == 'f') {
                    health-=40;//если взяли ядовитейший в мире цветок,то переменная health=health-40;
                    TileMap[i][j] = ' ';//убрали цветок
                }

                if (TileMap[i][j] == 'h') {
                    health += 20;//если взяли сердечко,то переменная health=health+20;
                    TileMap[i][j] = ' ';//убрали сердечко
                }
                }
        }
	float getplayercoordinateX(){	//этим методом будем забирать координату Х
		return x;
	}
	float getplayercoordinateY(){	//этим методом будем забирать координату Y
		return y;
	}
};


int main()
{
    randomMapGenerate();
    bool showMissionText = true;//логическая переменная, отвечающая за появление текста миссии на экране

	Texture quest_texture;
	Sprite s_quest;
	s_quest.setTexture(quest_texture);
	s_quest.setTextureRect(IntRect(0, 0, 340, 510));  //приведение типов, размеры картинки исходные
	s_quest.setScale(0.6f, 0.6f);//чуть уменьшили картинку, => размер стал меньше

    Font font;//шрифт
     font.loadFromFile("CyrilicOld.TTF");//передаем нашему шрифту файл шрифта
     Text text("", font, 20);//создаем объект текст. закидываем в объект текст строку, шрифт, размер шрифта(в пикселях);//сам объект текст (не строка)
     text.setColor(Color::Red);//покрасили текст в красный. если убрать эту строку, то по умолчанию он белый
     text.setStyle(sf::Text::Bold | sf::Text::Underlined);//жирный и подчеркнутый текст. по умолчанию он "худой":)) и не подчеркнутый

	RenderWindow window(sf::VideoMode(640, 480), "Lesson 8. kychka-pc.ru");

	view.reset(sf::FloatRect(0, 0, 640, 480));

	float CurrentFrame = 0;//хранит текущий кадр
	Clock clock;
    Clock gameTimeClock;//переменная игрового времени, будем здесь хранить время игры
    int gameTime = 0;//объявили игровое время, инициализировали.

	Player p("hero.png",250,250,96.0, 96.0);//создаем объект p класса player,задаем "hero.png" как имя файла+расширение, далее координата Х,У, ширина, высота.

	while (window.isOpen())
	{

     float time = clock.getElapsedTime().asMicroseconds();

     if (p.life) {
            getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());
            gameTime=gameTimeClock.getElapsedTime().asSeconds();//игровое время в секундах идёт вперед, пока жив игрок, перезагружать как time его не надо. оно не обновляет логику игры
     }
      else { view.rotate(0.01); }//игровое время в секундах идёт вперед, пока жив игрок, перезагружать как time его не надо. оно не обновляет логику игры . если игрок умер - вращаем камеру


     clock.restart();
     time = time / 800;


		sf::Event event;
		while (window.pollEvent(event))
         {
         if (event.type == sf::Event::Closed)
         window.close();

         if (event.type == Event::KeyPressed)//событие нажатия клавиши
         if ((event.key.code == Keyboard::Tab)) {//если клавиша ТАБ

             switch (showMissionText) {//переключатель, реагирующий на логическую переменную showMissionText

                 case true: {
                     std::ostringstream playerHealthString;//строка здоровья игрока
                     playerHealthString << p.health; //заносим в строку здоровье
                     std::ostringstream task;//строка текста миссии
                     task << getTextMission(getCurrentMission(p.getplayercoordinateX()));//вызывается функция getTextMission (она возвращает текст миссии), которая принимает в качестве аргумента функцию getCurrentMission(возвращающую номер миссии), а уже эта ф-ция принимает в качестве аргумента функцию p.getplayercoordinateX() (эта ф-ция возвращает Икс координату игрока)
                     text.setString("Health: " + playerHealthString.str()+"\n" + task.str());//задаем
                     text.setPosition(view.getCenter().x + 125, view.getCenter().y - 130);//позиция всего этого текстового блока
                     s_quest.setPosition(view.getCenter().x + 115, view.getCenter().y - 130);//позиция фона для блока
                     showMissionText = false;//эта строка позволяет убрать все что мы вывели на экране
                     break;//выходим , чтобы не выполнить условие "false" (которое ниже)
                 }
                 case false: {
                     text.setString("");//если не нажата клавиша таб, то весь этот текст пустой
                     showMissionText = true;// а эта строка позволяет снова нажать клавишу таб и получить вывод на экран
                     break;
                 }
             }
         }
         }



		///////////////////////////////////////////Управление персонажем с анимацией////////////////////////////////////////////////////////////////////////
		if (p.life) {
            /*if (Keyboard::isKeyPressed(Keyboard::Left)) {
                p.dir = 1; p.speed = 0.1;
                CurrentFrame += 0.005*time;
                if (CurrentFrame > 3) CurrentFrame -= 3;
                p.sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 96, 96, 96));
                getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());//передаем координаты игрока в функцию управления камерой
            }

            if (Keyboard::isKeyPressed(Keyboard::Right)) {
                p.dir = 0; p.speed = 0.1;
                CurrentFrame += 0.005*time;
                if (CurrentFrame > 3) CurrentFrame -= 3;
                p.sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 192, 96, 96));
                getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());//передаем координаты игрока в функцию управления камерой
            }

            if (Keyboard::isKeyPressed(Keyboard::Up)) {
                p.dir = 3; p.speed = 0.1;
                CurrentFrame += 0.005*time;
                if (CurrentFrame > 3) CurrentFrame -= 3;
                p.sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 288, 96, 96));
                getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());//передаем координаты игрока в функцию управления камерой

            }

            if (Keyboard::isKeyPressed(Keyboard::Down)) {
                p.dir = 2; p.speed = 0.1;
                CurrentFrame += 0.005*time;
                if (CurrentFrame > 3) CurrentFrame -= 3;
                p.sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 0, 96, 96));
                getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());//передаем координаты игрока в функцию управления камерой

            }*/
}
		p.update(time);//оживляем объект p класса Player с помощью времени sfml, передавая время в качестве параметра функции update. благодаря этому персонаж может двигаться

		changeview();
viewmap(time);

    Image map_image;//объект изображения для карты
	map_image.loadFromFile("images/map.png");//загружаем файл для карты
	Texture map;//текстура карты
	map.loadFromImage(map_image);//заряжаем текстуру картинкой
	Sprite s_map;//создаём спрайт для карты
	s_map.setTexture(map);//заливаем текстуру спрайтом
    window.setView(view);
	window.clear();

    if ((getCurrentMission(p.getplayercoordinateX())) == 0) { //Если текущая миссия 0, то рисуем карту вот так
			for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32));
				if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(64, 0, 32, 32));
				if ((TileMap[i][j] == 'f')) s_map.setTextureRect(IntRect(96, 0, 32, 32));
				if ((TileMap[i][j] == 'h')) s_map.setTextureRect(IntRect(128, 0, 32, 32));
				s_map.setPosition(j * 32, i * 32);

				window.draw(s_map);
			}
		}

		if ((getCurrentMission(p.getplayercoordinateX())) >= 1) { //Если текущая миссия 1, то рисуем карту вот так
			for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(64, 0, 32, 32));//для примера поменял местами вывод спрайта для этого символа и..
				if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(0, 0, 32, 32));//и для вот этого. логически-игровой смысл их остался таким же
				if ((TileMap[i][j] == 'f')) s_map.setTextureRect(IntRect(96, 0, 32, 32));
				if ((TileMap[i][j] == 'h')) s_map.setTextureRect(IntRect(128, 0, 32, 32));
				s_map.setPosition(j * 32, i * 32);

				window.draw(s_map);
			}
		}



		//window.draw(s_map);
        /*
        std::ostringstream playerHealthString,gameTimeString;    // объявили переменную здоровья и времени
		playerHealthString << p.health; gameTimeString << gameTime;		//формируем строку
        text.setString("Health: " + playerHealthString.str()+"\nTime: "+gameTimeString.str());//задаем строку тексту и вызываем сформированную выше строку методом .str()
		text.setPosition(view.getCenter().x - 165, view.getCenter().y - 200);//задаем позицию текста, отступая от центра камеры
		window.draw(text);//рисую этот текст
        */

		window.draw(p.sprite);//рисуем спрайт объекта p класса player
		if (!showMissionText) {
            text.setPosition(view.getCenter().x + 125, view.getCenter().y - 130);//позиция всего этого текстового блока
            s_quest.setPosition(view.getCenter().x + 115, view.getCenter().y - 130);//позиция фона для блока
            window.draw(text); //рисуем спрайт свитка (фон для текста миссии). а затем и текст. все это завязано на логическую переменную, которая меняет свое состояние от нажатия клавиши ТАБ
        }
		window.display();
	}

	return 0;
}
