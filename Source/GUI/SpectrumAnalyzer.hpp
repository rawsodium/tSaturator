//
//  SpectrumAnalyzer.hpp
//  TapeSaturation - VST3
//
//  Created by Deanna Turner on 3/9/25.
//

#pragma once

#include <JuceHeader.h>
#include "../DSP/FFTProcessing.h"
#include "Utilities.hpp"

/* 
 This file contains code adapted from JUCE's official Spectrum Analyser tutorial. Under the terms of the ISC license
 included with this project, this is a fair use of this code. Thus, this project is also under the same licensing terms.
 This is fair use!
*/
/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*
 Utils class for this component
 */
struct SpectrumAnalyzerUtils
{
    static juce::Rectangle<int> getRenderArea(juce::Rectangle<int> bounds)
    {
        bounds.removeFromTop(12);
        bounds.getBottom();
        bounds.removeFromLeft(20);
        bounds.removeFromRight(20);
    
        return bounds;
    }
    
    static juce::Rectangle<int> getAnalysisArea(juce::Rectangle<int> bounds)
    {
        bounds = getRenderArea(bounds);
        bounds.removeFromTop(4);
//        bounds.removeFromBottom(4);
        return bounds;
    }
};

/*
 Render logic for drawing the L and R channels on the analyzer based on the FFT data processed by the application
 */
class SpectrumAnalyzer : public juce::Component, private juce::Timer
{
public:
    SpectrumAnalyzer(FFTAnalyzer& analyzerRef) : analyzer(analyzerRef)
    {
        startTimerHz(60);
    }

    void timerCallback() override
    {
        if (shouldShowFFTAnalysis)
        {
            auto bounds = getLocalBounds();
            auto fftBounds = SpectrumAnalyzerUtils::getAnalysisArea(bounds).toFloat();
            fftBounds.setBottom(bounds.getBottom());
            analyzer.processFFT();
        }
        repaint();
    }

    juce::Path generateSpectrumPath(const float* data, size_t dataSize,
                                     juce::Rectangle<int> bounds,
                                     float minFreq = MIN_FREQ, float maxFreq = MAX_FREQ,
                                     float minDB = MIN_DB, float maxDB = MAX_DB,
                                     int smoothingPasses = 3)
    {
        juce::Path p;
        auto width = bounds.getWidth();

        auto mapFreqToX = [=](float freq) {
            float normX = std::log10(freq / minFreq) / std::log10(maxFreq / minFreq);
            return normX * width;
        };

        std::vector<float> smoothed(data, data + dataSize);

        for (int pass = 0; pass < smoothingPasses; ++pass)
        {
            std::vector<float> temp(smoothed);
            for (size_t i = 1; i < dataSize - 1; ++i)
            {
                smoothed[i] = (temp[i - 1] + temp[i] + temp[i + 1]) / 3.0f;
            }
        }

        bool started = false;
        for (size_t i = 0; i < dataSize; ++i)
        {
            float normPos = (float)i / (float)(dataSize - 1);
            float freq = std::pow(10.0f, normPos * (std::log10(maxFreq) - std::log10(minFreq)) + std::log10(minFreq));
            float x = mapFreqToX(freq);

            float dB = juce::jlimit(minDB, maxDB, smoothed[i]);
            float y = juce::jmap(dB, minDB, maxDB, (float)bounds.getBottom(), (float)bounds.getY());

            if (!started)
            {
                p.startNewSubPath(x, y);
                started = true;
            }
            else
            {
                p.lineTo(x, y);
            }
        }

        return p;
    }
    
    float dBtoY(float dB) const
    {
        auto localBounds = getLocalBounds();
        auto bounds = SpectrumAnalyzerUtils::getAnalysisArea(localBounds).toFloat();
        return juce::jmap(dB, -36.f, 12.f, bounds.getBottom(), bounds.getY());
    }

    float freqToX(float freq) const
    {
        auto localBounds = getLocalBounds();
        auto bounds = SpectrumAnalyzerUtils::getAnalysisArea(localBounds).toFloat();

        float normX = std::log10(freq / MIN_FREQ) / std::log10(MAX_FREQ / MIN_FREQ);
        if (freq == MIN_FREQ)
        {
            return bounds.getX();
        } else if (freq == MAX_FREQ)
        {
            return bounds.getRight();
        } else {
            return normX * bounds.getWidth();
        }
    }

    void paint(juce::Graphics& g) override
    {
        auto localBounds = getLocalBounds();
        auto analysisArea = SpectrumAnalyzerUtils::getAnalysisArea(localBounds);
        g.setColour(TSPalette::getAnalysisAreaBackgroundColor());
        g.fillRect(analysisArea);
        auto bounds = getModuleBackgroundArea(localBounds);
        drawGridAndLabels(g, bounds);
        drawSpectrum(g, bounds);
    }

    void drawGridAndLabels(juce::Graphics& g, juce::Rectangle<int> bounds)
    {
        auto renderArea = SpectrumAnalyzerUtils::getAnalysisArea(bounds);
        auto left = renderArea.getX();
        auto right = renderArea.getRight();
        auto top = renderArea.getY();
        auto bottom = renderArea.getBottom();
        g.setColour(TSPalette::getGridlineColor());
        g.setOpacity(0.3f);

        const int fontHeight = 10;
        g.setFont(fontHeight);
        
        for (size_t i = 0; i < freqs.size(); ++i)
        {
            // draw gridlines
            float f = freqs[i];
            float x = freqToX(f);
            g.setColour(TSPalette::getGridlineColor());
            g.setOpacity(0.3f);
            g.drawVerticalLine((int)x, top, bottom);
            
            // draw labels
            g.setColour(TSPalette::getLabelTicksAndTextColor());
            g.setOpacity(1.0f);
            juce::String str = hertz[i];
            auto textWidth = g.getCurrentFont().getStringWidth(str);
            int labelX = (int)(x - textWidth * 0.5f);
            int labelY = top - fontHeight - 2;
            g.drawText(str, labelX, labelY, textWidth, fontHeight, juce::Justification::centred);
        }
        
        // dB grid lines
        for (size_t i = 0; i < gains.size(); ++i)
        {
            float dB = gains[i];
            float y = dBtoY(dB);
            g.setColour(TSPalette::getGridlineColor());
            g.setOpacity(0.3f);
            g.drawHorizontalLine((int)y, left, right);
           
            g.setColour(TSPalette::getLabelTicksAndTextColor());
            g.setOpacity(1.0f);
            juce::String str = dbs[i];
            auto textWidth = g.getCurrentFont().getStringWidth(str);
            int labelX = left - textWidth - 4;
            int labelY = (int)(y - fontHeight * 0.5f);
            if (dB == 12.f)
            {
                labelY = (int)(y + fontHeight);
            } 
            else if (dB == -36.f)
            {
                labelY = (int)(y - fontHeight * 0.75f);
            }
            g.drawText(str, labelX, labelY, textWidth, fontHeight, juce::Justification::centredLeft);
        }
    }

    void drawSpectrum(juce::Graphics& g, juce::Rectangle<int> bounds)
    {
        auto responseArea = SpectrumAnalyzerUtils::getAnalysisArea(bounds);

        juce::Graphics::ScopedSaveState sss(g);
        g.reduceClipRegion(responseArea);

        const auto* left = analyzer.getScopeData(0);
        const auto* right = analyzer.getScopeData(1);

        auto leftPath = generateSpectrumPath(left, FFTAnalyzer::scopeSize, responseArea);
        auto rightPath = generateSpectrumPath(right, FFTAnalyzer::scopeSize, responseArea);
        
        leftPath.applyTransform(juce::AffineTransform().translation(responseArea.getX(), 0));
        rightPath.applyTransform(juce::AffineTransform().translation(responseArea.getX(), 0));

        g.setColour(TSPalette::getLeftChannelColor());
        g.strokePath(leftPath, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved));

        g.setColour(TSPalette::getRightChannelColor());
        g.strokePath(rightPath, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved));
    }
    
    void toggleAnalysisEnablement(bool enabled)
    {
        shouldShowFFTAnalysis = enabled;
    }
private:
    FFTAnalyzer& analyzer;
    bool shouldShowFFTAnalysis = true;
    const std::vector<float> freqs { 20.f, 50.f, 100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f, 10000.f, 20000.f };
    const std::vector<float> gains { -36.f, -24.f, -12.f, 0.f, 12.f };
    const std::vector<juce::String> hertz
    { "20Hz", "50Hz", "100Hz", "200Hz", "500Hz", "1kHz", "2kHz", "5kHz", "10kHz", "20kHz" };
    const std::vector<juce::String> dbs { "-36", "-24", "-12", "0", "+12" };
};
