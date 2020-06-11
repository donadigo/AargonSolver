#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <deque>
#include "PieceMap.h"
#include "Reflector.h"
#ifdef _WIN32
#include <Windows.h>
#endif 


auto mirrorReflector = std::make_shared<MirrorReflector>();
auto oneWayReflector = std::make_shared<OneWayReflector>();
auto prismReflector = std::make_shared<PrismReflector>();
auto splitterReflector = std::make_shared<SplitterReflector>();
auto threewayReflector = std::make_shared<ThreewayReflector>();
auto refractorReflector = std::make_shared<RefractorReflector>();
auto convertReflector = std::make_shared<ConvertReflector>();
auto doubleReflector = std::make_shared<DoubleReflector>();

int maxLitCoins = 0;
int iteration = 0;
bool solve(const PieceMap& map,
           const std::unordered_map<std::shared_ptr<Piece>, unsigned>& available,
           const std::vector<Vec2>& positions,
           const std::vector<Vec2>& used)
{
    for (auto& position : positions) {
        for (auto& pair : available) {
            if (pair.second == 0) {
                continue;
            }

            for (int rotation = 0; rotation < 360; rotation += 45) {
                auto p = *pair.first;
                p.pos = position;
                p.rotation = static_cast<Rotation>(rotation);

                PieceMap newMap = map;
                newMap.add(p);
                newMap.update();

                if (newMap.isSolved()) {
                    return true;
                } 

                int prev = maxLitCoins;
                maxLitCoins = (std::max)(maxLitCoins, map.getLitCoins());

                if (iteration % 10000 == 0) {
                    system("cls");
                    std::cout << "Max lit coins: " << maxLitCoins << "\n";
                    newMap.print();
                }

                auto newAvailable = available;
                newAvailable[pair.first]--;

                auto newUsed = used;
                newUsed.insert(newUsed.begin(), positions.begin(), positions.end());

                auto newPositions = newMap.getAvailableBeamPositions();
                newPositions.erase(std::remove_if(newPositions.begin(), newPositions.end(), [&](const Vec2& pos) {
                    return std::find(newUsed.begin(), newUsed.end(), pos) != newUsed.end();
                }), newPositions.end());

                iteration++;
                if (solve(newMap, newAvailable, newPositions, newUsed)) {
                    return true;
                }
            }
        }
    }

    return false;
}

struct Frame
{
    PieceMap map;
    std::vector<Vec2> positions;
    std::map<std::shared_ptr<Piece>, int> available;
    std::vector<Vec2> used;

    Frame(const PieceMap& _map) : map(_map) {}
    Frame(const Frame& frame) :
        map(frame.map),
        positions(frame.positions),
        available(frame.available),
        used(frame.used)
    {

    }
};

bool frameCanBeSolved(Frame& frame)
{
    Color availableColors = NONE;
    Color requiredColors = NONE;

    auto& map = frame.map;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            auto& piece = map.getAt(col, row);
            auto& beams = map.getBeamsAt(col, row);
            if (std::find(frame.positions.begin(), frame.positions.end(), Vec2(col, row)) != frame.positions.end()) {
                for (auto& beam : beams) {
                    availableColors = static_cast<Color>(availableColors | beam.color);
                }
            }

            if (piece && piece->type == COIN) {
                requiredColors = static_cast<Color>(requiredColors | piece->color);
                if (!piece->lit && !beams.empty()) {
                    for (auto& b : beams) {
                        if ((b.color | piece->color) != piece->color) {
                            //return false;
                        }
                    }
                }
            }
        }
    }

    if ((requiredColors & availableColors) != requiredColors) {
        auto it = std::find_if(frame.available.begin(), frame.available.end(), [&](auto& pair) {
            bool isConverter = dynamic_cast<ConvertReflector*>(pair.first->reflector.get()) != nullptr;
            return isConverter && pair.second > 0;
        });

        return it != frame.available.end();
    }

    return true;
}

PieceMap solveIter(const PieceMap& map, const std::map<std::shared_ptr<Piece>, int>& available)
{
    std::deque<Frame> stack;
    Frame f(map);
    f.positions = map.getAvailableBeamPositions();
    f.available = available;
    stack.push_back(f);

    while (!stack.empty()) {
        Frame frame = std::move(stack.back());
        stack.pop_back();
        for (auto& pos : frame.positions) {
            for (auto& pair : frame.available) {
                if (pair.second == 0) {
                    continue;
                }

                for (int rotation = 0; rotation < 360; rotation += 45) {
                    Frame newFrame(frame);
                    auto p = *pair.first;
                    p.pos = pos;
                    p.rotation = static_cast<Rotation>(rotation);
                    newFrame.map.add(p);
                    newFrame.map.update();

                    if (newFrame.map.isSolved()) {
                        return newFrame.map;
                    }

                    int prev = maxLitCoins;
                    maxLitCoins = (std::max)(maxLitCoins, newFrame.map.getLitCoins());

                    newFrame.available[pair.first]--;
                    newFrame.used.insert(newFrame.used.begin(), frame.positions.begin(), frame.positions.end());

                    newFrame.positions = newFrame.map.getAvailableBeamPositions();
                    newFrame.positions.erase(std::remove_if(newFrame.positions.begin(), newFrame.positions.end(), [&](const Vec2& pos) {
                        return std::find(newFrame.used.begin(), newFrame.used.end(), pos) != newFrame.used.end();
                    }), newFrame.positions.end());

                    if (!frameCanBeSolved(newFrame)) {
                        continue;
                    }

                    if (iteration % 10000 == 0) {
                        system("cls");
                        std::cout << "Max lit coins: " << maxLitCoins << "\n";
                        newFrame.map.print();
                    }

                    iteration++;
                    stack.push_back(newFrame);
                }
            }
        }
    }

    return map;
}

int main()
{
    PieceMap map;
    map.add(Piece::createReflector(4, 0, SOUTH_WEST, oneWayReflector, false, false));
    map.add(Piece::createReflector(4, 1, SOUTH_WEST, oneWayReflector, false, false));
    map.add(Piece::createReflector(4, 2, SOUTH_WEST, oneWayReflector, false, false));

    map.add(Piece::createReflector(8, 0, SOUTH_EAST, oneWayReflector, false, false));
    map.add(Piece::createReflector(8, 1, SOUTH_EAST, oneWayReflector, false, false));
    map.add(Piece::createReflector(8, 2, SOUTH_EAST, oneWayReflector, false, false));

    map.add(Piece::createReflector(4, 8, NORTH_WEST, oneWayReflector, false, false));
    map.add(Piece::createReflector(4, 9, NORTH_WEST, oneWayReflector, false, false));
    map.add(Piece::createReflector(4, 10, NORTH_WEST, oneWayReflector, false, false));

    map.add(Piece::createReflector(8, 8, NORTH_EAST, oneWayReflector, false, false));
    map.add(Piece::createReflector(8, 9, NORTH_EAST, oneWayReflector, false, false));
    map.add(Piece::createReflector(8, 10, NORTH_EAST, oneWayReflector, false, false));

    map.add(Piece::createReflector(4, 3, NORTH, prismReflector, false, false));
    map.add(Piece::createReflector(5, 4, NORTH, prismReflector, false, false));

    map.add(Piece::createReflector(4, 7, WEST, prismReflector, false, false));
    map.add(Piece::createReflector(5, 6, WEST, prismReflector, false, false));

    map.add(Piece::createReflector(8, 3, EAST, prismReflector, false, false));
    map.add(Piece::createReflector(7, 4, EAST, prismReflector, false, false));

    map.add(Piece::createReflector(7, 6, SOUTH, prismReflector, false, false));
    map.add(Piece::createReflector(8, 7, SOUTH, prismReflector, false, false));

    map.add(Piece::createLaser(13, 5, WEST, WHITE));

    map.add(Piece::createCoin(4, 4, BLUE));
    map.add(Piece::createCoin(4, 6, BLUE));
    map.add(Piece::createCoin(5, 2, RED));
    map.add(Piece::createCoin(6, 2, WHITE));
    map.add(Piece::createCoin(7, 2, BLUE));
    map.add(Piece::createCoin(5, 8, RED));
    map.add(Piece::createCoin(6, 8, WHITE));
    map.add(Piece::createCoin(7, 8, BLUE));
    map.add(Piece::createCoin(0, 5, BLUE));
    map.add(Piece::createCoin(1, 5, BLUE));
    map.add(Piece::createCoin(2, 5, BLUE));

    //Crashes
    //map.add(Piece::createReflector(9, 3, NORTH_EAST, mirrorReflector));
    //map.add(Piece::createReflector(10, 3, SOUTH_EAST, mirrorReflector));
    //map.add(Piece::createReflector(10, 4, NORTH_WEST, mirrorReflector));
    //map.add(Piece::createReflector(11, 4, SOUTH_EAST, mirrorReflector));
    //map.add(Piece::createReflector(12, 5, NORTH_EAST, prismReflector));
    //map.add(Piece::createReflector(9, 6, NORTH_WEST, mirrorReflector));
    //map.add(Piece::createReflector(12, 6, NORTH_WEST, doubleReflector));
    //map.add(Piece::createReflector(11, 7, NORTH_WEST, mirrorReflector));
    //map.add(Piece::createReflector(13, 7, SOUTH_EAST, mirrorReflector));
    //map.add(Piece::createReflector(9, 8, NORTH, prismReflector));
    //map.add(Piece::createReflector(12, 8, NORTH_WEST, refractorReflector));
    //map.add(Piece::createReflector(13, 8, NORTH_WEST, prismReflector));
    //map.add(Piece::createReflector(13, 9, NORTH_EAST, doubleReflector));
    //map.add(Piece::createReflector(14, 9, SOUTH_EAST, prismReflector));
    //map.add(Piece::createReflector(9, 10, NORTH_WEST, mirrorReflector));
    //map.add(Piece::createReflector(14, 10, SOUTH_WEST, mirrorReflector));

    //map.update();
    //map.print();
    //return 0;

    std::map<std::shared_ptr<Piece>, int> available =
    {
        { std::make_shared<Piece>(Piece::createReflector(-1, -1, NORTH, mirrorReflector)), 9 },
        { std::make_shared<Piece>(Piece::createReflector(-1, -1, NORTH, prismReflector)), 4 },
        { std::make_shared<Piece>(Piece::createReflector(-1, -1, NORTH, refractorReflector)), 2 },
        { std::make_shared<Piece>(Piece::createReflector(-1, -1, NORTH, doubleReflector)), 2 }
    };

    map.update();
    //solve(map, available, map.getAvailableBeamPositions(), {});
    solveIter(map, available);
    // OneWay test
    //Piece laser = Piece::createLaser(10, 10, NORTH, BLUE);
    //Piece laser2 = Piece::createLaser(15, 10, NORTH, RED);
    //Piece oneWay = Piece::createReflector(10, 7, SOUTH, oneWayReflector);
    //Piece mirror = Piece::createReflector(10, 5, NORTH_EAST, mirrorReflector);
    //Piece mirror2 = Piece::createReflector(15, 5, SOUTH_EAST, mirrorReflector);
    //map.add(laser);
    //map.add(laser2);
    //map.add(oneWay);
    //map.add(mirror);
    //map.add(mirror2);

    // Prism test
    //map.add(Piece::createLaser(5, 0, SOUTH_EAST, WHITE));
    //map.add(Piece::createLaser(5, 5, EAST, WHITE));
    //map.add(Piece::createLaser(10, 0, SOUTH, GREEN));
    //map.add(Piece::createReflector(10, 5, WEST, prismReflector));

    // Splitter test
    //map.add(Piece::createLaser(10, 10, NORTH, GREEN));
    //map.add(Piece::createLaser(17, 5, WEST, BLUE));
    //map.add(Piece::createLaser(0, 5, EAST, RED));
    //map.add(Piece::createReflector(10, 5, NORTH, splitterReflector));
    //map.add(Piece::createCoin(5, 5, YELLOW));

    // Recursion test
    //map.add(Piece::createLaser(10, 10, NORTH, RED));
    //map.add(Piece::createReflector(10, 7, SOUTH_EAST, threewayReflector));
    //map.add(Piece::createReflector(7, 4, NORTH, mirrorReflector));
    //map.add(Piece::createReflector(10, 1, NORTH_EAST, refractorReflector));
    //map.add(Piece::createReflector(17, 1, SOUTH_EAST, mirrorReflector));
    //map.add(Piece::createReflector(17, 7, SOUTH_WEST, mirrorReflector));
    //map.add(Piece::createReflector(14, 7, EAST, convertReflector));
    //map.add(Piece::createCoin(15, 7, WHITE));

    // Double test
    /*map.add(Piece::createLaser(10, 10, NORTH, RED));
    map.add(Piece::createLaser(10, 0, SOUTH, BLUE));
    map.add(Piece::createReflector(10, 5, SOUTH, doubleReflector));*/

    //map.print();
    //std::cout << "Solved: " << map.isSolved() << "\n";
    return 0;
}