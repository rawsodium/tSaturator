//
//  KnobPanel.hpp
//  TapeSaturation
//
//  Created by Deanna Turner on 3/14/25.
//

#pragma once

#include <JuceHeader.h>
#include "RotaryKnob.hpp"

/*
 Struct that holds all knobs on the interface. This class is only a listener so that it can redraw the text under the knobs
 */
struct KnobPanel : public juce::Component
{
public:
    KnobPanel(juce::AudioProcessorValueTreeState& apvts);
    
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    juce::AudioProcessorValueTreeState& apvts;
    RotaryKnob saturationKnob, driveKnob, mixKnob, hissKnob;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;
    
    Attachment saturationKnobAttachment, driveKnobAttachment, mixKnobAttachment, hissKnobAttachment;
};

