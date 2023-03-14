#include "View.h"

#include <exception>

#include <Views/ChartView.h>
#include <Views/FileDialogView.h>
#include <Views/ItemView.h>
#include <Views/PopUps.h>


// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui_internal.h>
#include <implot.h>
#include <stdio.h>
#define GL_SILENCE_DEPRECATION

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void View::_init()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        throw std::exception("glfwInit failed");
    }

    // Decide GL+GLSL versions
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    _window = glfwCreateWindow(1280, 720, "MPT", NULL, NULL);
    if (!_window)
    {
        throw std::exception("Failed to create glfw window");
    }
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1);
    glfwSetWindowAttrib(_window, GLFW_DECORATED, GLFW_FALSE);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImPlot::CreateContext();
}

void View::_stageView(std::unique_ptr<Viewable> view)
{
    _stagedViews.push_back(std::move(view));
}

View::View(Application* app)
{
    _init();
    _views.push_back(std::make_unique<ItemView>());
    _views.push_back(std::make_unique<ChartView>());
    _app = app;
}

View::~View()
{
    // Cleanup
    ImPlot::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(_window);
    glfwTerminate();
}

void View::render() noexcept(false)
{
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    const ImGuiStyle& style = ImGui::GetStyle();
    const float menuBarHeight = style.FramePadding.y * 2.0f + ImGui::CalcTextSize("MPT").y;
    if (ImGui::BeginViewportSideBar("##TitleBar", ImGui::GetMainViewport(), ImGuiDir_Up, menuBarHeight, ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::Text("MPT");
            if (!_app->getModel().getSaved())
            {
                ImGui::Text("*");
            }
            float windowXMax = ImGui::GetWindowWidth();

            //  Window buttons
            const ImVec2 windowButtonSize(menuBarHeight, menuBarHeight);
            ImGui::SetCursorPosX(windowXMax - menuBarHeight);
            if (ImGui::Button("X##Close", windowButtonSize))
            {
                _app->setShouldStop();
            }
            ImGui::SetCursorPosX(windowXMax - menuBarHeight * 2.0f - style.ItemSpacing.x);
            if (ImGui::Button("^##Maximize", windowButtonSize))
            {
                if (glfwGetWindowAttrib(_window, GLFW_MAXIMIZED))
                {
                    glfwRestoreWindow(_window);
                }
                else
                {
                    glfwMaximizeWindow(_window);
                }
            }
            ImGui::SetCursorPosX(windowXMax - menuBarHeight * 3.0f - style.ItemSpacing.x * 2.0f);
            if (ImGui::Button("_##Minimize", windowButtonSize))
            {
                glfwIconifyWindow(_window);
            }

            ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
            ImGui::InvisibleButton("##TitleGrab", ImVec2(windowXMax, menuBarHeight));
            if (ImGui::IsItemActive())
            {
                int x, y;
                glfwGetWindowPos(_window, &x, &y);
                ImGuiIO& io = ImGui::GetIO();
                const ImVec2 delta = io.MouseDelta;
                x += static_cast<int>(delta.x);
                y += static_cast<int>(delta.y);
                glfwSetWindowPos(_window, x, y);
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();
    }

    if (ImGui::BeginViewportSideBar("##MenuBar", ImGui::GetMainViewport(), ImGuiDir_Up, menuBarHeight, ImGuiWindowFlags_MenuBar))
    //if (ImGui::Begin("##MenuBar", 0, ImGuiWindowFlags_MenuBar))
    {
        ImGuiWindowFlags_UnsavedDocument;
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open"))
                {
                    if (_app->getModel().getProjFilepath().empty())
                    {
                        _stageView(
                            std::move(
                                std::make_unique<FileDialogView>(
                                    *MPT::GetRootDirs().begin(),
                                    MPT::FileDialogPurpose::Open,
                                    MPT::FileExtension::MPT
                                    )
                            )
                        );
                    }
                    else
                    {
                        _stageView(
                            std::move(
                                std::make_unique<FileDialogView>(
                                    _app->getModel().getProjFilepath(),
                                    MPT::FileDialogPurpose::Open,
                                    MPT::FileExtension::MPT
                                    )
                            )
                        );
                    }
                }
                if (ImGui::MenuItem("Save"))
                {
                    try
                    {
                        _app->getModel().save();
                    }
                    catch (const std::exception&)
                    {
                        _stageView(
                            std::move(
                                std::make_unique<FileDialogView>(
                                    *MPT::GetRootDirs().begin(),
                                    MPT::FileDialogPurpose::Save,
                                    MPT::FileExtension::MPT
                                    )
                            )
                        );
                    }
                }
                if (ImGui::MenuItem("Save as"))
                {
                    if (_app->getModel().getProjFilepath().empty())
                    {
                        _stageView(
                            std::move(
                                std::make_unique<FileDialogView>(
                                    *MPT::GetRootDirs().begin(),
                                    MPT::FileDialogPurpose::Save,
                                    MPT::FileExtension::MPT
                                    )
                            )
                        );
                    }
                    else
                    {
                        _stageView(
                            std::move(
                                std::make_unique<FileDialogView>(
                                    _app->getModel().getProjFilepath(),
                                    MPT::FileDialogPurpose::Save,
                                    MPT::FileExtension::MPT
                                    )
                            )
                        );
                    }
                }
                if (ImGui::MenuItem("Close"))
                {
                    _app->getModel().closeProject();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Items"))
                {
                    _stageView(std::move(std::make_unique<ItemView>()));
                }
                if (ImGui::MenuItem("Graph"))
                {
                    _stageView(std::move(std::make_unique<ChartView>()));
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();
    }

    for (auto it = _views.begin(); it != _views.end();)
    {
        it->get()->onImGuiRender(*_app);
        if (it->get()->shouldClose())
        {
            it = _views.erase(it);
            continue;
        }
        ++it;
    }

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui::Render();

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    for (auto& it : _stagedViews)
    {
        for (auto it = _stagedViews.begin(); it != _stagedViews.end(); ++it)
        {
            _views.push_back(std::move(*it));
        }
        _stagedViews.clear();
    }

    int display_w, display_h;
    glfwGetFramebufferSize(_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(_window);
}

bool View::getWindowShouldClose() const noexcept
{
    return glfwWindowShouldClose(_window);
}
