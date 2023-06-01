#include "../../Nasha/src/Nasha.h"

class Sandbox : public Nasha::Application{
private:
    /* data */
public:
    Sandbox(/* args */)= default;
    ~Sandbox() override= default;
};

Nasha::Application* Nasha::createApplication(){
    return new Sandbox();
}
