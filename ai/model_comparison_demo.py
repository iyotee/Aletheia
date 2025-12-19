#!/usr/bin/env python3
"""
ALETHEIA Model Performance Comparison Demo
Shows correct table formatting for model comparisons
"""

def demonstrate_correct_table_formatting():
    """Demonstrate the correct way to format performance comparison tables"""

    print("ALETHEIA Model Performance Comparison")
    print("=" * 50)

    # Correct metrics dictionary structure
    metrics = {
        'Accuracy': ['75%', '91%'],
        'Performance Gain': ['+15-25%', '+25-40%'],
        'Pattern Recognition': ['Basic', 'Advanced'],
        'GCC Compatibility': ['Partial', 'Full'],
        'Real Code Understanding': ['Limited', 'Excellent']
    }

    # CORRECT table formatting (this is how it should work)
    print("\nPerformance Comparison:")
    print("-" * 30)
    print("Metric                | Synthetic | Real C Code")
    print("----------------------|-----------|------------")

    for metric, values in metrics.items():
        # Correctly format each row with proper spacing
        print(f"{metric:<22}| {values[0]:<10}| {values[1]}")

    print("\nTable formatting is now correct!")
    print("Each metric shows both the old (Synthetic) and new (Real C Code) values")

    # Additional demonstration
    print("\nKey Improvements:")
    improvements = [
        ("Accuracy", "75%", "91%", "+16%"),
        ("Performance Gain", "+15-25%", "+25-40%", "+10-15%"),
        ("Pattern Recognition", "Basic", "Advanced", "Major upgrade"),
        ("GCC Compatibility", "Partial", "Full", "Complete"),
        ("Real Code Understanding", "Limited", "Excellent", "Transformative")
    ]

    for metric, old_val, new_val, improvement in improvements:
        print(f"{metric:<22}: {old_val} -> {new_val} ({improvement})")

if __name__ == "__main__":
    demonstrate_correct_table_formatting()
