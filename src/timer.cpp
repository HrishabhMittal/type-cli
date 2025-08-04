#ifndef TIMER
#define TIMER
#include <chrono>

class Timer {
    using clock=std::chrono::high_resolution_clock;
    clock::time_point start_time,past_time,pause_time;
    bool running=true;
public:
    Timer() : start_time(clock::now()),past_time(start_time),running(true) {}
    int time_from_beginning() {
        if (running) {
            auto end_time=clock::now();
            past_time=end_time;
            return std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time).count();
        } else {
            return std::chrono::duration_cast<std::chrono::milliseconds>(pause_time-start_time).count();
        }
    }

    void stop() {
        if (running) {
            pause_time=clock::now();
            running=false;
        }
    }
    void reset_and_start() {
        running=true;
        start_time=clock::now();
        past_time=start_time;
    }
    void resume() {
        if (!running) {
            auto now=clock::now();
            start_time+=(now-pause_time);
            past_time=now;
            running=true;
        }
    }

    int time_from_last_call() {
        if (!running) return 0;
        auto end_time=clock::now();
        auto past_time_copy=past_time;
        past_time=end_time;
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time-past_time_copy).count();
    }
};
#endif
