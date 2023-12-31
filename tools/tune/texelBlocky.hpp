#include "texel-tuner/src/base.h"
#include "texel-tuner/src/external/chess.hpp"

namespace Blocky {

// This is an interface for a texel-tuner used for the development for Blocky:
// https://github.com/GediminasMasaitis/texel-tuner
class BlockyEval
    {
    public:
        constexpr static bool includes_additional_score = false;
        constexpr static bool supports_external_chess_eval = false;

        static parameters_t get_initial_parameters();
        static EvalResult get_fen_eval_result(const std::string& fen);
        static EvalResult get_external_eval_result(const chess::Board& board); // unused
        static void print_parameters(const parameters_t& parameters);
    };

void printPSQT(const parameters_t& parameters, int index);
void printArr(const parameters_t& parameters, int offset, int index);
void printCoeff(const parameters_t& parameters, int index); // for debugging

}; // namespace Blocky