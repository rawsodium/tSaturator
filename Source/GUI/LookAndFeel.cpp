//
//  LookAndFeel.cpp
//  TapeSaturation - VST3
//
//  Created by Deanna Turner on 3/10/25.
//

#include "LookAndFeel.hpp"
#include "RotaryKnob.hpp"

void LookAndFeel::drawRotarySlider(juce::Graphics & g,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider & slider)
{
    using namespace juce;
    
    auto bounds = Rectangle<float>(x, y, width, height);
    auto tickBoundingBox = Rectangle<float>(x, y, width, height).expanded(10);
    
    g.setColour(TSPalette::getInnerKnobColor());
    g.fillEllipse(bounds);
    
    g.setColour(TSPalette::getOuterKnobColor());
    g.drawEllipse(bounds.reduced(4), 10);
    g.setColour(TSPalette::getLabelTicksAndTextColor());
    g.drawEllipse(bounds, 1.f);
    
    Path tickMarks;
    tickMarks.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), tickBoundingBox.getWidth() / 2, tickBoundingBox.getWidth() / 2, (rotaryEndAngle + rotaryStartAngle) + degreesToRadians(45), rotaryStartAngle, rotaryEndAngle, true);
    g.strokePath(tickMarks, juce::PathStrokeType(2.0f));
    
    // Draw ticks
    auto center = bounds.getCentre();
    auto tickRadius = bounds.getWidth() * 0.5f + 9.5f;
    const int numMajorTicks = 11;
    int numMinorTicksPerGap = 1;
    
    // Drive knob should not have minor ticks
    if (auto* knob = dynamic_cast<RotaryKnob*>(&slider))
    {
        juce::String title = knob->getKnobTitle();
        if (title == "Drive")
            numMinorTicksPerGap = 0;
    }
    int totalTicks = (numMajorTicks - 1) * (numMinorTicksPerGap + 1) + 1;

    // JUCE starts drawing from 3 o'clock and not 12
    auto startAngTicks = degreesToRadians(225.f - 90.f);
    auto endAngTicks = startAngTicks + degreesToRadians(270.f);
    
    for (int i = 0; i < totalTicks; ++i)
    {
        float angle = jmap((float)i, 0.f, (float)(totalTicks - 1), startAngTicks, endAngTicks);
        
        bool isMajor = (i % (numMinorTicksPerGap + 1)) == 0;

        float tickLength = isMajor ? 8.0f : 4.0f;

        auto tickStart = center + Point<float>(std::cos(angle), std::sin(angle)) * tickRadius;
        auto tickEnd   = center + Point<float>(std::cos(angle), std::sin(angle)) * (tickRadius + tickLength);

        g.drawLine({ tickStart, tickEnd }, 2.0f);
    }
    
    if (auto* rk = dynamic_cast<RotaryKnob*>(&slider))
    {
        g.setColour(TSPalette::getLabelTicksAndTextColor());
        auto center = bounds.getCentre();
        Path p;
        
        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY() - rk->getTextHeight() * 1.5);
        
        p.addRoundedRectangle(r, 2.f);
        
        jassert(rotaryStartAngle < rotaryEndAngle);
        
        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
        
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
        
        g.fillPath(p);
    }
}

void LookAndFeel::drawToggleButton(juce::Graphics &g,
                                   juce::ToggleButton &toggleButton,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown)
{
    using namespace juce;
    if (auto* analyzerButton = dynamic_cast<ToggleButton*>(&toggleButton))
    {
        g.setColour(Colours::white);
        auto bounds = toggleButton.getLocalBounds();
        g.drawRect(bounds);
        g.setFont(14);
        juce::String str = toggleButton.getToggleState() ? "Resume Analyzer" : "Pause Analyzer";
        g.drawText(str, bounds, juce::Justification::centred);
        
        if (toggleButton.getToggleState())
        {
            g.setOpacity(0.5f);
            g.fillRect(bounds);
        }
    }
}
