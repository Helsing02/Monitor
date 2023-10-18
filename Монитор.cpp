#include <iostream>
#include <locale>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex lock;
std::condition_variable cv;
bool ready = false;

void prov_func() {
    for (int i = 0; i < 10; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        lock.lock();
        if (ready) {
            std::cout << "provider waits...\n";
            lock.unlock();
            continue;
        }
        std::cout << "event triggered\n";
        cv.notify_one();
        std::cout << "consumer was notified\n";
        ready = true;
        lock.unlock();
    }
}

void consume_func() {
    for (int i = 0; i < 10; i++) {
        std::unique_lock <std::mutex> lk(lock);
        std::cout << "waiting\n";
        cv.wait(lk, [] {return ready == true; });
        std::cout << "finished waiting\n";
        ready = false;
        std::cout << "consumed\n\n";
        lk.unlock();
    }
}

int main() {
    setlocale(LC_ALL, "rus");

    std::thread provider(prov_func);
    std::thread consumer(consume_func);

    provider.join();
    consumer.join();

}