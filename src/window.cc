#include "window.h"
#include "ctrl/camera.h"
#include "ctrl/key_input.h"
#include "ctrl/mouse_input.h"
#include "gfx/block.h"
#include "gui/menu_bar.h"
#include "imgui_multiplatform/imgui.h"
#include "level_editor/settings_menu.h"
#include "resources.h"
#include "trigonometry.h"
#include <spdlog/spdlog.h>


namespace vx {
    static GLFWwindow *window;
    static std::shared_ptr<ctrl::Camera> camera = std::make_shared<ctrl::Camera>();
    static std::unique_ptr<ctrl::MouseInput> input = std::make_unique<ctrl::MouseInput>();
    static std::unique_ptr<gui::Menubar> menubar = std::make_unique<gui::Menubar>();
    static bgfx::Init init;
    static ivec2 windowDimensions(1280, 720);

    static void glfwErrorCallback(int err, const char *msg) { spdlog::error("GLFW Error {}: {}", err, msg); }

    static void glfwCursorPosCallback(GLFWwindow *_window, double xpos, double ypos) {
        input->handleCursorPos(_window, xpos, ypos, camera);
    }

    static void glfwMouseButtonCallback(GLFWwindow *_window, int button, int action, int mods) {
        input->handleMouseButtonPress(_window, button, action, mods, camera);
    }

    static void glfwKeyCallback(GLFWwindow *_window, int key, int scancode, int action, int mods) {
        ctrl::KeyInput::getInstance()->handleKeyPressEvent(key, scancode, action, mods);
    }

    static void glfwScrollCallback(GLFWwindow *_window, double xoffset, double yoffset) {
        input->handleScrollEvent(xoffset, yoffset, camera);
    }

    static void glfwResizeCallback(GLFWwindow *window, int width, int height) {
        spdlog::debug("Resizing w: {}, h: {}", width, height);
        bgfx::reset(width, height, BGFX_RESET_VSYNC, init.resolution.format);
        camera->resize(width, height);
    }

    static void glfwWindowPosCallback(GLFWwindow *window, int xpos, int ypos) {
        spdlog::debug("Window pos x: {}, y: {}", xpos, ypos);
    }

    auto initializeWindow(const std::string &windowTitle) -> bool {
        spdlog::info("Loading main window");
        glfwSetErrorCallback(glfwErrorCallback);

        if (!glfwInit()) {
            spdlog::error("Error initializing glfw");
            return false;
        }

        // Turn off resize
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();

        if (!monitor) {
            spdlog::error("Could not find primary monitor");
            return false;
        }

        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        if (!mode) {
            spdlog::error("Could not get video mode");
            return false;
        }
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        windowDimensions = ivec2(mode->width, mode->height);
        spdlog::debug("Dimensions: {}", glm::to_string(windowDimensions));
        window = glfwCreateWindow(windowDimensions.x, windowDimensions.y, windowTitle.c_str(), nullptr, nullptr);

        if (!window) {
            spdlog::error("Error creating GLFW Window");
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);

        glfwSetCursorPosCallback(window, glfwCursorPosCallback);
        glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
        glfwSetScrollCallback(window, glfwScrollCallback);
        glfwSetWindowSizeCallback(window, glfwResizeCallback);
        // glfwSetKeyCallback(window, glfwKeyCallback);
        glfwSetWindowPosCallback(window, glfwWindowPosCallback);

        return true;
    }

    auto initializeBgfx(bgfx::ProgramHandle &program) -> bool {
        // Tell bgfx to not create a separate render thread
        bgfx::renderFrame();

        bgfx::PlatformData platformData;

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if ENTRY_CONFIG_USE_WAYLAND
        platformData.ndt = glfwGetWaylandDisplay();
#else
        platformData.ndt = glfwGetX11Display();
        platformData.nwh = (void *) glfwGetX11Window(window);
#endif

#elif BX_PLATFORM_OSX
        platformData.ndt = nullptr;
        platformData.nwh = glfwGetCocoaWindow(window);
#elif BX_PLATFORM_WINDOWS
        platformData.ndt = nullptr;
        platformData.nwh = glfwGetWin32Window(window);
#endif

        init.platformData = platformData;

// Use metal for macs and vulkan for everything else
#ifdef __APPLE__
        init.type = bgfx::RendererType::Metal;
#else
        init.type = bgfx::RendererType::OpenGL;
#endif
        init.resolution.width = windowDimensions.x;
        init.resolution.height = windowDimensions.y;
        init.resolution.reset = BGFX_RESET_VSYNC;
        bgfx::init(init);

        bgfx::reset(windowDimensions.x, windowDimensions.y, BGFX_RESET_VSYNC, init.resolution.format);
        bgfx::setDebug(BGFX_DEBUG_TEXT);
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x32323232, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);

#if BX_PLATFORM_WINDOWS
        const std::string shaderResourcePath = "../resources/shaders";
#else
        const std::string shaderResourcePath = "resources/shaders";
#endif

        program = vx::loadShaderProgram(shaderResourcePath, "core");
        imguiCreate();
        return true;
    }

    auto launchWindow(const std::string &windowTitle) -> int {
        camera->resize(windowDimensions.x, windowDimensions.y);
        if (!initializeWindow(windowTitle)) {
            spdlog::error("Window initialization failed");
            return EXIT_FAILURE;
        }

        bgfx::ProgramHandle program;
        if (!initializeBgfx(program)) {
            spdlog::error("Renderer initialization failed");
            return EXIT_FAILURE;
        }

        menubar->registerMenu(level_editor::showSettingsMenu);

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            //==============================
            const auto currentMousePosition = input->currentMousePos();
            imguiBeginFrame(currentMousePosition.x, currentMousePosition.y, input->mouseButtonImgui(), 0,
                            windowDimensions.x, windowDimensions.y);
            menubar->render(window);
            imguiEndFrame();
            //==============================

            bgfx::touch(0);

            bgfx::dbgTextClear();
            bgfx::dbgTextPrintf(0, 0, 0x6f, "");

            bgfx::setViewTransform(0, &camera->viewMatrix(), &camera->projectionMatrix());

            bgfx::setViewRect(0, 0, 0, windowDimensions.x, windowDimensions.y);

            //! THIS CAUSES A SEGFAULT
            // fixtures::getBaseLayerFixture().renderer->render(program);

            glfwSwapBuffers(window);
            bgfx::frame();
        }

        bgfx::destroy(program);
        spdlog::info("Deleting buffers");
        imguiDestroy();

        //! THIS CAUSES A SEGFAULT
        // fixtures::getBaseLayerFixture().renderer->destroy();
        bgfx::shutdown();
        glfwTerminate();

        return 0;
    }
}// namespace vx
