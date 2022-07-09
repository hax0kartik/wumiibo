#include "app.hpp"
int main()
{
    App app;
    app.Intialize();
    app.RunLoop();
    app.Finalize();
    return 0;
}