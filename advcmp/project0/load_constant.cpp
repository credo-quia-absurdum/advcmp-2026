#include <cstdint>
#include <iostream>
#include <cassert>

#include "load_constant.h"

static uint64_t signExtendBits(uint64_t value, unsigned bits)
{
    assert(bits > 0 && bits < 64);

    uint64_t mask = (1ull << bits) - 1ull;
    uint64_t signBit = 1ull << (bits - 1);
    uint64_t masked = value & mask;

    if ((masked & signBit) != 0ull)
    {
        masked |= ~mask;
    }

    return masked;
}

void emitLoadConstant(int64_t targetConstant, ProgramEmitter &emitter)
{
    emitter.clear();
    uint64_t targetBits = static_cast<uint64_t>(targetConstant);

    // Zero constant
    if (targetConstant == 0)
    {
        std::cout << "case: zero constant" << std::endl;

        // The target constant is exactly zero.
        // Emit at least one instruction.

        // TODO: Your Implementation Here

        return;
    }

    // Signed 12-bit constant
    if ((targetBits & 0xFFFFFFFFFFFFF800ull) == 0ull ||
        (targetBits & 0xFFFFFFFFFFFFF800ull) == 0xFFFFFFFFFFFFF800ull)
    {
        std::cout << "case: signed 12-bit constant" << std::endl;

        // The target constant is representable as a signed 12-bit integer.
        // You can implement this case with one instruction.

        // TODO: Your Implementation Here

        return;
    }

    // Signed 32-bit constant
    const uint64_t upper33Bits = targetBits & 0xFFFFFFFF80000000ull;
    if (upper33Bits == 0ull || upper33Bits == 0xFFFFFFFF80000000ull)
    {
        std::cout << "case: signed 32-bit constant" << std::endl;

        // The target constant is representable as a signed 32-bit integer.
        // You can implement this case with two instructions.

        // TODO: Your Implementation Here

        return;
    }

    // Constant with at least 32 leading and trailing zeros in total
    const unsigned leadingZeroBits = static_cast<unsigned>(__builtin_clzll(targetBits));
    const unsigned trailingZeroBits = static_cast<unsigned>(__builtin_ctzll(targetBits));
    if ((leadingZeroBits + trailingZeroBits) >= 32)
    {
        std::cout << "case: constant with at least 32 leading and trailing zeros in total" << std::endl;

        // The total number of leading and trailing zeros is at least 32.
        // You can finish this case with four instructions.

        // TODO: Your Implementation Here

        return;
    }

    // if no earlier case matches, use the fixed-width fallback baseline.
    // Sequence is always:
    // lui, addiw, slli, addi, slli, addi, slli, addi.
    // Shifts are 11, 11, 10; chunk widths are 11, 11, 10.
    // This is a deterministic fallback implementation.
    std::cout << "case: fallback 8-instruction baseline" << std::endl;

    // Build the upper 32-bit word with lui + addiw.
    // upper12 is the signed 12-bit addiw immediate taken from the low 12 bits
    // of that upper 32-bit word.
    int64_t upper12 = static_cast<int64_t>(signExtendBits(targetBits >> 32, 12));

    // addiw always adds a sign-extended 12-bit immediate.
    // When upper12 is negative, that immediate is represented with leading 1s,
    // so the effect is the same as subtracting from the lui result.
    // Compensate for that by adding 1 to the upper 20-bit part before emitLui.
    int64_t upper20 = static_cast<int64_t>(signExtendBits((targetBits >> 44) + (upper12 < 0), 20));

    emitter.emitLui(upper20);
    emitter.emitAddiw(upper12);

    // Append the remaining low 32 bits as 11 / 11 / 10-bit chunks.
    emitter.emitSlli(11);
    emitter.emitAddi(static_cast<int64_t>((targetBits >> 21) & 0x7FF));

    emitter.emitSlli(11);
    emitter.emitAddi(static_cast<int64_t>((targetBits >> 10) & 0x7FF));

    emitter.emitSlli(10);
    emitter.emitAddi(static_cast<int64_t>(targetBits & 0x3FF));
}
