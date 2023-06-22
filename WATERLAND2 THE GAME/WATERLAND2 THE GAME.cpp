#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <iomanip>

#define endl std::endl
#define print(x) std::cout << x
#define LAST(x) x, __last__entry
#define RANDENUM(enumType) (enumType)(rand() % (int)enumType::__last__entry)

#define NUM_ROUNDS 25

enum class Weather {
    SUNNY,
    CLOUDY,
    RAINING,
    LAST(THUNDERSTORM),
};

float getWeatherSalesMultiplier(Weather weather) {
    switch (weather) {
    case Weather::SUNNY:
        return 1.2;
    case Weather::CLOUDY:
        return 1;
    case Weather::RAINING:
        return 0.7;
    case Weather::THUNDERSTORM:
        return 0.2;
    }
}

float getChanceOfPurchase(Weather weather, float price) {
    switch (weather) {
    case Weather::SUNNY:
        return -0.6787621 + (99.81697 - -0.6787621) / (1 + pow(price / 1.259171, 3.857938));
    default:
        return -1.094763 + (99.85388 - -1.094763) / (1 + pow(price / 1.020997, 3.153287));
    }
    
}

const char* getWeatherSplashText(Weather weather) {
    switch (weather) {
    case Weather::SUNNY:
        return R"CHEEZEIT(        .
      \ | /
    '-.;;;.-'
   -==;;;;;==-
    .-';;;'-.
      / | \
        
Its SUNNY! You will profit from increased sales figures and higher willingness to pay more!)CHEEZEIT";
    case Weather::CLOUDY:
        return R"CHEEZEIT(      __   _
    _(  )_( )_
   (_   _    _)
     (_) (__)

Its CLOUDY! Sales are as usual.)CHEEZEIT";
    case Weather::RAINING:
        return R"CHEEZEIT(      __   _
    _(  )_( )_
   (_   _    _)
  / /(_) (__)
 / / / / / /
/ / / / / /

Its RAINING! Not as many people will be outside, you will see reduced sales figures.)CHEEZEIT";
    case Weather::THUNDERSTORM:
        return R"CHEEZEIT(      __   _
    _(  )_( )_
   (_   _    _)
     (_) (__)
        /
        \
        /

There is a THUNDERSTORM! Barely anyone will be outside and buy your water.)CHEEZEIT";
    }
}

struct PlayerState {
    float money;
    float pricePerGlass;
    uint32_t glassesOnStock;
};

int tryReadInt(char* msg) {
    print(msg << endl);
    
    char inp[100];
    std::cin >> inp;

    int num;

    try {
        num = std::stoi(inp);
        return num;
    }
    catch (std::exception const& ex) {
        print("Invalid number! Try again!" << endl);
        return tryReadInt(msg);
    }
}

float tryReadFloat(char* msg) {
    print(msg << endl);

    char inp[100];
    std::cin >> inp;

    float num;

    try {
        num = std::stof(inp);

        return num;
    }
    catch (std::exception const& ex) {
        print("Invalid number! Try again!" << endl);
        return tryReadInt(msg);
    }
}

void initPlayerState(PlayerState* state) {
    state->money = 6;
    state->glassesOnStock = 0;
    state->pricePerGlass = 0;
}

int main()
{
    print("Welcome to Waterland 2!" << endl << std::fixed << std::setprecision(2));

    int playerCount = tryReadInt((char*)"Please choose the amount of players you want to play with:");
    std::vector<PlayerState> players = *(new std::vector<PlayerState>());

    for (int i = 0; i < playerCount; i++) {
        PlayerState* state = (PlayerState*)malloc(sizeof(PlayerState));

        if (state == nullptr) {
            print("Failed to allocate player states. Most likely out of memory!");
            return -123;
        }

        initPlayerState(state);
        players.push_back(*state);
    }

    if (playerCount == 1) {
        print("King: Okay, so you are lonely. Lets see how far you get with " << NUM_ROUNDS << " rounds!" << endl);
    }
    else if (playerCount == 69) {
        print("King: So we are doing this for the memes?" << endl);
    }
    else {
        print("King: The one to reach the most amount of money within " << NUM_ROUNDS << " rounds gets my crown!" << endl);
    }

    Weather currentWeather = Weather::SUNNY;

    for (int round = 0; round < NUM_ROUNDS; round++) {
        currentWeather = RANDENUM(Weather);

        print(endl << endl << endl << endl << getWeatherSplashText(currentWeather) << endl);

        for (int playerIdx = 0; playerIdx < playerCount; playerIdx++) {
            PlayerState* player = &players.at(playerIdx);

            print(endl << endl << endl << "Its your turn, player " << playerIdx << "! You have " << player->money << "$! What do you want to set your price to ? " << endl);
        priceSelection:
            float newPrice = tryReadFloat((char*)"New Price: ");

            if (newPrice < 0 || newPrice > 100) {
                print("Thats too low or too high of a price! Please try again!" << endl);
                goto priceSelection;
            }

            player->pricePerGlass = newPrice;

            print("Okay, selling them for " << player->pricePerGlass << "$ per glass of water. How many glasses of water do you want to order, player " << playerIdx << " ? One glass costs you 0.6$!" << endl);

        glassOrdering:
            int amountOfGlasses = tryReadInt((char*)"Amount of glasses to order: ");

            if (amountOfGlasses < 0) {
                print("You cant order less than 0 glasses you goofy goober!" << endl);
                goto glassOrdering;
            }

            if (amountOfGlasses * 0.6 > player->money) {
                print("You cant afford that many glasses of water!" << endl);
                goto glassOrdering;
            }

            player->glassesOnStock = amountOfGlasses;
            player->money -= amountOfGlasses * 0.6;

#pragma region Simulate Market
            float salesMult = getWeatherSalesMultiplier(currentWeather);
            float willigness = getChanceOfPurchase(currentWeather, player->pricePerGlass);
            int amountOfPassengers = (100 + rand() % 200) * salesMult;
            int nSales = 0;
            int nMissedSales = 0;

            for (int i = 0; i < amountOfPassengers; i++) {
                if (rand() % 100 < willigness) {
                    if (player->glassesOnStock <= 0) {
                        nMissedSales++;
                        continue;
                    }

                    nSales++;
                    player->money += player->pricePerGlass;
                    player->glassesOnStock--;
                }
            }

            print("Your turn is over. There were " << amountOfPassengers << " potential customers. You bought " << amountOfGlasses << " glasses and sold " << nSales << " of them for a total revenue of " << (nSales * player->pricePerGlass) << "$ or a total profit of " << ((nSales * player->pricePerGlass) - (amountOfGlasses * 0.6)) << "$!" << endl);
#pragma endregion
        }
    }

#pragma region Determine winner
    float currentLeadMoney = -1;
    int currentLeadIndex = 0;

    for (int i = 0; i < playerCount; i++) {
        if (players.at(i).money > currentLeadMoney) {
            currentLeadIndex = i;
            currentLeadMoney = players.at(i).money;
        }
    }

    print("The winner is player number " << currentLeadIndex << " with " << currentLeadMoney << "$! Congrats!" << endl);
#pragma endregion

}