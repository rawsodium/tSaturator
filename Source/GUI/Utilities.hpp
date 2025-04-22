//
//  Utilities.hpp
//  TapeSaturation - VST3
//
//  Created by Deanna Turner on 3/11/25.
//

#pragma once

#include <JuceHeader.h>

// any constants need to be defined as a static constexpr <insert type>
static constexpr float NEG_INFINITY = -72.f;
static constexpr float SAMPLE_RATE = 44100.f;
static constexpr float MIN_FREQ = 20.f;
static constexpr float MAX_FREQ = 20000.f;
static constexpr float MAX_DB = 36.f;
static constexpr float MIN_DB = -12.f;

juce::Rectangle<int> getModuleBackgroundArea(juce::Rectangle<int> bounds);
