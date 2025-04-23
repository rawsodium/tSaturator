/*
  ==============================================================================

    MixProcessor.cpp
    Created: 20 Mar 2025 1:32:57am
    Author:  Deanna Turner

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MixProcessor.h"
#include "Params.h"

MixProcessor::MixProcessor (juce::AudioProcessorValueTreeState& apvts)
{
    apvts.addParameterListener(Params::MIX, this);
}

void MixProcessor::prepare (const juce::dsp::ProcessSpec &_spec)
{
    spec = _spec;
    mixer.prepare(spec);
    mixer.setMixingRule(juce::dsp::DryWetMixingRule::balanced);
}

void MixProcessor::setDryBlock(juce::dsp::AudioBlock<const float> db)
{
    // Resize the storage buffer to match the input
    dryBuffer.setSize((int)db.getNumChannels(), (int)db.getNumSamples(), false, false, true);
    
    // Copy dry samples into the storage buffer
    for (size_t ch = 0; ch < db.getNumChannels(); ++ch)
    {
        dryBuffer.copyFrom((int)ch, 0, db.getChannelPointer(ch), (int)db.getNumSamples());
    }

    // Point originalDryBlock to the persistent buffer
    originalDryBlock = juce::dsp::AudioBlock<float>(dryBuffer);
}

void MixProcessor::process (const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto wetBlock = context.getOutputBlock();
    mixer.pushDrySamples(originalDryBlock);
    mixer.setWetMixProportion(dryWetMix);
    mixer.mixWetSamples(wetBlock);
}

void MixProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == Params::MIX)
    {
        dryWetMix = newValue;
        percentageToNumber();
    }
}

void MixProcessor::reset()
{
    mixer.reset();
}
