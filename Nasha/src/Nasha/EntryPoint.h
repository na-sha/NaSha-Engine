#pragma once

extern Nasha::Application* Nasha::createApplication();

int main(int argc, char** argv){
    std::cout << "Successfully created application" << std::endl;
    auto app = Nasha::createApplication();
    app -> Run();
    delete app;
}
