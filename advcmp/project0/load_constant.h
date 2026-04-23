#ifndef LOAD_CONSTANT_H
#define LOAD_CONSTANT_H

#include <cstdint>

#include "sim.h"

// Assignment: implement only load_constant.cpp.
// - Do not change sim.h or main.cpp.
// - emitLoadConstant must build a program using only instructions documented in sim.h.
// - The baseline fallback implementation emits exactly 8 instructions.

void emitLoadConstant(int64_t targetConstant, ProgramEmitter &emitter);

#endif
