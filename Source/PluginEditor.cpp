/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "./GUI/RotaryKnob.hpp"
#include "./GUI/KnobPanel.hpp"

HeaderBar::HeaderBar()
{
    addAndMakeVisible(analyzerToggleButton);
}

void HeaderBar::resized()
{
    auto bounds = getLocalBounds();
    
    analyzerToggleButton.setBounds(bounds.removeFromLeft(120)
                             .withTrimmedTop(20)
                             .withTrimmedLeft(20)
                             .withTrimmedBottom(20));
}

void HeaderBar::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    juce::Rectangle<float> titleBox;
    titleBox.setHeight(bounds.getHeight());
    titleBox.setWidth(bounds.getWidth() / 3);
    titleBox.setX(bounds.getCentreX() + 5);
    titleBox.setRight(bounds.getRight() - 50);
    g.setColour(TSPalette::getAnalysisAreaBackgroundColor());
    g.fillAll();
    title = juce::ImageCache::getFromMemory(BinaryData::tSaturator_logo_jpg, BinaryData::tSaturator_logo_jpgSize);
    g.drawImage(title, titleBox);
}

//==============================================================================
TapeSaturationAudioProcessorEditor::TapeSaturationAudioProcessorEditor (TapeSaturationAudioProcessor& p)
: AudioProcessorEditor(&p), audioProcessor (p), analyzer (p.analyzer)
{
    setLookAndFeel(&lnf);
    
    addAndMakeVisible(headerBar);
    addAndMakeVisible(analyzer);
    addAndMakeVisible(knobPanel);
    
    auto safePtr = juce::Component::SafePointer<TapeSaturationAudioProcessorEditor>(this);
    
    headerBar.analyzerToggleButton.onClick = [safePtr]()
    {
        if (auto* comp = safePtr.getComponent())
        {
            auto enabled = comp->headerBar.analyzerToggleButton.getToggleState();
            comp->analyzer.toggleAnalysisEnablement(!enabled);
        }
    };
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(850, 600);
    startTimerHz(60);
}

TapeSaturationAudioProcessorEditor::~TapeSaturationAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void TapeSaturationAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(TSPalette::getPluginBackgroundColor());
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}

void TapeSaturationAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    
    headerBar.setBounds(bounds.removeFromTop(75));
    analyzer.setBounds(bounds.removeFromTop(225));
    knobPanel.setBounds(bounds.removeFromBottom(300));
}

void TapeSaturationAudioProcessorEditor::timerCallback()
{
    analyzer.timerCallback();
}
