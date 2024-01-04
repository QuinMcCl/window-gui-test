#include <stdio.h>
#include <math.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "renderer.h"

#include <nonstd.h>
#include <nonstd_glfw_opengl.h>

typedef struct imgui_file_options_s
{
    bool options_enabled = false;
    bool unsaved_changes = true;
    bool should_close = false;
} imgui_file_options_t;

typedef struct imgui_tool_options_s
{
    bool show_tool_metrics = 0;
    bool show_tool_debug_log = 0;
    bool show_tool_id_stack_tool = 0;
    bool show_tool_style_editor = 0;
    bool show_tool_about = 0;
} imgui_tool_options_t;

typedef struct imgui_main_menu_options_s
{
    imgui_file_options_t file_options;
    imgui_tool_options_t tool_options;
} imgui_main_menu_options_t;

void ShowMainMenu(imgui_main_menu_options_t *menu_options);
void ShowFileMenu(imgui_file_options_t *file_options);
void ShowTools(imgui_tool_options_t *tool_options);

void ShowClosePopUp(imgui_file_options_t *file_options, program_state_t *state);

int imgui_event_handle_blocker(void *ptr, void *e);

void *foregroundThread(void *args)
{
    program_state_t current_state = INVALID_STATE;
    get_current_state(&current_state);

    ImGuiIO io;
    nonstd_glfw_t imgui_handle_blocker;
    nonstd_glfw_window_t tmp_window;
    nonstd_glfw_window_t main_window;

    struct timespec ts;
    tripplebuffer_t *tripplebuffer = (tripplebuffer_t *)args;

    // Our state
    double dt = 1.0 / 60.0;

    imgui_main_menu_options_t main_menu_options;

    double h = 0.0;
    double c = 1.0;
    double x = 0.0;
    double m = 0.0;

    float red, green, blue;

    // init imgui_handle_blocker
    {
        imgui_handle_blocker.update = nonstd_glfw_update;
        imgui_handle_blocker.draw = nonstd_glfw_draw;
        imgui_handle_blocker.cleanup = nonstd_glfw_cleanup;
        imgui_handle_blocker.event_handler = imgui_event_handle_blocker;
        imgui_handle_blocker.sibling = NULL;
        imgui_handle_blocker.child = NULL;
    }

    nonstd_glfw_window_init(&tmp_window, 100, 100, "tmp_window", 0.0f, 0.0f, 0.0f, 1.0f);
    nonstd_glfw_window_cleanup(&tmp_window);
    nonstd_glfw_window_init(&main_window, 1920, 1080, "MainWindow", 0.0f, 0.0f, 0.0f, 1.0f);

    nonstd_glfw_adopt(&imgui_handle_blocker, (nonstd_glfw_t *)&main_window);

    glfwSetWindowUserPointer(main_window.window, &imgui_handle_blocker);
    install_callbacks(main_window.window);

    // start imgui
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
        // io.ConfigViewportsNoAutoMerge = true;
        // io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(main_window.window, true);
        const char *glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    current_state = RUN;
    set_current_state(&current_state);

    while (current_state != STOP)
    {
        glfwPollEvents();
        tripplebuffer_swap_front(tripplebuffer);
        tripplebuffer_cpy_out_front((void *)&h, tripplebuffer, 0, 1);
        nonstd_glfw_window_get_should_close(&main_window, (int *)&(main_menu_options.file_options.should_close));

        // color update
        {
            x = 1.0 - fabs(fmod(6.0 * h, 2.0) - 1);

            if (h < (1.0 / 6.0))
            {
                red = m + c;
                green = m + x;
                blue = m;
            }
            else if (h < (2.0 / 6.0))
            {
                red = m + x;
                green = m + c;
                blue = m;
            }
            else if (h < (3.0 / 6.0))
            {
                red = m;
                green = m + c;
                blue = m + x;
            }
            else if (h < (4.0 / 6.0))
            {
                red = m;
                green = m + x;
                blue = m + c;
            }
            else if (h < (5.0 / 6.0))
            {
                red = m + x;
                green = m;
                blue = m + c;
            }
            else
            {
                red = m + c;
                green = m;
                blue = m + x;
            }
        }

        // new imgui frame
        {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        // my draw calls
        {
            nonstd_glfw_window_set_clear_color(&main_window, red, green, blue, 1.0f);
            nonstd_glfw_window_draw(&main_window);
        }

        // imgui draw calls
        {
            ShowMainMenu(&main_menu_options);

            if (main_menu_options.tool_options.show_tool_metrics)
                ImGui::ShowMetricsWindow(&(main_menu_options.tool_options.show_tool_metrics));
            if (main_menu_options.tool_options.show_tool_debug_log)
                ImGui::ShowDebugLogWindow(&(main_menu_options.tool_options.show_tool_debug_log));
            if (main_menu_options.tool_options.show_tool_id_stack_tool)
                ImGui::ShowIDStackToolWindow(&(main_menu_options.tool_options.show_tool_id_stack_tool));
            if (main_menu_options.tool_options.show_tool_style_editor)
            {
                ImGui::Begin("Dear ImGui Style Editor", &(main_menu_options.tool_options.show_tool_style_editor));
                ImGui::ShowStyleEditor();
                ImGui::End();
            }
            if (main_menu_options.tool_options.show_tool_about)
                ImGui::ShowAboutWindow(&(main_menu_options.tool_options.show_tool_about));

            if (main_menu_options.file_options.should_close)
            {
                ShowClosePopUp(&(main_menu_options.file_options), &current_state);
                nonstd_glfw_window_set_should_close(&main_window, (int *)&(main_menu_options.file_options.should_close));
                set_current_state(&current_state);
            }

            // Rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow *backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }
        }

        // swap
        nonstd_glfw_window_swap(&main_window);

        get_current_state(&current_state);

        ts.tv_sec = 0l;
        ts.tv_nsec = (long)(dt * 1.0E9);
        while (nanosleep(&ts, &ts))
            ;
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    nonstd_glfw_window_cleanup(&main_window);
    glfwTerminate();

    pthread_exit(NULL);
}

void ShowMainMenu(imgui_main_menu_options_t *menu_options)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ShowFileMenu(&(menu_options->file_options));
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z"))
            {
            }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
            {
            } // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X"))
            {
            }
            if (ImGui::MenuItem("Copy", "CTRL+C"))
            {
            }
            if (ImGui::MenuItem("Paste", "CTRL+V"))
            {
            }
            ImGui::EndMenu();
        }

        ShowTools(&(menu_options->tool_options));
        ImGui::EndMainMenuBar();
    }
}

void ShowFileMenu(imgui_file_options_t *file_options)
{
    if (ImGui::MenuItem("New"))
    {
    }
    if (ImGui::MenuItem("Open", "Ctrl+O"))
    {
    }
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S"))
    {
    }
    if (ImGui::MenuItem("Save As.."))
    {
    }

    ImGui::Separator();
    if (ImGui::BeginMenu("Options"))
    {
        ImGui::MenuItem("Enabled", "", &(file_options->options_enabled));
        ImGui::BeginChild("child", ImVec2(0, 60), ImGuiChildFlags_Border);
        for (int i = 0; i < 10; i++)
            ImGui::Text("Scrolling Text %d", i);
        ImGui::EndChild();
        static float f = 0.5f;
        static int n = 0;
        ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
        ImGui::InputFloat("Input", &f, 0.1f);
        ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Colors"))
    {
        float sz = ImGui::GetTextLineHeight();
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char *name = ImGui::GetStyleColorName((ImGuiCol)i);
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
            ImGui::Dummy(ImVec2(sz, sz));
            ImGui::SameLine();
            ImGui::MenuItem(name);
        }
        ImGui::EndMenu();
    }

    // Here we demonstrate appending again to the "Options" menu (which we already created above)
    // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
    // In a real code-base using it would make senses to use this feature from very different code locations.
    if (ImGui::BeginMenu("Options")) // <-- Append!
    {
        static bool b = true;
        ImGui::Checkbox("SomeOption", &b);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (ImGui::MenuItem("Checked", NULL, true))
    {
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Quit", "Alt+F4"))
    {
        file_options->should_close = true;
    }
}

void ShowTools(imgui_tool_options_t *tool_options)
{
    if (ImGui::BeginMenu("Tools"))
    {
        const bool has_debug_tools = true;
        ImGui::MenuItem("Metrics/Debugger", NULL, &(tool_options->show_tool_metrics), has_debug_tools);
        ImGui::MenuItem("Debug Log", NULL, &(tool_options->show_tool_debug_log), has_debug_tools);
        ImGui::MenuItem("ID Stack Tool", NULL, &(tool_options->show_tool_id_stack_tool), has_debug_tools);
        ImGui::MenuItem("Style Editor", NULL, &(tool_options->show_tool_style_editor));
        ImGui::MenuItem("About Dear ImGui", NULL, &(tool_options->show_tool_about));
        ImGui::EndMenu();
    }
}

void ShowClosePopUp(imgui_file_options_t *file_options, program_state_t *state)
{
    if (file_options->unsaved_changes)
    {
        ImGui::OpenPopup("Save before exiting?");
        if (ImGui::BeginPopupModal("Save before exiting?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("So you want to save before exiting?\nThis operation cannot be undone!");
            ImGui::Separator();

            // ImGui::SetItemDefaultFocus();
            if (ImGui::Button("Save and Exit", ImVec2(120, 0)))
            {
                // TODO: Save
                *state = STOP;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Exit Without Saving", ImVec2(120, 0)))
            {
                *state = STOP;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                file_options->should_close = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    else
    {
        *state = STOP;
    }
}

int imgui_event_handle_blocker(void *ptr, void *e)
{
    ImGuiIO &io = ImGui::GetIO();
    nonstd_glfw_t *nonstd_glfw = (nonstd_glfw_t *)ptr;
    event_t *event = (event_t *)e;

    switch (event->type)
    {
    case WINDOWPOS:
    case WINDOWSIZE:
    case WINDOWCLOSE:
    case WINDOWREFRESH:
    case WINDOWFOCUS:
    case WINDOWICONIFY:
    case WINDOWMAXIMIZE:
    case FRAMEBUFFERSIZE:
    case WINDOWCONTENTSCALE:
        break;
    case MOUSEBUTTON:
    case CURSORPOS:
    case CURSORENTER:
    case SCROLL:
        if (io.WantCaptureMouse)
            return 0;
        break;
    case KEY:
    case CHARPRESS:
    case CHARMODS:
        if (io.WantCaptureKeyboard)
            return 0;
        break;
    case DROP:
    case MONITOR:
    case JOYSTICK:
        break;
    default:
        return 0;
    }

    if (nonstd_glfw->child != NULL && nonstd_glfw->child->event_handler != NULL)
        nonstd_glfw->child->event_handler(nonstd_glfw->child, e);
    if (nonstd_glfw->sibling != NULL && nonstd_glfw->sibling->event_handler != NULL)
        nonstd_glfw->sibling->event_handler(nonstd_glfw->sibling, e);
    return 0;
}
