#pragma once

#include <memory>
#include <vector>

#include <GLFW/glfw3.h>

#include <Application.h>
#include <Views/Viewable.h>


class View
{
private:
    GLFWwindow* _window;
    Application* _app;

    std::vector<std::unique_ptr<Viewable>> _views;
    std::vector<std::unique_ptr<Viewable>> _stagedViews;

    void _init();
    void _stageView(std::unique_ptr<Viewable> view);
public:
    View(Application* app);
    ~View();

    void render() noexcept(false);

    bool getWindowShouldClose() const noexcept;
};
