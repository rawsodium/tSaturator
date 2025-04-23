/*
  ==============================================================================

    HissProcessor.h
    Created: 17 Mar 2025 11:17:35pm
    Author:  Deanna Turner

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Params.h"

/*
 Enum for the response curves in the EQ used to treat the hiss noise.
 */
enum ResponseCurveBands
{
    LOW_CUT,
    LOW_SHELF,
    BAND_ONE,
    BAND_TWO,
    BAND_THREE,
    BAND_FOUR,
    HIGH_SHELF,
    HIGH_CUT
};

// modeling Ampex ATR-102 half-inch two-track, with extended low frequency heads, 30 IPS
struct HissResponseCurveSettings
{
    float lowCutFreq { 30.f }, lowCutSlope { 36.f };
    float lowShelfFreq { 75.5f }, lowShelfGainDecibels { -2.3f };
    double lowShelfQuality { 1.0 }, highShelfQuality { 1.3 }, lowCutQuality { 0.75 }, highCutQuality { 0 };
    float bandOneFreq { 50.f }, bandOneGainDecibels { -3.2f }, bandOneQuality { 3.6f };
    float bandTwoFreq { 85.f }, bandTwoGainDecibels { 2.f }, bandTwoQuality { 3.2f };
    float bandThreeFreq { 250.f }, bandThreeGainDecibels { -1.7f }, bandThreeQuality { 2.f };
    float bandFourFreq { 5000.f }, bandFourGainDecibels { -0.5f }, bandFourQuality { 1.8f };
    float highShelfFreq { 0 }, highShelfGainDecibels { 0 };
    float highCutFreq { 0 }, highCutSlope { 6.f };
};

using FilterPtr = juce::dsp::IIR::Coefficients<float>::Ptr;
using Filter = juce::dsp::IIR::Filter<float>;
using EQChain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter>;

FilterPtr peakFilter(float freq, float q, float gain, double sampleRate);
FilterPtr lowCutFilter(const HissResponseCurveSettings& settings, double sampleRate);
FilterPtr highCutFilter(const HissResponseCurveSettings& settings, double sampleRate);
FilterPtr lowShelfFilter(const HissResponseCurveSettings& settings, double sampleRate);
FilterPtr highShelfFilter(const HissResponseCurveSettings& settings, double sampleRate);

/*
 Handles how much hiss (filtered white noise) is outputted
 */
class HissProcessor : public juce::dsp::ProcessorBase, public juce::AudioProcessorValueTreeState::Listener
{
public:
    HissProcessor(juce::AudioProcessorValueTreeState& apvts);
    void setGain (float hissPercentage);
    void initializeFilters(HissResponseCurveSettings& settings);
    void prepare (const juce::dsp::ProcessSpec& _spec) override;
    void process (const juce::dsp::ProcessContextReplacing<float>& context) override;
    void reset () override;
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    void processBlock(float* buffer, int numSamples);
    inline void updateCoefficients(Filter& filter, FilterPtr newCoefficients)
    {
        filter.coefficients = newCoefficients;
    }
private:
    juce::AudioProcessorValueTreeState* apvts;
    juce::dsp::ProcessSpec spec;
    
    EQChain preProcessEQ;
    HissResponseCurveSettings settings;
    Filter LP, LS, B1, B2, B3, B4, B5, HS, HP;
    
    // for white noise generation
    float curGain = -60.0f;
    float prevGain = curGain;
    
    juce::Random random;
    
    float hissPercentage;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HissProcessor)
};
