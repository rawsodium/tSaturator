/*
  ==============================================================================

    HissProcessor.cpp
    Created: 17 Mar 2025 11:17:35pm
    Author:  Deanna Turner

  ==============================================================================
*/

#include "HissProcessor.h"
#include "Params.h"

/*
 Sets the gain value for the amount of hiss heard based on the knob value, scaled between -60dB and -12dB.
 */
void HissProcessor::setGain(float hissPercentage)
{
    float targetGain = juce::Decibels::decibelsToGain(juce::jmap(hissPercentage, 0.0f, 100.0f, -60.0f, -12.0f));
    curGain = targetGain;
}

/*
 Sets (and returns) the coefficients for a peak filter in the EQ
 */
FilterPtr peakFilter(float freq, float q, float gain, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, freq, q, juce::Decibels::decibelsToGain(gain));
}

/*
 Sets (and returns) the coefficients for a low cut filter in the EQ
 */
FilterPtr lowCutFilter(const HissResponseCurveSettings& settings, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, settings.lowCutFreq);
}

/*
 Sets (and returns) the coefficients for a high cut filter in the EQ
 */
FilterPtr highCutFilter(const HissResponseCurveSettings& settings, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, settings.highCutFreq);
}

/*
 Sets (and returns) the coefficients for a low shelf filter in the EQ
 */
FilterPtr lowShelfFilter(const HissResponseCurveSettings& settings, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, settings.lowShelfFreq, settings.lowShelfQuality, juce::Decibels::decibelsToGain(settings.lowShelfGainDecibels));
}

/*
 Sets (and returns) the coefficients for a high shelf filter in the EQ
 */
FilterPtr highShelfFilter(const HissResponseCurveSettings& settings, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, settings.highShelfFreq, settings.highShelfQuality, juce::Decibels::decibelsToGain(settings.highShelfGainDecibels));
}

/*
 Instantiates all filters in the EQ with the proper coefficients
 */
void HissProcessor::initializeFilters(HissResponseCurveSettings& hrcs)
{
    updateCoefficients(preProcessEQ.get<LOW_CUT>(), lowCutFilter(hrcs, spec.sampleRate));
    updateCoefficients(preProcessEQ.get<LOW_SHELF>(), lowShelfFilter(hrcs, spec.sampleRate));
    updateCoefficients(preProcessEQ.get<BAND_ONE>(), peakFilter(hrcs.bandOneFreq, hrcs.bandOneQuality, hrcs.bandOneGainDecibels, spec.sampleRate));
    updateCoefficients(preProcessEQ.get<BAND_TWO>(), peakFilter(hrcs.bandTwoFreq, hrcs.bandTwoQuality, hrcs.bandTwoGainDecibels, spec.sampleRate));
    updateCoefficients(preProcessEQ.get<BAND_THREE>(), peakFilter(hrcs.bandThreeFreq, hrcs.bandThreeQuality, hrcs.bandThreeGainDecibels, spec.sampleRate));
    updateCoefficients(preProcessEQ.get<BAND_FOUR>(), peakFilter(hrcs.bandFourFreq, hrcs.bandFourQuality, hrcs.bandFourGainDecibels, spec.sampleRate));
    updateCoefficients(preProcessEQ.get<HIGH_SHELF>(), highShelfFilter(hrcs, spec.sampleRate));
    updateCoefficients(preProcessEQ.get<HIGH_CUT>(), highCutFilter(hrcs, spec.sampleRate));
}

HissProcessor::HissProcessor(juce::AudioProcessorValueTreeState& apvts)
{
    apvts.addParameterListener(Params::HISS, this);
}

void HissProcessor::prepare (const juce::dsp::ProcessSpec& _spec)
{
    spec = _spec;
    prevGain = curGain = juce::Decibels::decibelsToGain(-60.f);
    preProcessEQ.prepare(spec);
}

/*
 Generates white noise
 */
void HissProcessor::processBlock(float *buffer, int numSamples)
{
    for (int n = 0; n < numSamples; ++n)
    {
        float noise = (random.nextFloat() - 0.5f) * curGain;
        buffer[n] += noise;
    }
}

void HissProcessor::process (const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& outBlock = context.getOutputBlock();
    auto numChannels = outBlock.getNumChannels();
    auto numSamples = outBlock.getNumSamples();

    // Iterate over channels and process noise per channel
    for (size_t ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = outBlock.getChannelPointer(ch);
        
        processBlock(channelData, static_cast<int>(numSamples));
        
        // Wrap the raw buffer into a JUCE AudioBlock for DSP processing
        juce::dsp::AudioBlock<float> channelBlock(&channelData, 1, numSamples);
        juce::dsp::ProcessContextReplacing<float> eqContext(channelBlock);
        
        preProcessEQ.process(eqContext);
    }
}

void HissProcessor::reset ()
{
    prevGain = curGain;
    preProcessEQ.reset();
}

void HissProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == Params::HISS)
    {
        prevGain = curGain;
        hissPercentage = newValue;
        setGain(hissPercentage);
    }
}
