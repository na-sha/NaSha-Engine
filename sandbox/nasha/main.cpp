#include <nasha.inl>

class Sandbox : public nasha::Application{
private:
    /* data */
public:
    Sandbox(/* args */)= default;
    ~Sandbox() override= default;
};

nasha::Application* nasha::createApplication(){
    return new Sandbox();
}