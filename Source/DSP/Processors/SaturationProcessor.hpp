//
//  SaturationProcessor.hpp
//  TapeSaturation
//
//  Created by Deanna Turner on 3/16/25.
//

#pragma once

#include <JuceHeader.h>
#include "Params.h"

/*
 Enum for the processor chain that the saturation processor goes through.
 */
enum SaturationChainPositions
{
    DISTORTION,
    COMPRESSION
};

/*
 Handles applying saturation to the input signal based on the value of the knob
 */
class SaturationProcessor : public juce::dsp::ProcessorBase, public juce::AudioProcessorValueTreeState::Listener
{
public:
    SaturationProcessor(juce::AudioProcessorValueTreeState& apvts);

    inline float getSaturationMultiplier() { return std::pow(saturation / 100.f, 2.0f); };
    inline float getMaxBlockSize() const { return spec.maximumBlockSize; }
    void prepare (const juce::dsp::ProcessSpec& _spec) override;
    void process (const juce::dsp::ProcessContextReplacing<float>& context) override;
    void reset () override;
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    float getLatencyInSamples();
    void setCompressorSettings();
    
private:
    juce::AudioProcessorValueTreeState* apvts;
    
    float saturation;
    float prevSaturation;

    juce::dsp::ProcessSpec spec;
    
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
    
    using SaturationChain = juce::dsp::ProcessorChain<juce::dsp::WaveShaper<float>, juce::dsp::Compressor<float>>;
    SaturationChain processorChain;
    
    // sets the transfer function
    void setDistortionTransferFunction(const juce::dsp::ProcessContextReplacing<float> &context)
    {
        auto& waveshaper = processorChain.get<SaturationChainPositions::DISTORTION>();
        
        waveshaper.functionToUse = [] (float x) {
            return (std::tanh(x) * 5.f) / (std::tanh(5.f));
        };
    }
};
