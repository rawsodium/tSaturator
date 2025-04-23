/*
  ==============================================================================

    DriveProcessor.h
    Created: 21 Mar 2025 12:39:45am
    Author:  Deanna Turner

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Params.h"

/*
 Handles applying gain to the incoming signal before saturation is applied.
 */
class DriveProcessor : public juce::dsp::ProcessorBase, public juce::AudioProcessorValueTreeState::Listener
{
public:
    DriveProcessor(juce::AudioProcessorValueTreeState& apvts);

    inline void mapDriveScaleToDecibels(float drive)
    {
        float gainInDecibels = juce::jmap(drive, 0.0f, 10.0f, 0.0f, 12.0f); // 0dB to +12dB range
        curGain = juce::Decibels::decibelsToGain(gainInDecibels);
    }
    void prepare (const juce::dsp::ProcessSpec& _spec) override;
    void process (const juce::dsp::ProcessContextReplacing<float>& context) override;
    void reset () override;
    void parameterChanged (const juce::String& parameterID, float newValue) override;
private:
    juce::AudioProcessorValueTreeState* apvts;
    juce::dsp::ProcessSpec spec;
    float drive;
    float curGain;
    float prevGain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DriveProcessor)
};
