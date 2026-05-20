#pragma once

namespace pos {

class PosBeep {
public:
    /** Load beep into memory (optional; first play() loads automatically). */
    static void preload();

    /** Play the same short mono beep used by the web POS when an item is added. */
    static void play();
};

} // namespace pos
