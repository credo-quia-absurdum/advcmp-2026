#include "sim.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;

bool log_instructions = true;

int64_t signExtendBits(uint64_t value, unsigned bits)
{
    uint64_t mask = (1ull << bits) - 1ull;
    uint64_t signBit = 1ull << (bits - 1);
    uint64_t masked = value & mask;

    if ((masked & signBit) != 0ull)
    {
        masked |= ~mask;
    }

    return static_cast<int64_t>(masked);
}

bool isSignedImmWithinBits(int64_t value, int bits)
{
    int64_t min = -(1LL << (bits - 1));
    int64_t max = (1LL << (bits - 1)) - 1;
    return (value >= min) && (value <= max);
}

bool isUnsignedImmWithinBits(uint64_t value, int bits)
{
    return value < (1ull << bits);
}

void printImmediateRangeError(const char *op, int64_t imm)
{
    cerr << op << " immediate out of range: "
         << imm << " (" << formatHex64(static_cast<uint64_t>(imm)) << ")"
         << endl;
}

void printImmediateRangeError(const char *op, uint64_t imm)
{
    cerr << op << " immediate out of range: "
         << imm << " (" << formatHex64(imm) << ")"
         << endl;
}

void ProgramEmitter::emitLui(int64_t imm)
{
    // lui imm20
    // x = sext(imm20 << 12)
    if (!isSignedImmWithinBits(imm, 20))
    {
        printImmediateRangeError("lui", imm);
    }
    assert(isSignedImmWithinBits(imm, 20));
    instructions_.push_back({SimOp::Lui, imm});
}

void ProgramEmitter::emitAddiw(int64_t imm)
{
    // addiw imm12
    // x = sext((x + sext(imm12))[31:0])
    if (!isSignedImmWithinBits(imm, 12))
    {
        printImmediateRangeError("addiw", imm);
    }
    assert(isSignedImmWithinBits(imm, 12));
    instructions_.push_back({SimOp::Addiw, imm});
}

void ProgramEmitter::emitSlli(uint64_t imm)
{
    // slli shamt6
    // x = x << shamt6
    if (!isUnsignedImmWithinBits(imm, 6))
    {
        printImmediateRangeError("slli", imm);
    }
    assert(isUnsignedImmWithinBits(imm, 6));
    instructions_.push_back({SimOp::Slli, static_cast<int64_t>(imm)});
}

void ProgramEmitter::emitAddi(int64_t imm)
{
    // addi imm12
    // x = x + sext(imm12)
    if (!isSignedImmWithinBits(imm, 12))
    {
        printImmediateRangeError("addi", imm);
    }
    assert(isSignedImmWithinBits(imm, 12));
    instructions_.push_back({SimOp::Addi, imm});
}

void ProgramEmitter::emitSrli(uint64_t imm)
{
    // srli shamt6
    // x = x >>u shamt6
    if (!isUnsignedImmWithinBits(imm, 6))
    {
        printImmediateRangeError("srli", imm);
    }
    assert(isUnsignedImmWithinBits(imm, 6));
    instructions_.push_back({SimOp::Srli, static_cast<int64_t>(imm)});
}

const std::vector<SimInstruction> &ProgramEmitter::instructions() const
{
    return instructions_;
}

void ProgramEmitter::clear()
{
    instructions_.clear();
}

std::string formatHex64(uint64_t v)
{
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setw(16) << std::setfill('0') << v;
    std::string hex = oss.str();

    std::string out = "0x";
    for (std::size_t i = 0; i < 16; ++i)
    {
        out.push_back(hex[i]);
        if ((i % 4) == 3 && i != 15)
        {
            out.push_back('\'');
        }
    }
    return out;
}

std::string formatImmDecimalHex(int64_t imm)
{
    std::ostringstream oss;
    oss << imm << " (" << formatHex64(static_cast<uint64_t>(imm)) << ")";
    return oss.str();
}

std::string formatHexBitsCompact(uint64_t value, unsigned hexDigits)
{
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setw(static_cast<int>(hexDigits))
        << std::setfill('0') << value;
    std::string hex = oss.str();

    std::string out = "0x";
    for (std::size_t i = 0; i < hex.size(); ++i)
    {
        out.push_back(hex[i]);
        if (((hex.size() - i - 1) % 4) == 0 && i + 1 != hex.size())
        {
            out.push_back('\'');
        }
    }
    return out;
}

std::string formatImmDecimalHexBits(uint64_t bits, unsigned width)
{
    std::ostringstream oss;
    oss << static_cast<int64_t>(signExtendBits(bits, width))
        << " (" << formatHexBitsCompact(bits, width == 12 ? 3 : 5) << ")";
    return oss.str();
}

std::string formatInstruction(const SimInstruction &inst)
{
    std::ostringstream oss;

    switch (inst.op)
    {
    case SimOp::Lui:
        oss << "lui " << formatHexBitsCompact(static_cast<uint64_t>(inst.imm) & 0xFFFFFull, 5);
        break;
    case SimOp::Addiw:
        oss << "addiw "
            << formatImmDecimalHexBits(static_cast<uint64_t>(inst.imm) & 0xFFFull, 12);
        break;
    case SimOp::Slli:
        oss << "slli " << inst.imm;
        break;
    case SimOp::Addi:
        oss << "addi "
            << formatImmDecimalHexBits(static_cast<uint64_t>(inst.imm) & 0xFFFull, 12);
        break;
    case SimOp::Srli:
        oss << "srli " << inst.imm;
        break;
    }

    return oss.str();
}

SimResult simulate(uint64_t target, const std::vector<SimInstruction> &program, bool verbose)
{
    uint64_t reg = 0;
    int step = 0;
    const int step_width = 5;
    const int instr_width = 22;
    const int reg_width = 22;
    const int target_width = 22;
    const int status_width = 10;

    auto printSeparator = [&](std::initializer_list<int> widths)
    {
        for (int width : widths)
        {
            cout << "+";
            for (int i = 0; i < width + 2; ++i)
            {
                cout << "-";
            }
        }
        cout << "+";
        cout << "\n";
    };

    if (verbose && log_instructions)
    {
        printSeparator({step_width, instr_width, reg_width, target_width, status_width});

        cout << "| " << std::left << std::setw(step_width) << "Step"
             << " | " << std::left << std::setw(instr_width) << "Instruction"
             << " | " << std::left << std::setw(reg_width) << "Reg"
             << " | " << std::left << std::setw(target_width) << "Target"
             << " | " << std::left << std::setw(status_width) << "Match"
             << " |\n";

        printSeparator({step_width, instr_width, reg_width, target_width, status_width});
    }

    for (const SimInstruction &inst : program)
    {
        ++step;

        switch (inst.op)
        {
        case SimOp::Lui:
            // lui imm20
            // x = sext(imm20 << 12)
            reg = static_cast<uint64_t>(signExtendBits(static_cast<uint64_t>(inst.imm) << 12, 32));
            break;
        case SimOp::Addiw:
            // addiw imm12
            // x = sext((x + sext(imm12))[31:0])
            {
                int64_t imm = static_cast<int64_t>(signExtendBits(static_cast<uint64_t>(inst.imm), 12));
                uint64_t low32 = (reg + static_cast<uint64_t>(imm)) & 0xFFFF'FFFFull;
                reg = static_cast<uint64_t>(signExtendBits(low32, 32));
            }
            break;
        case SimOp::Slli:
            // slli shamt6
            // x = x << shamt6
            {
                uint64_t shamt = static_cast<uint64_t>(inst.imm) & 0x3Full;
                reg <<= shamt;
            }
            break;
        case SimOp::Addi:
            // addi imm12
            // x = x + sext(imm12)
            reg += static_cast<uint64_t>(signExtendBits(static_cast<uint64_t>(inst.imm), 12));
            break;
        case SimOp::Srli:
            // srli shamt6
            // x = x >>u shamt6
            {
                uint64_t shamt = static_cast<uint64_t>(inst.imm) & 0x3Full;
                reg >>= shamt;
            }
            break;
        }

        if (verbose && log_instructions)
        {
            bool matches = reg == target;
            std::string match_txt = matches ? "match" : "mismatch";

            cout << "| " << std::left << std::setw(step_width) << step
                 << " | " << std::left << std::setw(instr_width) << formatInstruction(inst)
                 << " | " << std::left << std::setw(reg_width) << formatHex64(reg)
                 << " | " << std::left << std::setw(target_width) << formatHex64(target)
                 << " | " << std::left << std::setw(status_width) << match_txt
                 << " |\n";
        }
    }

    bool matches_target = reg == target;

    if (verbose)
    {
        if (log_instructions)
        {
            printSeparator({step_width, instr_width, reg_width, target_width, status_width});
            cout << "\n";
        }

        cout << "final: target=" << formatHex64(target)
             << ", result=" << formatHex64(reg)
             << ", " << (matches_target ? "match" : "mismatch") << endl;
    }

    return {reg, matches_target, step};
}
