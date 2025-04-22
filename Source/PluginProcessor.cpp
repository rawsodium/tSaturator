/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/MixProcessor.h"
#include "DSP/DriveProcessor.h"
#include "DSP/HissProcessor.h"
#include "DSP/Params.h"

//==============================================================================
TapeSaturationAudioProcessor::TapeSaturationAudioProcessor() :
AudioProcessor (BusesProperties()
                .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
apvts {*this, nullptr, juce::Identifier("Controls"), createParameterLayout()}
{
}

TapeSaturationAudioProcessor::~TapeSaturationAudioProcessor()
{
}

//==============================================================================
const juce::String TapeSaturationAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TapeSaturationAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TapeSaturationAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TapeSaturationAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TapeSaturationAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TapeSaturationAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TapeSaturationAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TapeSaturationAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TapeSaturationAudioProcessor::getProgramName (int index)
{
    return {};
}

void TapeSaturationAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TapeSaturationAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;
    
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    spec.sampleRate = sampleRate;
    
    dp.prepare(spec);
    sp.prepare(spec);
    hp.prepare(spec);
    mp.prepare(spec);
}

void TapeSaturationAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TapeSaturationAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TapeSaturationAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data
    
    // mark to delete??
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto block = juce::dsp::AudioBlock<float>(buffer);
    
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    auto inBlock = context.getInputBlock();
    
    mp.setDryBlock(inBlock);
    dp.process(context);
    sp.process(context);
    hp.process(context);
    mp.process(context);
    
    auto outBlock = context.getOutputBlock();
    juce::AudioBuffer<float> outBuf;
    
    outBuf.setSize((int)outBlock.getNumChannels(), (int)outBlock.getNumSamples(), false, false, true);
    // Copy output samples into the storage buffer
    for (size_t ch = 0; ch < outBlock.getNumChannels(); ++ch)
    {
        outBuf.copyFrom((int)ch, 0, outBlock.getChannelPointer(ch), (int)outBlock.getNumSamples());
    }
    
    auto* chL = outBuf.getReadPointer(0);
    auto* chR = outBuf.getNumChannels() > 1 ? outBuf.getReadPointer(1) : chL;

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        analyzer.pushSample(0, chL[i]);
        analyzer.pushSample(1, chR[i]);
    }
}

//==============================================================================
bool TapeSaturationAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TapeSaturationAudioProcessor::createEditor()
{
    return new TapeSaturationAudioProcessorEditor (*this);
}

//==============================================================================
void TapeSaturationAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    // save params upon close/open
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void TapeSaturationAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes));
    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout TapeSaturationAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(Params::SATURATION, 1),
                                                           Params::SATURATION,
                                                           juce::NormalisableRange<float>(0.f, 100.f, 1.f, 0.25f),
                                                           0.f));
    // default of 1, otherwise some funky stuff happens...
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(Params::DRIVE, 2),
                                                           Params::DRIVE,
                                                           juce::NormalisableRange<float>(0.f, 10.f, 1.f, 0.25f),
                                                           1.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(Params::MIX, 3),
                                                           Params::MIX,
                                                           juce::NormalisableRange<float>(0.f, 100.f, 1.f, 0.25f),
                                                           0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(Params::HISS, 4),
                                                           Params::HISS,
                                                           juce::NormalisableRange<float>(0.f, 100.f, 1.f, 0.25f),
                                                           0.f));
    return layout;
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TapeSaturationAudioProcessor();
}
