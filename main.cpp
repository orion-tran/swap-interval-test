#include <iostream>
#include <chrono>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

uint64_t get_highest_refresh_rate() {
    int highest = 0l;

    int monitor_count;
    GLFWmonitor **monitors = glfwGetMonitors(&monitor_count);

    for (int i = 0; i < monitor_count; ++i) {
        int refresh_rate = glfwGetVideoMode(monitors[i])->refreshRate;
        if (refresh_rate > highest) highest = refresh_rate;
    }

    return highest;
}

void exit_with_message(char* arg) {
    printf("Please launch this program as 'GLFW_FIXED=TRUE %s' (you can use TRUE/FALSE/HALF)\n", arg);
}

int main(int argc, char **argv) {
    const char* var = std::getenv("GLFW_FIXED");
    if (var == nullptr) {
        exit_with_message(argv[0]);
        return 0;
    }
    std::string fixed(var);
    if (fixed.empty()) {
        exit_with_message(argv[0]);
        return 0;
    }

    bool fixed_true = fixed == "TRUE";
    bool fixed_false = fixed == "FALSE";
    bool fixed_half = fixed == "HALF";

    if (!fixed_true && !fixed_false && !fixed_half) {
        printf("Please use either TRUE or FALSE or HALF (case sensitive)!\n");
    }

    glfwInit();

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_RESIZABLE, false);
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Swap Interval Test", nullptr, nullptr);
    // opengl does not like it when the window has a content area of 0
    glfwSetWindowSizeLimits(window, 320, 180, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(window);

    // for the fix, we need to set this value to 0 (if you are unaffected, 1 is perfectly fine to use!)
    glfwSwapInterval(fixed_true || fixed_half ? 0 : 1);
    glewExperimental = true;
    glewInit();

    printf("Renderer info:\n");
    printf("- OpenGL Version %s (Shading language version: %s)\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("- GPU: %s (Vendor: %s)\n\n", glGetString(GL_RENDERER), glGetString(GL_VENDOR));

    printf("Try dragging the window around!\n\n");

    printf("On affected systems, setting the fixed mode to:\n");
    printf("TRUE should be smoothly draggable, without consuming the entire thread\n");
    printf("HALF should be smoothly draggable, but it consumes the entire thread\n");
    printf("FALSE should lag crazily while dragging, and it does not consume the entire thread\n\n");

    printf("On unaffected systems, you should feel absolutely no difference in turning the variable from TRUE/FALSE and dragging the window\n");

    // Feel free to use this code for free in perpetuity throughout the universe
    // without attribution or payment! I am not liable or responsible for what you
    // do with this code, though.

    // NOTE: this is technically a HACK and should only be used in lieu of a properly working implementation of
    // display synchronization

    uint64_t time_scale = glfwGetTimerFrequency();
    // the time coefficient is there to account for computers who count in any timescale
    // greater than ns (this will break for any timescale smaller than ns though)
    uint64_t time_coefficient = time_scale / 1000000000l;

    // I recommend personally using 2x the main monitor's refresh rate or making this user configurable!
    uint64_t frame_limit = time_scale / (get_highest_refresh_rate() * 2);

    uint64_t frame_start;
    uint64_t delta_time;

    while (!glfwWindowShouldClose(window)) {
        frame_start = glfwGetTimerValue();

        // --> put your rendering of the frame's code here <--
        // for now it's just a completely blank slate (the screen should just be black)

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        // do not do the fancy wait when it is not fixed
        if (!fixed_true) continue;

        delta_time = glfwGetTimerValue() - frame_start;
        while (delta_time < frame_limit) {
            // will basically pause using the (time remaining) / 4, allows it to keep getting
            // closer to the resume time of the next frame and go over only by a tiny bit
            std::this_thread::sleep_for(std::chrono::nanoseconds(time_coefficient * (frame_limit - delta_time) / 4l));
            delta_time = glfwGetTimerValue() - frame_start;
        }
    }
}
