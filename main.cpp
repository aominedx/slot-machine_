#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <string>
#include <memory>
#include <random>
#include <iostream>
#include <iomanip>
#include <sstream>
#include<unordered_map>

const int WINDOW_HEIGHT = 600;
const int WINDOW_WIDTH = 800;
const int STARTBUTTON_X = 150;
const int STARTBUTTON_Y = 250;
const int STOPBUTTON_X = 450;
const int STOPBUTTON_Y = 250;
std::vector<std::string> symbols = { "A","B","C","D","E","F" };
const int SPIN_COUNT = 5;
const int SYMBOL_COUNT = 5;
const float SPIN_DURATION = 5.0f;
const float MAX_SPIN_SPEED = 500.0f;  // Максимальная скорость вращения
const float ACCELERATION_TIME = 1.f; // Время разгона
const float DECELERATION_TIME = 1.f; // Время замедления

class Button {
public:
    Button(float x, float y, float width, float height, const std::string& text, sf::Font& font) {
        buttonShape.setSize(sf::Vector2f(width, height));
        buttonShape.setPosition(x, y);
        buttonShape.setFillColor(sf::Color(100, 100, 250));
        buttonShape.setOutlineThickness(2);
        buttonShape.setOutlineColor(sf::Color::White);

        buttonText.setFont(font);
        buttonText.setString(text);
        buttonText.setCharacterSize(24);
        buttonText.setFillColor(sf::Color::White);

        sf::FloatRect textBounds = buttonText.getLocalBounds();
        buttonText.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
        buttonText.setPosition(x + width / 2, y + height / 2);
    }

    bool isHovered(sf::Vector2f mousePos) {
        return buttonShape.getGlobalBounds().contains(mousePos);
    }

    bool isClicked(sf::Vector2f mousePos, bool isMousePressed) {
        return isHovered(mousePos) && isMousePressed;
    }

    void update(sf::Vector2f mousePos, bool isMousePressed) {
        if (isClicked(mousePos, isMousePressed)) {
            buttonShape.setFillColor(sf::Color(50, 50, 200));  // Цвет при нажатии
        }
        else if (isHovered(mousePos)) {
            buttonShape.setFillColor(sf::Color(150, 150, 255));  // Цвет при наведении
        }
        else {
            buttonShape.setFillColor(sf::Color(100, 100, 250));  // Исходный цвет
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(buttonShape);
        window.draw(buttonText);
    }

private:
    sf::RectangleShape buttonShape;
    sf::Text buttonText;
};

class SlotMachine;
class SpinningState;
class ShowingResultState;

class State
{
public:
    virtual ~State() = default;
    virtual void update(float dt) = 0;
    virtual void handleInput(sf::RenderWindow& mainWindow) = 0;
    virtual void render(sf::RenderWindow& mainWindow) = 0;
};

class WaitingState : public State
{
public:
    WaitingState(SlotMachine* machine) : machine(machine) {}
    void update(float dt) override {}
    void handleInput(sf::RenderWindow& mainWindow) override;
    void render(sf::RenderWindow& mainWindow) override;
private:
    SlotMachine* machine;
};
class SpinningState : public State
{
public:
    SpinningState(SlotMachine* machine);
    void update(float dt) override;
    void handleInput(sf::RenderWindow& mainWindow) override;
    void render(sf::RenderWindow& mainWindow) override;
private:
    SlotMachine* machine;
    std::vector<int> currentSymbols;
    std::vector<int> currentSymbols2;
    std::vector<int> currentSymbols3;
    float elapsedTime;
    float currentSpeed;
    bool stopPressed;
    float stopTimer;
    std::vector<float> reelOffsets;  
};
class ShowingResultState : public State
{
public:
    ShowingResultState(SlotMachine* machine, const std::vector<int>& symbols);
    ShowingResultState(SlotMachine* machine, const std::vector<int>& symbols1, const std::vector<int>& symbols2, const std::vector<int>& symbols3);
    void update(float dt) override {}
    void handleInput(sf::RenderWindow& mainWindow) override;
    void render(sf::RenderWindow& mainWindow) override;
    double resultScore();
    
private:
    SlotMachine* machine;
    std::vector<int> resultSymbols;
    std::vector<int> resultSymbols2;
    std::vector<int> resultSymbols3;
    double resScore = 0;
};

ShowingResultState::ShowingResultState(SlotMachine* machine, const std::vector<int>& symbols)
    : machine(machine), resultSymbols(symbols), resultSymbols2(symbols), resultSymbols3(symbols) {}
ShowingResultState::ShowingResultState(SlotMachine* machine, const std::vector<int>& symbols1, const std::vector<int>& symbols2, const std::vector<int>& symbols3)
    : machine(machine), resultSymbols(symbols1), resultSymbols2(symbols2), resultSymbols3(symbols3) {
    resScore = resultScore();
    std::cout << resScore << std::endl;
}
void ShowingResultState::render(sf::RenderWindow& mainWindow)
{ 
    sf::Font font;
    font.loadFromFile("arialmt.ttf");
    
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("casino1.jpg");
    sf::Sprite background(backgroundTexture);
    mainWindow.draw(background);


    Button myButton(450, 450, 200, 50, "START", font);
    sf::Vector2f mousePos = mainWindow.mapPixelToCoords(sf::Mouse::getPosition(mainWindow));
    bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    myButton.update(mousePos, isMousePressed);

    myButton.draw(mainWindow);

    std::ostringstream scoreStream;
    scoreStream << std::fixed << std::setprecision(2) << "Score: " << resScore;
    sf::Text scoreText(scoreStream.str(), font, 30);
    scoreText.setPosition(200, 450);
    scoreText.setFillColor(sf::Color::White);

    
    mainWindow.draw(scoreText);

    sf::RectangleShape outline(sf::Vector2f(500, 280));
    outline.setPosition(165, 50);
    outline.setOutlineThickness(1);
    outline.setFillColor(sf::Color::Transparent);
    mainWindow.draw(outline);
    sf::Vertex lines[] =
    {
        sf::Vertex(sf::Vector2f(260,50)),
        sf::Vertex(sf::Vector2f(260,330)),

        sf::Vertex(sf::Vector2f(360,50)),
        sf::Vertex(sf::Vector2f(360,330)),

        sf::Vertex(sf::Vector2f(460,50)),
        sf::Vertex(sf::Vector2f(460,330)),

        sf::Vertex(sf::Vector2f(560,50)),
        sf::Vertex(sf::Vector2f(560,330))
    };
    
    mainWindow.draw(lines, 8, sf::Lines);
    
    for (size_t i = 0; i < SPIN_COUNT; i++)
    {
        sf::Text simbolText(symbols[resultSymbols[i]], font, 30U);
        simbolText.setPosition(200 + i * 100, 70);
        mainWindow.draw(simbolText);
        sf::Text simbolText2(symbols[resultSymbols2[i]], font, 30U);
        simbolText2.setPosition(200 + i * 100, 160);
        mainWindow.draw(simbolText2);
        sf::Text simbolText3(symbols[resultSymbols3[i]], font, 30U);
        simbolText3.setPosition(200 + i * 100, 250);
        mainWindow.draw(simbolText3);
    }
}
double ShowingResultState::resultScore()
{
    double currentScore = 0;
    auto calculateLineScore = [&](const std::vector <int>& line) -> double
    {
        double lineScore = 0;
        std::unordered_map<std::string, double> symbolPoints =
        {
            {"A",20.11},{"B",15.62},{"C",10.64},
            {"D",7.48},{"E",6.83},{"F",5.29}
        };
        for (size_t i = 0; i < SPIN_COUNT; i++)
        {
            lineScore += symbolPoints[symbols[line[i]]];
        }
        int combo=1;
        for (size_t i = 1; i < SPIN_COUNT; i++)
        {
            if (line[i] == line[i - 1])
                combo++;
            else
                combo = 1;
            if (combo == 3)
                lineScore += 50;
            if (combo == 4)
                lineScore += 250;
            if (combo == 5)
                lineScore += 500;

        }
        return lineScore;
    };
    currentScore += calculateLineScore(resultSymbols);
    currentScore += calculateLineScore(resultSymbols2);
    currentScore += calculateLineScore(resultSymbols3);
    return currentScore;
}

void WaitingState::render(sf::RenderWindow& mainWindow)
{
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("casino1.jpg");
    sf::Sprite background(backgroundTexture);
    mainWindow.draw(background);

    sf::Font font;
    if (!font.loadFromFile("arialmt.ttf"))
    {
        std::cout << "can't load arialmt.ttf" << std::endl;
        exit(-1);
    }
    Button myButton(450, 450, 200, 50, "START", font);
    sf::Vector2f mousePos = mainWindow.mapPixelToCoords(sf::Mouse::getPosition(mainWindow));
    bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    myButton.update(mousePos, isMousePressed);
   
    myButton.draw(mainWindow);
   
}

SpinningState::SpinningState(SlotMachine* machine) : machine(machine), elapsedTime(0), currentSpeed(0), reelOffsets(SPIN_COUNT, 0.0f), stopPressed(false), stopTimer(0.0f)
{
    for (size_t i = 0; i < SPIN_COUNT; i++)
    {
        currentSymbols.push_back(rand() % symbols.size());
        currentSymbols2.push_back(rand() % symbols.size());
        currentSymbols3.push_back(rand() % symbols.size());
    }
}
void SpinningState::handleInput(sf::RenderWindow& mainWindow)
{
    sf::Event event;
    while (mainWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            mainWindow.close();

        if (event.type == sf::Event::MouseButtonPressed)
        {
            sf::Vector2i mousePosition = sf::Mouse::getPosition(mainWindow);
            sf::RectangleShape stopButton(sf::Vector2f(200, 50));
            stopButton.setPosition(450, 450);

            if (stopButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
            {
                std::cout << "stop button pressed" << std::endl;
                stopPressed = true;
                stopTimer = 1.0f;  // таймер остановки
            }
        }
    }
}
void SpinningState::render(sf::RenderWindow& mainWindow)
{
    sf::Font font;
    font.loadFromFile("arialmt.ttf");

    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("casino1.jpg");
    sf::Sprite background(backgroundTexture);
    mainWindow.draw(background);
    Button myButton(450, 450, 200, 50, "STOP", font);
    sf::Vector2f mousePos = mainWindow.mapPixelToCoords(sf::Mouse::getPosition(mainWindow));
    bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    myButton.update(mousePos, isMousePressed);
    
    myButton.draw(mainWindow);

    sf::RectangleShape outline(sf::Vector2f(500, 280));
    outline.setPosition(165, 50);
    outline.setOutlineThickness(1);
    outline.setFillColor(sf::Color::Transparent);
    mainWindow.draw(outline);

    sf::Vertex lines[] =
    {
        sf::Vertex(sf::Vector2f(260,50)),
        sf::Vertex(sf::Vector2f(260,330)),

        sf::Vertex(sf::Vector2f(360,50)),
        sf::Vertex(sf::Vector2f(360,330)),

        sf::Vertex(sf::Vector2f(460,50)),
        sf::Vertex(sf::Vector2f(460,330)),

        sf::Vertex(sf::Vector2f(560,50)),
        sf::Vertex(sf::Vector2f(560,330))

    };
    mainWindow.draw(lines, 8, sf::Lines);
    for (size_t i = 0; i < SPIN_COUNT; i++)
    {
        sf::Text simbolText(symbols[currentSymbols[i]], font, 30U);        
        simbolText.setPosition(200 + i * 100, 70 + reelOffsets[i]);
        mainWindow.draw(simbolText);
        sf::Text simbolText2(symbols[currentSymbols2[i]], font, 30U);
        simbolText2.setPosition(200 + i * 100, 150 + reelOffsets[i]);
        mainWindow.draw(simbolText2);
        sf::Text simbolText3(symbols[currentSymbols3[i]], font, 30U);
        simbolText3.setPosition(200 + i * 100, 250 + reelOffsets[i]);
        mainWindow.draw(simbolText3);
    }
}

class SlotMachine {
public:
    SlotMachine()
    {
        setState(std::make_unique<WaitingState>(this));
    }
    void setState(std::unique_ptr<State> state)
    {
        currentState = std::move(state);
    }
    void run()
    {
        
        sf::RenderWindow mainWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Slot-machine");
        mainWindow.setVerticalSyncEnabled(true);
        sf::Image icon;
        icon.loadFromFile("icon.png");
        mainWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
        sf::Clock clock;

        while (mainWindow.isOpen())
        {
            float dt = clock.restart().asSeconds();
            currentState->handleInput(mainWindow);
            currentState->update(dt);
            mainWindow.clear();
            currentState->render(mainWindow);
            mainWindow.display();
            //sf::sleep(sf::milliseconds(13));
        }
    }
private:
    std::unique_ptr<State> currentState;
};

void WaitingState::handleInput(sf::RenderWindow& mainWindow)
{
    sf::Event event;
    while (mainWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            mainWindow.close();
        sf::RectangleShape startButton(sf::Vector2f(200, 50));
        startButton.setPosition(450, 450);

        if (event.type == sf::Event::MouseButtonPressed)
        {
            sf::Vector2i mousePosition = sf::Mouse::getPosition(mainWindow);
            if (startButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
            {
                std::cout << "start button pressed" << std::endl;
                machine->setState(std::make_unique<SpinningState>(machine));
            }
            
        }
    }
}
void ShowingResultState::handleInput(sf::RenderWindow& mainWindow)
{
    sf::Event event;
    while (mainWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            mainWindow.close();
        sf::RectangleShape startButton(sf::Vector2f(200, 50));
        startButton.setPosition(450, 450);

        if (event.type == sf::Event::MouseButtonPressed)
        {
            sf::Vector2i mousePosition = sf::Mouse::getPosition(mainWindow);
            if (startButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
            {
                std::cout << "start button pressed" << std::endl;
                machine->setState(std::make_unique<SpinningState>(machine));
            }
        }
    }
}
void SpinningState::update(float dt)
{
    elapsedTime += dt;
    if (stopPressed)
    {
        stopTimer -= dt;
        if (stopTimer <= 0.0f)
        {
            machine->setState(std::make_unique<ShowingResultState>(machine, currentSymbols, currentSymbols2, currentSymbols3));
            return;
        }
    }
    float spinDuration = SPIN_DURATION;

    // разгон -> макс скорость -> замедление
    if (elapsedTime < ACCELERATION_TIME) {
        currentSpeed = MAX_SPIN_SPEED * (elapsedTime / ACCELERATION_TIME);
    }
    else if (elapsedTime > spinDuration - DECELERATION_TIME) {
        float decelTime = spinDuration - elapsedTime;
        currentSpeed = MAX_SPIN_SPEED * (decelTime / DECELERATION_TIME);
    }
    else {
        currentSpeed = MAX_SPIN_SPEED;
    }

    for (int i = 0; i < SPIN_COUNT; ++i) {
        reelOffsets[i] += currentSpeed * dt;
        if (reelOffsets[i] >= 50.0f) {
            reelOffsets[i] = 0.0f;
            currentSymbols[i] = rand() % symbols.size();
            currentSymbols2[i] = rand() % symbols.size();
            currentSymbols3[i] = rand() % symbols.size();
        }
    }
    if (elapsedTime >= spinDuration) {
        machine->setState(std::make_unique<ShowingResultState>(machine, currentSymbols, currentSymbols2, currentSymbols3));
    }
}

int main() {
    setlocale(LC_ALL, "ru");
   
    SlotMachine machine;
    machine.run();
    
    return 0;
}