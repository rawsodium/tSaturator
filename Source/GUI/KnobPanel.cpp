//
//  KnobPanel.cpp
//  TapeSaturation
//
//  Created by Deanna Turner on 3/14/25.
//

#include "KnobPanel.hpp"
#include "Utilities.hpp"
#include "LookAndFeel.hpp"
#include "../DSP/Params.h"

KnobPanel::KnobPanel(juce::AudioProcessorValueTreeState& apvts) : apvts(apvts),
saturationKnob(*apvts.getParameter("Saturation"), "", "Saturation"),
driveKnob(*apvts.getParameter("Drive"), "", "Drive"),
mixKnob(*apvts.getParameter("Mix"), "%", "Mix"),
hissKnob(*apvts.getParameter("Hiss"), "", "Hiss"),
saturationKnobAttachment(apvts, "Saturation", saturationKnob),
driveKnobAttachment(apvts, "Drive", driveKnob),
mixKnobAttachment(apvts, "Mix", mixKnob),
hissKnobAttachment(apvts, "Hiss", hissKnob)
{
    addAndMakeVisible(saturationKnob);
    addAndMakeVisible(driveKnob);
    addAndMakeVisible(mixKnob);
    addAndMakeVisible(hissKnob);
    
    apvts.addParameterListener(Params::SATURATION, this);
    apvts.addParameterListener(Params::DRIVE, this);
    apvts.addParameterListener(Params::MIX, this);
    apvts.addParameterListener(Params::HISS, this);
}

void KnobPanel::resized()
{
    auto bounds = getLocalBounds();
    using namespace juce;
    
    auto makeOuterKnobColumn = [](std::vector<RotaryKnob*> comps)
    {
        FlexBox flexBox;
        flexBox.flexDirection = FlexBox::Direction::column;
        flexBox.flexWrap = FlexBox::Wrap::noWrap;
        
        auto verticalSpacer = FlexItem().withHeight(85);
        
        for (auto* comp : comps)
        {
            auto flexComp = FlexItem(*comp).withFlex(1.f);
            flexComp.maxWidth = 250;
            flexComp.maxHeight = 300;
            flexBox.items.add(verticalSpacer);
            flexBox.items.add(flexComp);
            flexBox.items.add(verticalSpacer);
        }
        
        return flexBox;
    };
    
    auto makeInnerKnobColumn = [](std::vector<RotaryKnob*> comps)
    {
        FlexBox flexBox;
        flexBox.flexDirection = FlexBox::Direction::column;
        flexBox.flexWrap = FlexBox::Wrap::noWrap;
        
        auto verticalSpacer = FlexItem().withHeight(20);
        
        for (auto* comp : comps)
        {
            auto flexComp = FlexItem(*comp).withFlex(1.f);
            flexComp.maxWidth = 200;
            flexComp.maxHeight = 200;
            
            if (comp->getKnobTitle() == "Drive")
                flexBox.items.add(verticalSpacer);
            flexBox.items.add(flexComp);
            flexBox.items.add(verticalSpacer);
        }
        
        return flexBox;
    };
    
    auto colorArea = makeOuterKnobColumn({&saturationKnob});
    auto centerArea = makeInnerKnobColumn({&driveKnob, &mixKnob});
    auto noiseArea = makeOuterKnobColumn({&hissKnob});
    
    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    
    auto horizontalSpacerOne = FlexItem().withWidth(200);
    auto horizontalSpacerTwo = FlexItem().withWidth(275);

    flexBox.items.add(horizontalSpacerOne);
    flexBox.items.add(FlexItem(colorArea).withWidth(bounds.getWidth() * 0.33));
    flexBox.items.add(horizontalSpacerTwo);
    flexBox.items.add(FlexItem(centerArea).withWidth(bounds.getWidth() * 0.33));
    flexBox.items.add(horizontalSpacerTwo);
    flexBox.items.add(FlexItem(noiseArea).withWidth(bounds.getWidth() * 0.33));
    flexBox.items.add(horizontalSpacerOne);
    
    flexBox.performLayout(bounds);
}

void KnobPanel::parameterChanged(const juce::String &parameterID, float newValue)
{
    if (parameterID == Params::SATURATION)
    {
        auto knobBounds = saturationKnob.getBoundsInParent();
        auto centerX = knobBounds.getCentreX();
        auto bottomY = knobBounds.getBottom();
        
        auto knobRect = juce::Rectangle<int>(centerX - 50, bottomY - 6, 100, 18);
        repaint(knobRect);
        
    } else if (parameterID == Params::DRIVE)
    {
        auto knobBounds = driveKnob.getBoundsInParent();
        auto centerX = knobBounds.getCentreX();
        auto bottomY = knobBounds.getBottom();
        
        auto knobRect = juce::Rectangle<int>(centerX - 50, bottomY - 6, 100, 18);
        repaint(knobRect);
        
    } else if (parameterID == Params::MIX)
    {
        auto knobBounds = mixKnob.getBoundsInParent();
        auto centerX = knobBounds.getCentreX();
        auto bottomY = knobBounds.getBottom();
        
        auto knobRect = juce::Rectangle<int>(centerX - 50, bottomY - 6, 100, 18);
        repaint(knobRect);
        
    } else if (parameterID == Params::HISS)
    {
        auto knobBounds = hissKnob.getBoundsInParent();
        auto centerX = knobBounds.getCentreX();
        auto bottomY = knobBounds.getBottom();
        
        auto knobRect = juce::Rectangle<int>(centerX - 50, bottomY - 6, 100, 18);
        repaint(knobRect);
    }
}

void KnobPanel::paint(juce::Graphics &g)
{
    using namespace juce;
    auto bounds = getLocalBounds();

    auto colorArea = bounds.removeFromLeft(bounds.getWidth() * 0.33f);
    auto noiseArea = bounds.removeFromRight(bounds.getWidth() * 0.5f); // 0.5f because 0.33 already removed
    
    // Draw boxes
    g.setColour(TSPalette::getKnobAreaBorderColor());
    g.drawRoundedRectangle(colorArea.toFloat().reduced(20), 10.f, 4.f);
    g.drawRoundedRectangle(noiseArea.toFloat().reduced(20), 10.f, 4.f);

    // Optionally draw labels
    g.setColour(TSPalette::getLabelTicksAndTextColor());
    g.setFont(16.0f);
    g.drawFittedText("COLOR", colorArea, Justification::centredTop, 1);
    g.drawFittedText("NOISE", noiseArea, Justification::centredTop, 1);
    
    // --- Custom Knob Label/Value Drawing ---
    g.setFont(14.0f);
    
    auto drawKnobLabel = [&](RotaryKnob& knob)
    {
        auto knobBounds = knob.getBoundsInParent(); // Relative to KnobPanel
        auto centerX = knobBounds.getCentreX();
        auto topY = knobBounds.getY();
        auto bottomY = knobBounds.getBottom();

        auto label = knob.getKnobTitle();
        auto value = knob.getDisplayString();

        const int textWidth = 100;
        const int textHeight = 18;

        // Title above the knob
        g.drawFittedText(label,
                         centerX - (textWidth / 2),
                         topY - 14, // A little above the knob
                         textWidth,
                         textHeight,
                         Justification::centred,
                         1);
        // Value below the knob
        g.drawFittedText(value,
                         centerX - (textWidth / 2),
                         bottomY - 6, // A little below the knob
                         textWidth,
                         textHeight,
                         Justification::centred,
                         1);
    };
    
    drawKnobLabel(driveKnob);
    drawKnobLabel(hissKnob);
    drawKnobLabel(saturationKnob);
    drawKnobLabel(mixKnob);
}
