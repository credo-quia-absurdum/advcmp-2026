#include <array>
#include <cstdint>
#include <iostream>
#include <string>

#include "sim.h"
#include "load_constant.h"

using namespace std;

bool parseConstant(const string &input, int64_t &value)
{
    try
    {
        if (!input.empty() && input.front() == '-')
        {
            value = static_cast<int64_t>(stoll(input, nullptr, 0));
            return true;
        }

        value = static_cast<int64_t>(stoull(input, nullptr, 0));
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void printLoadResult(int64_t imm)
{
    string label = "constant " + formatHex64(static_cast<uint64_t>(imm));
    cout << "\n[START] " << label << endl;
    ProgramEmitter emitter;
    emitLoadConstant(imm, emitter);

    cout << "constant: " << formatHex64(static_cast<uint64_t>(imm))
         << " (" << imm << ")" << endl;
    cout << "instruction count: " << emitter.instructions().size() << endl;

    SimResult result = simulate(static_cast<uint64_t>(imm), emitter.instructions(), true);
    cout << "[END] " << label
         << " -> " << (result.matches_target ? "match" : "mismatch") << endl;
}

void printIntroSamples()
{
    constexpr array<int64_t, 11> samples{
        static_cast<int64_t>(0xCAFE'BABE'DEAD'BEEFull), // fallback baseline case
        static_cast<int64_t>(0x0000'0000'0000'0000ull), // zero
        static_cast<int64_t>(0x0000'0000'0000'07FFull), // signed 12-bit max
        static_cast<int64_t>(0xFFFF'FFFF'FFFF'F800ull), // signed 12-bit min
        static_cast<int64_t>(0x0000'0000'0000'0123ull), // signed 12-bit arbitrary
        static_cast<int64_t>(0x0000'0000'7FFF'FFFFull), // signed 32-bit max
        static_cast<int64_t>(0xFFFF'FFFF'8000'0000ull), // signed 32-bit min
        static_cast<int64_t>(0x0000'0000'1234'5678ull), // signed 32-bit arbitrary
        static_cast<int64_t>(0x0000'0001'0000'0000ull), // 32 trailing zeros
        static_cast<int64_t>(0x0020'0000'0040'0000ull), // 10 leading, 22 trailing zeros
        static_cast<int64_t>(0x0000'0200'0000'0400ull)  // 22 leading, 10 trailing zeros
    };

    for (int64_t imm : samples)
    {
        printLoadResult(imm);
    }
}

int main()
{
    log_instructions = true;

    cout << "loadConstant program" << endl;
    cout << "----------------" << endl;
    printIntroSamples();

    while (true)
    {
        cout << "\nEnter a constant in decimal or hex (0x...) to visualize loading, or q to quit: ";
        string input;
        if (!getline(cin, input))
        {
            break;
        }

        if (input == "q" || input == "quit" || input == "exit")
        {
            break;
        }

        int64_t imm = 0;
        if (!parseConstant(input, imm))
        {
            cout << "Invalid input. Try again." << endl;
            continue;
        }

        printLoadResult(imm);
    }

    return 0;
}
