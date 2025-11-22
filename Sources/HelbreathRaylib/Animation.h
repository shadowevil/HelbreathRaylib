#pragma once
#include <cstdint>

struct Animation
{
    // ------------------------------------------------------------
    // Configuration
    // ------------------------------------------------------------
    size_t  max_frame = 0;        // total frames
    double  frame_delay = 0.1;      // seconds per frame
    bool    loop = true;     // loop or stop at end
    bool    reverse = false;    // play backwards
    bool    empty = false;    // marks an "empty" animation

    // ------------------------------------------------------------
    // State
    // ------------------------------------------------------------
    size_t current_frame = 0;
    double last_frame_time = 0.0;

    // ------------------------------------------------------------
    // Constructors
    // ------------------------------------------------------------
    Animation()
		: empty(true)
    { }

    Animation(size_t start, size_t count, double delay,
        bool looped = true,
        bool reversed = false)
        : max_frame(count),
        frame_delay(delay),
        loop(looped),
        reverse(reversed),
        empty(false),
        current_frame(start),
        last_frame_time(0.0)
    {
        reset();
    }

    Animation(const Animation& other)
        : max_frame(other.max_frame),
        frame_delay(other.frame_delay),
        loop(other.loop),
        reverse(other.reverse),
        empty(other.empty)
    {
        reset();
    }

    Animation& operator=(const Animation& other)
    {
        if (this != &other)
        {
            max_frame = other.max_frame;
            frame_delay = other.frame_delay;
            loop = other.loop;
            reverse = other.reverse;
            empty = other.empty;

            reset();
        }
        return *this;
    }

    // ------------------------------------------------------------
    // Reset
    // ------------------------------------------------------------
    void reset()
    {
        current_frame = (reverse && max_frame > 0) ? max_frame - 1 : 0;
        last_frame_time = 0.0;
    }

    // ------------------------------------------------------------
    // Update (for normal and reversed playback)
    // ------------------------------------------------------------
    void update(double current_time)
    {
        if (empty) return;       // do nothing
        if (max_frame <= 1) return;

        if ((current_time - last_frame_time) < frame_delay)
            return;

        last_frame_time = current_time;

        if (!reverse)
        {
            if (loop)
                current_frame = (current_frame + 1) % max_frame;
            else if (current_frame + 1 < max_frame)
                current_frame++;
        }
        else // reversed
        {
            if (loop)
                current_frame = (current_frame == 0)
                ? max_frame - 1
                : current_frame - 1;
            else if (current_frame > 0)
                current_frame--;
        }
    }

    // ------------------------------------------------------------
    // Convenience: returns last valid frame for non-loop animations
    // ------------------------------------------------------------
    bool is_finished() const
    {
        return (!loop && !reverse && current_frame == max_frame - 1) ||
            (!loop && reverse && current_frame == 0);
    }
};