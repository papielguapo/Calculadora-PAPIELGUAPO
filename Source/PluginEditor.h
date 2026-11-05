#pragma once

#include "PluginProcessor.h"

class Calculadora1a1AudioProcessorEditor final : public juce::AudioProcessorEditor,
                                                 private juce::Button::Listener
{
public:
    explicit Calculadora1a1AudioProcessorEditor (Calculadora1a1AudioProcessor&);
    ~Calculadora1a1AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    bool keyPressed (const juce::KeyPress&) override;

private:
    class CalculatorLookAndFeel final : public juce::LookAndFeel_V4
    {
    public:
        void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&,
                                   bool, bool) override;
        void drawButtonText (juce::Graphics&, juce::TextButton&, bool, bool) override;
    };

    enum class KeyAction { digit, decimal, clear, sign, percent, operation, equals };
    struct Key { const char* label; KeyAction action; int value; };
    static constexpr int keyCount = 19;
    static const Key keys[keyCount];

    void buttonClicked (juce::Button*) override;
    void handleKey (int);
    void enterDigit (int);
    void enterDecimal();
    void chooseOperation (char);
    void calculateEquals();
    void updateDisplay();
    juce::String formattedNumber (double) const;

    CalculatorLookAndFeel calculatorLookAndFeel;
    juce::Label display;
    juce::TextButton buttons[keyCount];
    juce::String entry { "0" };
    double accumulator = 0.0;
    double repeatedOperand = 0.0;
    char pendingOperation = 0;
    char repeatedOperation = 0;
    bool startNewEntry = true;
    bool entryOverridesAccumulator = false;
    bool hasRepeat = false;
    bool error = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Calculadora1a1AudioProcessorEditor)
};
