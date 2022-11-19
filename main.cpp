#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

sf::Music menumusic;
sf::Music gamemusic;

sf::SoundBuffer sbuffer;
sf::Sound sjump;

sf::Font font;
sf::Text showScore;
sf::Text stostart;
int score = 0;

//Game States
#define SPLASH 0
#define MENU 1
#define GAME 2
#define END_GAME 3
int state = -1;

struct point
{ int x,y;};

class CScore {
    public:
    std::vector<int> vhiscores;

    void Read()
    {
        std::ifstream in("hiscores.dat");
        if(in.good())
        {
            std::string str;
            getline(in,str);
            std::stringstream ss(str);
            int n;
            for(int i=0;i<5;i++)
            {
                ss >> n;
                vhiscores.push_back(n);
            }
            in.close();
        }
        else
        {
            //if file does not exist fill with 5 scores
            for(int i=0;i<5;i++)
            {
                vhiscores.push_back(0);
            }
        }
    }

    void Update(int newscore)
    {
        //new score to the end
        vhiscores.push_back(newscore);
        //sort
        sort(vhiscores.rbegin(), vhiscores.rend());
        //remove the last
        vhiscores.pop_back();
    }

    void Write()
    {
        std::ofstream out("hiscores.dat");
        for(int i=0;i<5;i++)
        {
            out << vhiscores[i] << " ";
        }
        out.close();
    }

    void Show(sf::RenderWindow &app)
    {
        sf::Text showHiScores;
        showHiScores.setFont(font);
        showHiScores.setCharacterSize(20);
        showHiScores.setPosition(120.f, 110.f);
        showHiScores.setFillColor(sf::Color::Black);
        std::string histr="HiScores\n";
        for(int i=0;i<5;i++)
        {
            histr = histr + "    " + std::to_string(vhiscores[i]) + "\n";
        }
        showHiScores.setString(histr);
        app.draw(showHiScores);
    }
};

int main()
{
    srand(time(0));

    sf::RenderWindow app(sf::VideoMode(400, 533), "Doodle Game!");
    app.setFramerateLimit(60);

    sf::Texture t1,t2,t3,t4,t5,t6;
    t1.loadFromFile("images/background.png");
    t2.loadFromFile("images/platform.png");
    t3.loadFromFile("images/doodle.png");
    t4.loadFromFile("images/splash.png");
    t5.loadFromFile("images/gameover.png");
    t6.loadFromFile("images/menu.png");

    sf::Sprite sBackground(t1), sPlat(t2), sPers(t3);
    sf::Sprite sSplash(t4), sGameover(t5), sMenu(t6);

    // load music
	if( !menumusic.openFromFile("sound/Circus_Marcus_-_04_-_Le_petit_menuet_de_Bach.ogg"))
        return -1;
    menumusic.setLoop(true);

    if( !gamemusic.openFromFile("sound/Hobotek_-_05_-_Back_to_Attack_Analogue_Action.ogg"))
        return -1;
    gamemusic.setLoop(true);

    // load sound in buffer
    if( !sbuffer.loadFromFile("sound/smb_jump-small.wav")) return -1;
    // prepare sound to play
    sjump.setBuffer(sbuffer);

    // Load the text font
    font.loadFromFile("sansation.ttf");
    showScore.setFont(font);
    showScore.setCharacterSize(20);
    showScore.setPosition(240.f, 20.f);
    showScore.setFillColor(sf::Color::Black);
    stostart.setFont(font);
    stostart.setCharacterSize(20);
    stostart.setPosition(100.f, 250.f);
    stostart.setFillColor(sf::Color::Black);
    stostart.setString("Press S to start");

    CScore cscore;

	cscore.Read();

    point plat[20];

    //if the doodle reach height 200
    //then move platforms down.
	int x=100,y=100,h=200;
    float dy=0;

    state = SPLASH;

    while (app.isOpen())
    {
        if( state == SPLASH )
        {
            sf::Event event;
            while (app.pollEvent(event))
            {
                if ((event.type == sf::Event::Closed) ||
                    ((event.type == sf::Event::KeyPressed)
                     && (event.key.code == sf::Keyboard::Escape)))
                    app.close();

                // Space key pressed: change state to MENU
                if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
                {
                    state = MENU;
                    menumusic.play();
                }
            }
        }

        if( state == MENU )
        {
            sf::Event event;
            while (app.pollEvent(event))
            {
                if ((event.type == sf::Event::Closed) ||
                    ((event.type == sf::Event::KeyPressed)
                     && (event.key.code == sf::Keyboard::Escape)))
                    app.close();

                // S key pressed: change state to GAME
                if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::S))
                {
                    state = GAME;
                    score = 0;
                    //create 10 random platforms
                    for (int i=0;i<10;i++)
                    {
                       plat[i].x=rand()%400;
                       plat[i].y=rand()%533;
                    }
                    x = 100, y = 100;
                    menumusic.stop();
                    gamemusic.play();
                }
            }
        }

        if( state == GAME )
        {
            sf::Event event;
            while (app.pollEvent(event))
            {
                if ((event.type == sf::Event::Closed) ||
                    ((event.type == sf::Event::KeyPressed)
                     && (event.key.code == sf::Keyboard::Escape)))
                    app.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) x+=3;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) x-=3;
        }

        if( state == END_GAME )
        {
            gamemusic.stop();
            sf::Event event;
            while (app.pollEvent(event))
            {
                if ((event.type == sf::Event::Closed) ||
                    ((event.type == sf::Event::KeyPressed)
                     && (event.key.code == sf::Keyboard::Escape)))
                    app.close();

                // Any key pressed: change state to MENU
                if (event.type == sf::Event::KeyPressed)
                {
                    state = MENU;
                    menumusic.play();
                }
            }
        }

        //game cycle
        if( state == GAME )
        {
            dy+=0.2; //gravity
            y+=dy;
            //if (y>500)  dy=-10; //if doodle touches floor , jump.
            if( y > 500 )
            {
                state = END_GAME;
                cscore.Update(score);
            }

            if (y<h)
            {
                score += h-y;
                for (int i=0;i<10;i++)
                {
                  y=h;
                  plat[i].y=plat[i].y-dy; //move down the platforms.
                  //if the platform reach the floor, create a new one.
                  if (plat[i].y>533) {plat[i].y=0; plat[i].x=rand()%400;}
                }
            }

            //if the doodle is over a platform then jump.
            for (int i=0;i<10;i++)
            {
                if ((x+50>plat[i].x) && (x+20<plat[i].x+68)
                && (y+70>plat[i].y) && (y+70<plat[i].y+14) && (dy>0))
                {
                    sjump.play();
                    dy=-10;
                }
            }

            sPers.setPosition(x,y);
        }

        // game paint
        if( state == SPLASH )
        {
            app.draw(sSplash);
        }

        if( state == MENU )
        {
            app.draw(sMenu);
            app.draw(stostart);
            cscore.Show(app);
        }

        if( state == GAME )
        {
            app.draw(sBackground);
            app.draw(sPers);
            for (int i=0;i<10;i++)
            {
                sPlat.setPosition(plat[i].x,plat[i].y);
                app.draw(sPlat);
            }

            std::string sc = std::to_string(score);
            showScore.setString(sc);
            app.draw(showScore);
        }

        if( state == END_GAME )
        {
            app.draw(sBackground);
            app.draw(sGameover);
        }

        app.display();
    }

    cscore.Write();

    return 0;
}
