/*
 * ALETHEIA Self-Learning AI System
 * Advanced AI integration with continuous learning capabilities
 */

#ifndef SELF_LEARNING_AI_H
#define SELF_LEARNING_AI_H

#include <stdbool.h>
#include <stddef.h>

// AI Learning Configuration
#define MAX_LEARNING_SAMPLES 10000
#define MAX_CODE_LENGTH 8192
#define LEARNING_BATCH_SIZE 32
#define CONFIDENCE_UPDATE_RATE 0.01

// Performance Feedback Structure
typedef struct {
    char* original_code;
    char* optimized_code;
    double original_performance;  // Execution time in seconds
    double optimized_performance;
    double improvement_ratio;     // optimized/original
    bool optimization_applied;
    char* optimization_type;
    double ai_confidence;
    long timestamp;
} PerformanceFeedback;

// Learning Sample Structure
typedef struct {
    char* code;
    char* optimization_type;
    double performance_impact;
    double ai_confidence;
    bool successful;
} LearningSample;

// Self-Learning AI System
typedef struct {
    // Model integration
    void* ai_model;  // PyTorch model handle

    // Learning data
    LearningSample* learning_samples;
    int sample_count;
    int max_samples;

    // Performance tracking
    PerformanceFeedback* feedback_history;
    int feedback_count;
    int max_feedback;

    // Learning statistics
    double average_improvement;
    double learning_rate;
    int total_optimizations;
    int successful_optimizations;

    // Continuous learning settings
    bool continuous_learning_enabled;
    int learning_interval;  // Update model every N compilations
    int compilations_since_update;

    // Confidence adaptation
    double confidence_threshold;
    double confidence_learning_rate;

} SelfLearningAI;

// Core Functions
SelfLearningAI* create_self_learning_ai(const char* model_path);
void free_self_learning_ai(SelfLearningAI* ai);

// AI Analysis and Optimization
char* analyze_code_with_ai(SelfLearningAI* ai, const char* code, const char* function_name);
double predict_performance_impact(SelfLearningAI* ai, const char* code, const char* optimization);
bool should_apply_optimization(SelfLearningAI* ai, const char* code, const char* opt_type, double* confidence);

// Learning and Adaptation
void add_performance_feedback(SelfLearningAI* ai,
                            const char* original_code,
                            const char* optimized_code,
                            double original_perf,
                            double optimized_perf,
                            const char* opt_type,
                            double ai_confidence);

void update_ai_model(SelfLearningAI* ai);
void adapt_confidence_threshold(SelfLearningAI* ai);
void save_learning_data(SelfLearningAI* ai, const char* filename);
bool load_learning_data(SelfLearningAI* ai, const char* filename);

// Statistics and Monitoring
void get_ai_statistics(SelfLearningAI* ai, char* buffer, size_t buffer_size);
double get_average_improvement(SelfLearningAI* ai);
int get_successful_optimizations(SelfLearningAI* ai);

// Advanced Features
char* generate_optimization_suggestion(SelfLearningAI* ai, const char* code);
void analyze_compilation_patterns(SelfLearningAI* ai);
bool detect_code_patterns(SelfLearningAI* ai, const char* code, char** patterns, int max_patterns);

// Continuous Learning Control
void enable_continuous_learning(SelfLearningAI* ai, bool enabled);
void set_learning_interval(SelfLearningAI* ai, int interval);
void trigger_learning_update(SelfLearningAI* ai);

// Learning adjustment functions
double adjust_confidence_with_learning(SelfLearningAI* ai, const char* code, const char* optimization_type, double base_confidence);
double adjust_performance_prediction(SelfLearningAI* ai, const char* code, const char* optimization_type, double base_prediction);
double get_success_rate_for_pattern(SelfLearningAI* ai, const char* pattern);

#endif /* SELF_LEARNING_AI_H */
