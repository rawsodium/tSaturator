/*
  ==============================================================================

    MixProcessor.h
    Created: 20 Mar 2025 1:32:57am
    Author:  Deanna Turner

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Params.h"
#include "SaturationProcessor.hpp"

/*
 Handles the balance of the wet and dry signal outputted by the application.
 */
class MixProcessor : public juce::dsp::ProcessorBase, public juce::AudioProcessorValueTreeState::Listener
{
public:
    MixProcessor(juce::AudioProcessorValueTreeState& apvts);
    void prepare (const juce::dsp::ProcessSpec& _spec) override;
    void process (const juce::dsp::ProcessContextReplacing<float>& context) override;
    void reset () override;
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    inline void percentageToNumber() { dryWetMix /= 100.f; };
    void setDryBlock(juce::dsp::AudioBlock<const float> db);
private:
    juce::AudioProcessorValueTreeState* apvts;
    juce::dsp::ProcessSpec spec;
    
    juce::dsp::DryWetMixer<float> mixer;
    juce::AudioBuffer<float> dryBuffer;
    juce::dsp::AudioBlock<const float> originalDryBlock;
    float dryWetMix {0.f};
    float prevDryWet {0.f};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixProcessor)
};
