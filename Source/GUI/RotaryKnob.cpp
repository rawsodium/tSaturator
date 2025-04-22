//
//  RotaryKnob.cpp
//  TapeSaturation - VST3
//
//  Created by Deanna Turner on 3/11/25.
//

#include "RotaryKnob.hpp"
#include "Utilities.hpp"

void RotaryKnob::paint(juce::Graphics &g)
{
    using namespace juce;
    
    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;
    
    auto range = getRange();
    
    auto sliderBounds = getSliderBounds();

    FlexItem knobFlexBox;
    knobFlexBox.maxWidth = 250;
    knobFlexBox.maxHeight = 270;
    
    getLookAndFeel().drawRotarySlider(g,
                                      sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      static_cast<float>(jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0)),
                                      startAng,
                                      endAng,
                                      *this);
    g.setFont(getTextHeight());
}

juce::Rectangle<int> RotaryKnob::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    
    size -= getTextHeight() * 2;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(20);
    
    return r;
}

juce::String RotaryKnob::getDisplayString() const
{
    juce::String str;
    
    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
    {
        float val = getValue();
        str = juce::String(val);
    }
    else
    {
        jassertfalse;
    }
    
    if (suffix.isNotEmpty())
    {
        str << " ";
        str << suffix;
    }
    
    return str;
}
