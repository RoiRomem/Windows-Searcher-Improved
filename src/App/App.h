#ifndef APP_H
#define APP_H

#include <memory>

class InputBuf;

class App {
    public:
        std::unique_ptr<InputBuf> inputBuf;
        App(const int width, const int height);
        bool done = false;
        bool active = false;
        void Run();
};

#endif //APP_H
