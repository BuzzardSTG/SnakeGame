// g++ -o MySnakeGame SnakeGame.cpp -I /opt/homebrew/Cellar/sfml/2.6.1/include -L /opt/homebrew/Cellar/sfml/2.6.1/lib -lsfml-graphics -lsfml-window -lsfml-system
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <random>
#include <unistd.h>

using namespace std;

bool gameOver;
const int maxDifficulty = 4;
const int difficultySpeeds[] = {1, 1.5, 2, 3};
const int difficultySizes[] = {10, 25, 50, 100};
int gridSize = 20; // Size of each grid cell
int width, height;
int x, y, score;
int fruitX, fruitY;
int tailX[100], tailY[100];
int nTail;
enum eDirection
{
    STOP = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN
};
eDirection dir;

sf::RenderWindow window(sf::VideoMode(800, 800), "Snake Game");

int getDifficulty()
{
    int choice;
    cout << "Choose Difficulty:" << endl;
    cout << "1. Easy" << endl;
    cout << "2. Medium" << endl;
    cout << "3. Hard" << endl;
    cout << "4. KAMISAMA" << endl;
    cin >> choice;
    return choice;
}

void Setup(int difficulty)
{
    gameOver = false;
    dir = STOP;
    width = difficultySizes[difficulty - 1];
    height = difficultySizes[difficulty - 1];
    x = width / 2;
    y = height / 2;
    score = 0;
    nTail = 0;

    // Seed for random number generation
    srand(time(nullptr));

    // Initialize fruit coordinates
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> fruitDistX(0, width - 1);
    uniform_int_distribution<int> fruitDistY(0, height - 1);

    fruitX = fruitDistX(gen);
    fruitY = fruitDistY(gen);
}

void Draw()
{
    window.clear();

    // Set up the view to simulate the grid
    sf::View view(sf::FloatRect(0, 0, width * gridSize, height * gridSize));
    window.setView(view);

    // Draw snake head
    sf::CircleShape snakeHead(gridSize / 2);
    snakeHead.setFillColor(sf::Color::Cyan);
    snakeHead.setPosition(x * gridSize, y * gridSize);
    window.draw(snakeHead);

    // Draw fruit
    sf::RectangleShape fruit(sf::Vector2f(gridSize, gridSize));
    fruit.setFillColor(sf::Color::Yellow);
    fruit.setPosition(fruitX * gridSize, fruitY * gridSize);
    window.draw(fruit);

    // Draw snake tail
    sf::CircleShape snakeTail(gridSize / 2);
    for (int i = 0; i < nTail; i++)
    {
        snakeTail.setFillColor(sf::Color::Green);
        snakeTail.setPosition(tailX[i] * gridSize, tailY[i] * gridSize);
        window.draw(snakeTail);
    }

    // Draw border around the game area
    sf::RectangleShape border(sf::Vector2f(width * gridSize, height * gridSize));
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(1);
    border.setOutlineColor(sf::Color::Black);
    border.setPosition(0, 0);
    window.draw(border);

    window.display();
}

void Input()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
        else if (event.type == sf::Event::KeyPressed)
        {
            // Check if the new direction is not opposite to the current direction
            switch (event.key.code)
            {
            case sf::Keyboard::A:
                if (dir != RIGHT)
                    dir = LEFT;
                break;
            case sf::Keyboard::D:
                if (dir != LEFT)
                    dir = RIGHT;
                break;
            case sf::Keyboard::W:
                if (dir != DOWN)
                    dir = UP;
                break;
            case sf::Keyboard::S:
                if (dir != UP)
                    dir = DOWN;
                break;
            case sf::Keyboard::X:
                gameOver = true;
                break;
            }
        }
    }
}

void Algorithm()
{
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;

    for (int i = 1; i < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir)
    {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }

    // Check for collisions with walls
    if (x < 0 || x >= width || y < 0 || y >= height)
        gameOver = true;

    for (int i = 0; i < nTail; i++)
    {
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;
    }

    if (x == fruitX && y == fruitY)
    {
        score += 10;

        // Generate new random coordinates for the fruit
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> fruitDistX(0, width - 1);
        uniform_int_distribution<int> fruitDistY(0, height - 1);

        fruitX = fruitDistX(gen);
        fruitY = fruitDistY(gen);

        nTail++;
        tailX[nTail - 1] = -1; // Placeholder value, will be updated in the next iteration
        tailY[nTail - 1] = -1; // Placeholder value, will be updated in the next iteration
    }
}

#include <SFML/System.hpp> // Include SFML/System.hpp for threading

void DrawGameOver()
{
    // Set up a new view for the game over message
    sf::View gameOverView(sf::FloatRect(0, 0, width * gridSize, height * gridSize));
    window.setView(gameOverView);

    // Create a text object for the game over message
    sf::Font font;
    if (!font.loadFromFile("/Users/buzzardstg/Desktop/Lectures/Fall 2023/Computer Programming (C++)(e)/Final_Project_SnakeGame/montserrat/Montserrat-SemiBoldItalic.otf"))
    {
        cerr << "Error loading font" << endl;
        cerr << "Font file path: /Users/buzzardstg/Desktop/Lectures/Fall 2023/Computer Programming (C++)(e)/Final_Project_SnakeGame/montserrat/Montserrat-SemiBoldItalic.otf" << endl;
        return;
    }

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Game Over! Your score: " + to_string(score));

    // Calculate font size based on the grid size
    int calculatedFontSize = gridSize / 2;
    gameOverText.setCharacterSize(calculatedFontSize);

    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setStyle(sf::Text::Bold);

    // Center the text on the screen
    sf::FloatRect textBounds = gameOverText.getLocalBounds();
    gameOverText.setPosition((width * gridSize - textBounds.width) / 2, (height * gridSize - textBounds.height) / 2);

    // Draw the game over message
    window.draw(gameOverText);

    // Display the updated window
    window.display();

    // Display the current directory
    char buffer[256];
    if (getcwd(buffer, sizeof(buffer)) != NULL)
    {
        cout << "Current Directory: " << buffer << endl;
    }
    else
    {
        cerr << "Error getting current directory" << endl;
    }

    // Sleep for 3 seconds
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

int main()
{
    int difficulty = getDifficulty();
    Setup(difficulty);

    while (window.isOpen() && !gameOver)
    {
        Input();
        Algorithm();
        Draw();

        // Adjust sleep duration based on difficulty
        int sleepDuration = 1000 / (difficultySpeeds[difficulty - 1] * 10);
        if (difficulty == 1)
        {
            // Make difficulty 1 slower
            sleepDuration = 150;
        }

        this_thread::sleep_for(chrono::milliseconds(sleepDuration));
    }

    DrawGameOver();

    return 0;
}