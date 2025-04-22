//
//  Utilities.cpp
//  TapeSaturation - VST3
//
//  Created by Deanna Turner on 3/11/25.
//

#include "Utilities.hpp"

juce::Rectangle<int> getModuleBackgroundArea(juce::Rectangle<int> bounds)
{
    bounds.reduce(3, 3);
    return bounds;
}
