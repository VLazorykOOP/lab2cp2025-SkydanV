#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include <mutex>
#include <random>
#include <atomic>
#include <vector>
#include <conio.h> 

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Position {
    double x = 0;
    double y = 0;
};

std::mutex mtx;
std::atomic<bool> running(true);

double rand_direction() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<> dist(0, 2 * M_PI);
    return dist(gen);
}

void developer_motion(int id, double V, int N) {
    Position pos{ 0, 0 };
    double dir = rand_direction();

    while (running) {
        pos.x += V * cos(dir);
        pos.y += V * sin(dir);

        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Dev " << id << " position: (" << pos.x << ", " << pos.y << "), dir: " << dir << "\n";
        }

        for (int i = 0; i < N; ++i) {
            if (!running) break;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        dir = rand_direction();
    }
}

void manager_motion(int id, double V, double R) {
    Position pos{ R, 0 };
    double angle = 0.0;

    double omega = V / R;

    while (running) {
        angle += omega;

        pos.x = R * cos(angle);
        pos.y = R * sin(angle);

        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Manager " << id << " position: (" << pos.x << ", " << pos.y << "), angle: " << angle << "\n";
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    const double V = 1.0;
    const int N = 5;
    const double R = 10.0;

    std::vector<std::thread> threads;

    for (int i = 0; i < 2; ++i)
        threads.emplace_back(developer_motion, i + 1, V, N);

    for (int i = 0; i < 2; ++i)
        threads.emplace_back(manager_motion, i + 1, V, R);


    while (running) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 'x' || ch == 'X') {
                running = false;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    for (auto& t : threads) t.join();

    return 0;
}
