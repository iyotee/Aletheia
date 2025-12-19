/*
 * ALETHEIA-Full: Main Entry Point
 *
 * Command-line interface for the complete AI-optimized C compiler.
 */

#define BOOTSTRAP_BUILD 1
#include "compiler_adapter.h"

/* Show usage information */
void show_usage() {
    const char* usage =
        "ALETHEIA-Full " ALETHEIA_FULL_VERSION " - Complete AI-Optimized C Compiler\n"
        "\n"
        "USAGE:\n"
        "  aletheia-full [options] <input_file> [output_file]\n"
        "\n"
        "OPTIONS:\n"
        "  -O0, -O1, -O2, -O3    Optimization level (default: -O2)\n"
        "  -g                    Generate debug information\n"
        "  -v, --verbose         Verbose output\n"
        "  --ai-basic            Enable basic AI optimization\n"
        "  --ai-advanced         Enable advanced AI optimization\n"
        "  --security-scan       Enable security vulnerability scanning\n"
        "  --performance         Enable performance analysis\n"
        "  --target=<arch>       Target architecture (x86_64, arm64, riscv)\n"
        "  --version             Show version information\n"
        "  --help                Show this help message\n"
        "\n"
        "EXAMPLES:\n"
        "  aletheia-full hello.c\n"
        "  aletheia-full -O3 --ai-advanced --security-scan program.c output.s\n"
        "\n"
        "BOOTSTRAP CHAIN:\n"
        "  Built using: Stage0 → MesCC-ALE → TinyCC-ALE → ALETHEIA-Core → ALETHEIA-Full\n";

    /* In bootstrap mode, we can't print directly */
    /* The usage would be shown in a real implementation */
}

/* Parse command line arguments */
ALETHEIAConfig parse_arguments(int argc, char* argv[]) {
    ALETHEIAConfig config = {
        .optimization_level = 2,
        .debug_info = 0,
        .verbose = 0,
        .ai_level = AI_OPT_NONE,
        .learning_enabled = 0,
        .security_scan = 0,
        .performance_analysis = 0,
        .target_arch = "x86_64",
        .bootstrap_mode = 1
    };

    /* Parse arguments (simplified for bootstrap) */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-O0") == 0) config.optimization_level = 0;
        else if (strcmp(argv[i], "-O1") == 0) config.optimization_level = 1;
        else if (strcmp(argv[i], "-O2") == 0) config.optimization_level = 2;
        else if (strcmp(argv[i], "-O3") == 0) config.optimization_level = 3;
        else if (strcmp(argv[i], "-g") == 0) config.debug_info = 1;
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) config.verbose = 1;
        else if (strcmp(argv[i], "--ai-basic") == 0) config.ai_level = AI_OPT_BASIC;
        else if (strcmp(argv[i], "--ai-advanced") == 0) config.ai_level = AI_OPT_ADVANCED;
        else if (strcmp(argv[i], "--security-scan") == 0) config.security_scan = 1;
        else if (strcmp(argv[i], "--performance") == 0) config.performance_analysis = 1;
        else if (strncmp(argv[i], "--target=", 9) == 0) config.target_arch = argv[i] + 9;
        else if (strcmp(argv[i], "--version") == 0) {
            /* Version already handled above */
            exit(0);
        }
        else if (strcmp(argv[i], "--help") == 0) {
            show_usage();
            exit(0);
        }
        else if (!config.input_file) {
            config.input_file = argv[i];
        }
        else if (!config.output_file) {
            config.output_file = argv[i];
        }
    }

    return config;
}

/* Main entry point */
int main(int argc, char* argv[]) {
    /* Initialize AI optimizer */
    ai_init_optimizer();

    /* Initialize security scanner */
    security_init_scanner();

    if (argc < 2) {
        show_usage();
        return 1;
    }

    /* Handle version separately */
    if (strcmp(argv[1], "--version") == 0) {
        /* Version information would be printed in full implementation */
        return 0;
    }

    /* Parse command line arguments */
    ALETHEIAConfig config = parse_arguments(argc, argv);

    if (!config.input_file) {
        /* No input file specified */
        return 1;
    }

    /* Set default output file */
    if (!config.output_file) {
        config.output_file = "a.out";
    }

    /* AI prediction phase */
    if (config.ai_level > AI_OPT_NONE) {
        /* In full implementation, we would read the input file first */
        /* For bootstrap, we skip this */
        ai_predict_optimizations("", &config);
    }

    /* Compile the input file */
    int result = aletheia_compile_file(config.input_file, config.output_file, &config);

    /* AI learning phase */
    if (config.learning_enabled && result == 0) {
        /* Learn from successful compilation */
        ai_learn_from_compilation("", "");
    }

    return result;
}

