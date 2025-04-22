//
//  RotaryKnob.hpp
//  TapeSaturation - VST3
//
//  Created by Deanna Turner on 3/11/25.
//

#pragma once

#include <JuceHeader.h>

/*
 Handles logic for each rotary knob on the interface
 */
struct RotaryKnob : juce::Slider
{
public:
    RotaryKnob(juce::RangedAudioParameter& rap,
               const juce::String& unitSuffix,
               const juce::String& title) :
    juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                 juce::Slider::TextEntryBoxPosition::NoTextBox),
    param(&rap),
    suffix(unitSuffix),
    title(title)
    {
        setName(title);
    }
    
    ~RotaryKnob()
    {
        setName("");
    }
    
    struct LabelPos
    {
        float pos;
        juce::String label;
    };
    
    juce::Array<LabelPos> labels;
    
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;
    juce::String getKnobTitle() const { return this->title; }
protected:
    juce::RangedAudioParameter* param;
    juce::String suffix;
    juce::String title;
};
