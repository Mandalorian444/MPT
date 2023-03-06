#include "Application.h"

#include <iostream>

#include <View.h>
#include <DataClasses/Date.h>

//  Implement from here
//  https://github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/main.cpp

int main()
{
    Application app;
    View view(&app);

    while (!app.getShouldStop())
    {
        view.render();
        if (view.getWindowShouldClose())
        {
            app.setShouldStop();
        }
    }

    return 0;
}