//
//  LookAndFeel.hpp
//  TapeSaturation - VST3
//
//  Created by Deanna Turner on 3/10/25.
//

#include <JuceHeader.h>

namespace TSPalette
{
inline juce::Colour getLeftChannelColor() { return juce::Colours::skyblue; }
inline juce::Colour getRightChannelColor() { return juce::Colours::cadetblue; }
inline juce::Colour getGridlineColor() { return juce::Colours::lightgrey; }
inline juce::Colour getLabelTicksAndTextColor() { return juce::Colours::black; }
inline juce::Colour getOuterKnobColor() { return juce::Colours::darkgrey; }
inline juce::Colour getInnerKnobColor() { return juce::Colours::lightgrey; }
inline juce::Colour getKnobAreaBorderColor() { return juce::Colours::darkgrey; }
inline juce::Colour getPluginBackgroundColor() { return juce::Colours::lightsteelblue; }
inline juce::Colour getAnalysisAreaBackgroundColor() { return juce::Colours::black; }
inline juce::Colour getToggleButtonTextColor() { return juce::Colours::white; }
}
struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics&,
                           int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider&) override;
    
    void drawToggleButton (juce::Graphics &g,
                           juce::ToggleButton & toggleButton,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;
};
