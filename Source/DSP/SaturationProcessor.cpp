//
//  SaturationProcessor.cpp
//  TapeSaturation
//
//  Created by Deanna Turner on 3/16/25.
//

#include "SaturationProcessor.hpp"
#include "Params.h"

SaturationProcessor::SaturationProcessor(juce::AudioProcessorValueTreeState& apvts)
{
    apvts.addParameterListener(Params::SATURATION, this);
}

void SaturationProcessor::setCompressorSettings()
{
    processorChain.get<SaturationChainPositions::COMPRESSION>().setRatio(1.f);
    processorChain.get<SaturationChainPositions::COMPRESSION>().setAttack(30.f);
    processorChain.get<SaturationChainPositions::COMPRESSION>().setRelease(50.f);
    processorChain.get<SaturationChainPositions::COMPRESSION>().setThreshold(-30.f);
}

void SaturationProcessor::prepare(const juce::dsp::ProcessSpec &_spec)
{
    spec = _spec;
    processorChain.get<SaturationChainPositions::COMPRESSION>().prepare(spec);
    setCompressorSettings();
    oversampler.reset();
    constexpr size_t oversampleFactor = 2;
    constexpr auto filterType = juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR;
    oversampler = std::make_unique<juce::dsp::Oversampling<float>>(spec.numChannels, oversampleFactor, filterType);
    oversampler->initProcessing(spec.maximumBlockSize);
}


void SaturationProcessor::process (const juce::dsp::ProcessContextReplacing<float>& context)
{
    // upsample
//    DBG("Saturation - Before processing: " << context.getInputBlock().getChannelPointer(0)[0]);
    auto upsampledBlock = oversampler->processSamplesUp(context.getInputBlock());
    float satLevel = 1.0f + getSaturationMultiplier();
    // Scale input before passing through waveshaper
//    float satLevel = 1.0f + (getSaturationMultiplier() * 4.0f);

    if (saturation == 0.01f)
        satLevel = 1.0f;
    // apply saturation level from knob
    for (size_t channel = 0; channel < upsampledBlock.getNumChannels(); ++channel)
    {
        auto* channelData = upsampledBlock.getChannelPointer(channel);
        for (size_t sample = 0; sample < upsampledBlock.getNumSamples(); ++sample)
            channelData[sample] *= satLevel;
    }
    juce::dsp::ProcessContextReplacing<float> upsampledContext(upsampledBlock);
    setDistortionTransferFunction(upsampledContext);
    processorChain.process(upsampledContext);
    
    // normalize output so it isn't too loud
    // unsure if this is causing sat to not be heard anymore... investigate
//    float postGain = 1.0f / (1.0f + getSaturationMultiplier());
//    
//    for (size_t channel = 0; channel < upsampledBlock.getNumChannels(); ++channel)
//    {
//        auto* channelData = upsampledBlock.getChannelPointer(channel);
//        for (size_t sample = 0; sample < upsampledBlock.getNumSamples(); ++sample)
//            channelData[sample] *= postGain;
//    }

    // downsample
    oversampler->processSamplesDown(context.getOutputBlock());
//    DBG("Saturation - After processing: " << context.getOutputBlock().getChannelPointer(0)[0]);
}

void SaturationProcessor::reset ()
{
    oversampler->reset();
    processorChain.reset();
}

void SaturationProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == Params::SATURATION)
    {
        prevSaturation = saturation;
        saturation = newValue;
//        DBG("Current Saturation value: " << juce::String(newValue));
    }
}

float SaturationProcessor::getLatencyInSamples()
{
    return oversampler == nullptr ? 0.0f : oversampler->getLatencyInSamples();
}
