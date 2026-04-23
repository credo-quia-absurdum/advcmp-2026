#ifndef SIM_H
#define SIM_H

#include <cstdint>
#include <string>
#include <vector>

// RISC-V instructions used by this assignment simulator.
//
// All instructions operate on a single working register.
// The register starts from zero.
// Instructions are applied in order.
//
// Instruction lui
// Description:
//   Uses a 20-bit immediate to construct bits 31:12 of the low 32-bit word
//   and fills bits 11:0 with zeros. The resulting 32-bit value is then
//   sign-extended to the full 64-bit working register.
// Immediate constraint:
//   The immediate used for lui must be representable as a signed 20-bit value.
// Effect:
//   x <- sext(imm20 << 12), where "sext" means sign-extension
//
// Instruction addiw
// Description:
//   Adds a sign-extended 12-bit immediate to the current working register value.
//   Any overflow beyond the low 32 bits is ignored. The low 32-bit result is
//   then sign-extended back to the full 64-bit working register.
// Immediate constraint:
//   The immediate used for addiw must be representable as a signed 12-bit value.
// Effect:
//   x <- sext((x + sext(imm12))[31:0])
//
// Instruction addi
// Description:
//   Adds a sign-extended 12-bit immediate to the current working register value.
//   Arithmetic overflow is ignored, and the result is taken directly in the full
//   64-bit register width.
// Immediate constraint:
//   The immediate used for addi must be representable as a signed 12-bit value.
// Effect:
//   x <- x + sext(imm12)
//
// Instruction slli
// Description:
//   Performs a logical left shift on the current working register value.
//   Bits shifted out on the left are discarded, and zeros are inserted into
//   the low bits.
// Immediate constraint:
//   The shift amount used for slli must be representable as an unsigned 6-bit value in RV64.
// Effect:
//   x <- x << shamt6
//
// Instruction srli
// Description:
//   Performs a logical right shift on the current working register value.
//   Bits shifted out on the right are discarded, and zeros are inserted into
//   the high bits.
// Immediate constraint:
//   The shift amount used for srli must be representable as an unsigned 6-bit value in RV64.
// Effect:
//   x <- x >>u shamt6
enum class SimOp
{
    Lui,
    Addiw,
    Addi,
    Slli,
    Srli,
};

struct SimInstruction
{
    SimOp op;
    int64_t imm;
};

struct SimResult
{
    uint64_t final_reg;
    bool matches_target;
    int instruction_count;
};

class ProgramEmitter
{
public:
    void emitLui(int64_t imm);
    void emitAddiw(int64_t imm);
    void emitAddi(int64_t imm);
    void emitSlli(uint64_t imm);
    void emitSrli(uint64_t imm);

    const std::vector<SimInstruction> &instructions() const;
    void clear();

private:
    std::vector<SimInstruction> instructions_;
};

extern bool log_instructions;

std::string formatHex64(uint64_t v);
std::string formatInstruction(const SimInstruction &inst);
SimResult simulate(uint64_t target, const std::vector<SimInstruction> &program, bool verbose = true);

#endif
