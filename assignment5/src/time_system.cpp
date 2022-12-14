#include "time_system.h"

float Time::delta_time = zero;
float Time::elapsed_time = zero;
unsigned Time::fixed_update_times_this_frame = zero;

float Time::elapsed_time_last_frame = zero;
float Time::elapsed_time_fixed_update_remaining = zero;

/*static*/ void Time::Update() {
    elapsed_time = float(glfwGetTime());
    delta_time = elapsed_time - elapsed_time_last_frame;
    elapsed_time_last_frame = elapsed_time;

    float elapsed_time_fixedUpdate = delta_time + elapsed_time_fixed_update_remaining;
    fixed_update_times_this_frame = unsigned(std::floor(elapsed_time_fixedUpdate / fixed_delta_time));

    elapsed_time_fixed_update_remaining = elapsed_time_fixedUpdate - float(fixed_update_times_this_frame) * fixed_delta_time;
}
