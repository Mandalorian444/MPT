#include "Application.h"

#include <iostream>

#include <View.h>
#include <DataClasses/Date.h>


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