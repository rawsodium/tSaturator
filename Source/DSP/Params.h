/*
  ==============================================================================

    Params.h
    Created: 26 Mar 2025 2:05:55pm
    Author:  Deanna Turner

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/*
 A class to hold the paramID of each parameter in the application
 */
class Params
{
public:
    inline static juce::String SATURATION = "Saturation";
    inline static juce::String DRIVE = "Drive";
    inline static juce::String MIX = "Mix";
    inline static juce::String HISS = "Hiss";
};
