/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "./GUI/RotaryKnob.hpp"
#include "./GUI/LookAndFeel.hpp"
#include "./GUI/SpectrumAnalyzer.hpp"
#include "./GUI/KnobPanel.hpp"

/*
 Holds title and analyzer toggle button
 */
struct HeaderBar : juce::Component
{
    HeaderBar();
    void resized() override;
    void paint(juce::Graphics& g) override;
    
    juce::ToggleButton analyzerToggleButton;
    juce::Image title;
};

//==============================================================================
/**
*/
class TapeSaturationAudioProcessorEditor : public juce::AudioProcessorEditor, juce::Timer
{
public:
    TapeSaturationAudioProcessorEditor (TapeSaturationAudioProcessor&);
    ~TapeSaturationAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override;
private:
    LookAndFeel lnf;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TapeSaturationAudioProcessor& audioProcessor;
    
    HeaderBar headerBar;
    KnobPanel knobPanel { audioProcessor.apvts };
    SpectrumAnalyzer analyzer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapeSaturationAudioProcessorEditor)
};
