#include "PluginEditor.h"
#include <iomanip>
#include <sstream>

const Calculadora1a1AudioProcessorEditor::Key Calculadora1a1AudioProcessorEditor::keys[keyCount] = {
    { "AC", KeyAction::clear, 0 }, { "+/-", KeyAction::sign, 0 }, { "%", KeyAction::percent, 0 }, { "\xC3\xB7", KeyAction::operation, '/' },
    { "7", KeyAction::digit, 7 }, { "8", KeyAction::digit, 8 }, { "9", KeyAction::digit, 9 }, { "\xC3\x97", KeyAction::operation, '*' },
    { "4", KeyAction::digit, 4 }, { "5", KeyAction::digit, 5 }, { "6", KeyAction::digit, 6 }, { "\xE2\x88\x92", KeyAction::operation, '-' },
    { "1", KeyAction::digit, 1 }, { "2", KeyAction::digit, 2 }, { "3", KeyAction::digit, 3 }, { "+", KeyAction::operation, '+' },
    { "0", KeyAction::digit, 0 }, { ",", KeyAction::decimal, 0 }, { "=", KeyAction::equals, 0 }
};

void Calculadora1a1AudioProcessorEditor::CalculatorLookAndFeel::drawButtonBackground (
    juce::Graphics& g, juce::Button& button, const juce::Colour& background,
    bool isMouseOver, bool isButtonDown)
{
    auto colour = background;
    if (isButtonDown) colour = colour.darker (0.08f);
    else if (isMouseOver) colour = colour.brighter (0.04f);
    g.fillAll (colour);
    g.setColour (juce::Colour (0xff9b9b9b));
    g.drawRect (button.getLocalBounds().toFloat(), 0.7f);
}

void Calculadora1a1AudioProcessorEditor::CalculatorLookAndFeel::drawButtonText (
    juce::Graphics& g, juce::TextButton& button, bool, bool)
{
    g.setColour (button.findColour (juce::TextButton::textColourOffId));
    const float fontSize = button.getButtonText().length() > 2 ? 19.0f : 23.0f;
    g.setFont (juce::Font (juce::FontOptions (fontSize)));
    auto area = button.getLocalBounds().reduced (2, 0);
    if (button.getButtonText() == "0")
        g.drawText (button.getButtonText(), area.withTrimmedLeft (23), juce::Justification::centredLeft, true);
    else
        g.drawText (button.getButtonText(), area, juce::Justification::centred, true);
}

Calculadora1a1AudioProcessorEditor::Calculadora1a1AudioProcessorEditor (Calculadora1a1AudioProcessor& processor)
    : AudioProcessorEditor (processor)
{
    setSize (250, 321);
    setResizable (false, false);
    setWantsKeyboardFocus (true);

    display.setJustificationType (juce::Justification::centredRight);
    display.setColour (juce::Label::textColourId, juce::Colours::white);
    display.setFont (juce::Font (juce::FontOptions (47.0f, juce::Font::plain)));
    display.setText ("0", juce::dontSendNotification);
    addAndMakeVisible (display);

    for (int i = 0; i < keyCount; ++i)
    {
        auto& button = buttons[i];
        // Key labels are UTF-8 (including the three arithmetic glyphs). Tell JUCE
        // explicitly so its 7-bit ASCII constructor doesn't reinterpret the bytes.
        button.setButtonText (juce::String (juce::CharPointer_UTF8 (keys[i].label)));
        button.setColour (juce::TextButton::buttonColourId,
                          i % 4 == 3 ? juce::Colour (0xffe59438) : juce::Colour (0xffd6d6d6));
        button.setColour (juce::TextButton::buttonOnColourId,
                          i % 4 == 3 ? juce::Colour (0xffd9872d) : juce::Colour (0xffc9c9c9));
        button.setColour (juce::TextButton::textColourOffId, juce::Colour (0xff202020));
        button.setColour (juce::TextButton::textColourOnId, juce::Colour (0xff202020));
        button.setLookAndFeel (&calculatorLookAndFeel);
        button.setConnectedEdges (juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight
                                  | juce::Button::ConnectedOnTop | juce::Button::ConnectedOnBottom);
        button.setMouseCursor (juce::MouseCursor::PointingHandCursor);
        button.addListener (this);
        addAndMakeVisible (button);
    }
}

Calculadora1a1AudioProcessorEditor::~Calculadora1a1AudioProcessorEditor()
{
    for (auto& button : buttons)
    {
        button.removeListener (this);
        button.setLookAndFeel (nullptr);
    }
}

void Calculadora1a1AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff202b37));
    g.setColour (juce::Colour (0xff9b9b9b));
    g.drawHorizontalLine (61, 0.0f, 250.0f);
}

void Calculadora1a1AudioProcessorEditor::resized()
{
    display.setBounds (14, 2, 222, 57);
    constexpr int top = 61;
    constexpr int rowHeight = 52;
    const int colWidth[4] = { 63, 62, 63, 62 };
    int xPositions[4] = { 0, colWidth[0], colWidth[0] + colWidth[1], colWidth[0] + colWidth[1] + colWidth[2] };

    for (int i = 0; i < 16; ++i)
    {
        const int row = i / 4;
        const int col = i % 4;
        buttons[i].setBounds (xPositions[col], top + row * rowHeight, colWidth[col], rowHeight);
    }
    buttons[16].setBounds (0, top + 4 * rowHeight, colWidth[0] + colWidth[1], rowHeight);
    buttons[17].setBounds (colWidth[0] + colWidth[1], top + 4 * rowHeight, colWidth[2], rowHeight);
    buttons[18].setBounds (colWidth[0] + colWidth[1] + colWidth[2], top + 4 * rowHeight, colWidth[3], rowHeight);
}

void Calculadora1a1AudioProcessorEditor::buttonClicked (juce::Button* button)
{
    for (int i = 0; i < keyCount; ++i)
        if (&buttons[i] == button)
        {
            handleKey (i);
            return;
        }
}

bool Calculadora1a1AudioProcessorEditor::keyPressed (const juce::KeyPress& key)
{
    const auto c = key.getTextCharacter();
    if (c >= '0' && c <= '9') { enterDigit ((int) (c - '0')); return true; }
    if (c == '.' || c == ',') { enterDecimal(); return true; }
    if (c == '+' || c == '-' || c == '*' || c == '/') { chooseOperation ((char) c); return true; }
    if (c == '%' ) { handleKey (2); return true; }
    if (c == '=' || key.getKeyCode() == juce::KeyPress::returnKey) { calculateEquals(); return true; }
    if (key.getKeyCode() == juce::KeyPress::escapeKey || key.getKeyCode() == juce::KeyPress::deleteKey)
    { handleKey (0); return true; }
    if (key.getKeyCode() == juce::KeyPress::backspaceKey)
    {
        if (! startNewEntry && ! error)
        {
            entry = entry.dropLastCharacters (1);
            if (entry.isEmpty() || entry == "-") entry = "0";
            updateDisplay();
        }
        return true;
    }
    return false;
}

void Calculadora1a1AudioProcessorEditor::handleKey (int index)
{
    const auto& key = keys[index];
    switch (key.action)
    {
        case KeyAction::digit: enterDigit (key.value); break;
        case KeyAction::decimal: enterDecimal(); break;
        case KeyAction::clear:
            entry = "0"; accumulator = repeatedOperand = 0.0;
            pendingOperation = repeatedOperation = 0; startNewEntry = true;
            entryOverridesAccumulator = false; hasRepeat = error = false;
            updateDisplay(); break;
        case KeyAction::sign:
            if (! error && entry != "0")
            {
                entry = entry.startsWithChar ('-') ? entry.substring (1) : "-" + entry;
                entryOverridesAccumulator = true;
                hasRepeat = false;
            }
            updateDisplay(); break;
        case KeyAction::percent:
            if (! error)
            {
                double value = entry.getDoubleValue();
                if (pendingOperation == '+' || pendingOperation == '-') value = accumulator * value / 100.0;
                else value /= 100.0;
                entry = formattedNumber (value); startNewEntry = true;
                entryOverridesAccumulator = true; hasRepeat = false; updateDisplay();
            }
            break;
        case KeyAction::operation: chooseOperation ((char) key.value); break;
        case KeyAction::equals: calculateEquals(); break;
    }
}

void Calculadora1a1AudioProcessorEditor::enterDigit (int digit)
{
    if (error) { entry = "0"; error = false; }
    if (startNewEntry) { entry = juce::String (digit); startNewEntry = false; }
    else if (entry.replace (",", ".").length() < 16)
        entry += juce::String (digit);
    entryOverridesAccumulator = false;
    hasRepeat = false;
    updateDisplay();
}

void Calculadora1a1AudioProcessorEditor::enterDecimal()
{
    if (error) { entry = "0"; error = false; }
    if (startNewEntry) { entry = "0."; startNewEntry = false; }
    else if (! entry.contains (".")) entry += ".";
    entryOverridesAccumulator = false;
    hasRepeat = false;
    updateDisplay();
}

void Calculadora1a1AudioProcessorEditor::chooseOperation (char operation)
{
    if (error) { entry = "0"; error = false; }
    if (pendingOperation != 0 && (! startNewEntry || entryOverridesAccumulator))
    {
        const double right = entry.getDoubleValue();
        if (pendingOperation == '/' && right == 0.0)
        {
            entry = "Error"; error = true; pendingOperation = 0;
            startNewEntry = true; entryOverridesAccumulator = false; hasRepeat = false;
            updateDisplay(); return;
        }
        if (pendingOperation == '+') accumulator += right;
        else if (pendingOperation == '-') accumulator -= right;
        else if (pendingOperation == '*') accumulator *= right;
        else if (pendingOperation == '/') accumulator /= right;
        entry = formattedNumber (accumulator);
    }
    else accumulator = entry.getDoubleValue();
    pendingOperation = operation;
    startNewEntry = true;
    entryOverridesAccumulator = false;
    hasRepeat = false;
    updateDisplay();
}

void Calculadora1a1AudioProcessorEditor::calculateEquals()
{
    if (error) return;
    if (pendingOperation != 0)
    {
        const double right = startNewEntry && ! entryOverridesAccumulator ? accumulator : entry.getDoubleValue();
        const auto operation = pendingOperation;
        if (operation == '/' && right == 0.0)
        {
            entry = "Error"; error = true; pendingOperation = 0;
            startNewEntry = true; entryOverridesAccumulator = false; hasRepeat = false;
            updateDisplay(); return;
        }
        if (operation == '+') accumulator += right;
        else if (operation == '-') accumulator -= right;
        else if (operation == '*') accumulator *= right;
        else if (operation == '/') accumulator /= right;
        entry = formattedNumber (accumulator);
        repeatedOperation = operation;
        repeatedOperand = right;
        hasRepeat = true;
        pendingOperation = 0;
        startNewEntry = true;
        entryOverridesAccumulator = false;
    }
    else if (hasRepeat)
    {
        const double left = entry.getDoubleValue();
        if (repeatedOperation == '+') entry = formattedNumber (left + repeatedOperand);
        else if (repeatedOperation == '-') entry = formattedNumber (left - repeatedOperand);
        else if (repeatedOperation == '*') entry = formattedNumber (left * repeatedOperand);
        else if (repeatedOperation == '/' && repeatedOperand != 0.0) entry = formattedNumber (left / repeatedOperand);
    }
    updateDisplay();
}

juce::String Calculadora1a1AudioProcessorEditor::formattedNumber (double value) const
{
    if (value == 0.0) value = 0.0;
    std::ostringstream stream;
    stream << std::setprecision (12) << std::defaultfloat << value;
    return juce::String (stream.str());
}

void Calculadora1a1AudioProcessorEditor::updateDisplay()
{
    auto visible = entry;
    visible = visible.replace (".", ",");
    display.setText (visible, juce::dontSendNotification);
    const float size = visible.length() > 11 ? 28.0f : (visible.length() > 8 ? 35.0f : 47.0f);
    display.setFont (juce::Font (juce::FontOptions (size, juce::Font::plain)));
}
