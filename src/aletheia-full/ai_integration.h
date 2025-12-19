// ALETHEIA-Full AI Integration Header
// Interface between ALETHEIA compiler and AI optimization system

#ifndef ALETHEIA_AI_INTEGRATION_H
#define ALETHEIA_AI_INTEGRATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// AI Optimization Result Structure
typedef struct {
    char* optimization_name;
    char* description;
    double confidence_score;
    bool is_applicable;
    char* code_suggestion;
} AIOptimizationSuggestion;

// AI Analysis Result Structure
typedef struct {
    char* function_name;
    char* code_snippet;
    int complexity_score;
    int optimization_count;
    AIOptimizationSuggestion* suggestions;
    int suggestion_capacity;
    int suggestion_count;
    double average_confidence;
} AIAnalysisResult;

// AI Integration API Functions

/**
 * Initialize AI optimization system
 * @return true if AI system is ready, false otherwise
 */
bool ai_init(void);

/**
 * Shutdown AI optimization system
 */
void ai_shutdown(void);

/**
 * Analyze C code snippet with AI
 * @param code_snippet The C code to analyze
 * @param function_name Name of the function being analyzed
 * @return AI analysis result with optimization suggestions
 */
AIAnalysisResult* ai_analyze_code(const char* code_snippet, const char* function_name);

/**
 * Apply AI-suggested optimizations to code
 * @param original_code The original C code
 * @param analysis_result AI analysis with suggestions
 * @return Optimized C code with AI suggestions applied
 */
char* ai_apply_optimizations(const char* original_code, AIAnalysisResult* analysis_result);

/**
 * Check if AI system is available and operational
 * @return true if AI can be used, false otherwise
 */
bool ai_is_available(void);

/**
 * Get AI system version and capabilities
 * @return String describing AI system capabilities
 */
const char* ai_get_capabilities(void);

/**
 * Free AI analysis result memory
 * @param result The result to free
 */
void ai_free_result(AIAnalysisResult* result);

// Integration with ALETHEIA-Full compilation pipeline

/**
 * Hook called during parsing phase to collect function code for AI analysis
 * @param function_name Name of the function being parsed
 * @param function_code Complete function code as string
 */
void ai_collect_function_for_analysis(const char* function_name, const char* function_code);

/**
 * Hook called during optimization phase to apply AI suggestions
 * @param function_name Name of the function being optimized
 * @return Optimized function code with AI suggestions applied
 */
char* ai_optimize_function(const char* function_name);

/**
 * Hook called during code generation to emit AI-optimized code
 * @param function_name Name of the function
 * @param optimized_code The AI-optimized code
 */
void ai_emit_optimized_code(const char* function_name, const char* optimized_code);

// Configuration and tuning

/**
 * Set AI confidence threshold for automatic optimization application
 * @param threshold Confidence score threshold (0.0 to 1.0)
 */
void ai_set_confidence_threshold(double threshold);

/**
 * Enable/disable specific optimization categories
 * @param category Optimization category name
 * @param enabled true to enable, false to disable
 */
void ai_set_optimization_category(const char* category, bool enabled);

/**
 * Get AI optimization statistics
 * @return String with optimization statistics
 */
const char* ai_get_statistics(void);

// Self-learning AI functions

/**
 * Measure optimization performance and provide feedback for learning
 * @param original_code Original unoptimized code
 * @param optimized_code Code after AI optimization
 * @param original_performance Original execution time (seconds)
 * @param optimized_performance Optimized execution time (seconds)
 * @param optimization_type Type of optimization applied
 * @param ai_confidence AI confidence in the optimization
 */
void ai_measure_performance(const char* original_code,
                           const char* optimized_code,
                           double original_performance,
                           double optimized_performance,
                           const char* optimization_type,
                           double ai_confidence);

/**
 * Trigger manual learning update from accumulated feedback
 */
void ai_trigger_learning(void);

/**
 * Get detailed learning progress and self-improvement statistics
 * @return String with comprehensive learning progress information
 */
const char* ai_get_learning_progress(void);

#endif // ALETHEIA_AI_INTEGRATION_H
