#include "App.h"
#include "../InputBuf/InputBuf.h"

App::App(const int width, const int height) {
    inputBuf = std::make_unique<InputBuf>(width, height, this);
}

void App::Run() {
  if (inputBuf->getRequestedClose()) done = true;
  inputBuf->Draw();
}
