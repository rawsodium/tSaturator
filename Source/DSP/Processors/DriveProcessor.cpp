/*
  ==============================================================================

    DriveProcessor.cpp
    Created: 21 Mar 2025 12:39:45am
    Author:  Deanna Turner

  ==============================================================================
*/

#include "DriveProcessor.h"
#include "Params.h"

DriveProcessor::DriveProcessor(juce::AudioProcessorValueTreeState& apvts)
{
    apvts.addParameterListener(Params::DRIVE, this);
}

void DriveProcessor::prepare (const juce::dsp::ProcessSpec& _spec)
{
    spec = _spec;
    prevGain = curGain;
    if (drive == 0)
    {
        curGain = 0;
    }
}

void DriveProcessor::process (const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& block = context.getOutputBlock();
    auto numSamples = static_cast<int>(block.getNumSamples());
    auto numChannels = static_cast<int>(block.getNumChannels());
    
    if (curGain != prevGain)
    {
        for (int ch = 0; ch < numChannels; ++ch)
            {
                float* channelData = block.getChannelPointer(ch);
                float gainStep = (curGain - prevGain) / numSamples;
                float gain = prevGain;

                for (int i = 0; i < numSamples; ++i)
                {
                    channelData[i] *= gain;
                    gain += gainStep;
                }
            }
        prevGain = curGain;
    }
    else
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            juce::FloatVectorOperations::multiply(block.getChannelPointer(ch), curGain, numSamples);
        }
    }
}

void DriveProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == Params::DRIVE)
    {
        drive = newValue;
        prevGain = curGain;
        mapDriveScaleToDecibels(drive);
    }
}

void DriveProcessor::reset()
{
    prevGain = curGain;
}
