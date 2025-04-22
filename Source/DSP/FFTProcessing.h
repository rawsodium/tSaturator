/*
  ==============================================================================

    FFTProcessing.h
    Created: 19 Apr 2025 10:18:08pm
    Author:  Deanna Turner

  ==============================================================================
*/

/* 
 This file contains code adapted from JUCE's official Spectrum Analyser tutorial. Under the terms of the ISC license
 included with this project, this is a fair use of this code. Thus, this project is also under the same licensing terms.
*/

#include "../GUI/Utilities.hpp"
#pragma once

/*
 Takes incoming audio data and runs it through an FFT for the spectrum analyzer to display.
 */
class FFTAnalyzer
{
public:
    static constexpr int fftOrder = 11;
    static constexpr int fftSize = 1 << fftOrder;
    static constexpr int scopeSize = 2048;
    static constexpr int numChannels = 2;

    FFTAnalyzer()
        : forwardFFT(fftOrder),
          window(fftSize, juce::dsp::WindowingFunction<float>::hann)
    {}

    void pushSample(int channel, float sample)
    {
        jassert(channel < numChannels);

        if (fifoIndex[channel] == fftSize)
        {
            if (!nextFFTBlockReady[channel])
            {
                juce::zeromem(fftData[channel], sizeof(fftData[channel]));
                memcpy(fftData[channel], fifo[channel], sizeof(fifo[channel]));
                nextFFTBlockReady[channel] = true;
            }

            fifoIndex[channel] = 0;
        }

        fifo[channel][fifoIndex[channel]++] = sample;
    }

    void processFFT()
    {
        constexpr float smoothingCoeff = 0.7f;
        
        for (int ch = 0; ch < numChannels; ++ch)
        {
            if (!nextFFTBlockReady[ch]) continue;

            window.multiplyWithWindowingTable(fftData[ch], fftSize);
            forwardFFT.performFrequencyOnlyForwardTransform(fftData[ch]);
            
            int binWidth = 5;

            for (int i = 0; i < scopeSize; ++i)
            {
                auto skewedX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.2f);
                int centerBin = juce::jlimit(0, fftSize / 2, (int)(skewedX * fftSize * 0.5f));
                
                float avgMagnitude = 0.0f;
                int binCount = 0;
                
                for (int j = -binWidth; j <= binWidth; ++j)
                {
                    int bin = juce::jlimit(0, fftSize / 2, centerBin + j);
                    avgMagnitude += fftData[ch][bin];
                    ++binCount;
                }
                
                avgMagnitude /= (float)binCount;
                auto levelInDB = juce::Decibels::gainToDecibels(fftData[ch][centerBin]);
                levelInDB = juce::jlimit(MIN_DB, MAX_DB, levelInDB);
                
                if (scopeData[ch][i] == 0.0f)
                    scopeData[ch][i] = levelInDB;
                else
                    scopeData[ch][i] = smoothingCoeff * scopeData[ch][i] + (1.0f - smoothingCoeff) * levelInDB;
            }
            nextFFTBlockReady[ch] = false;
        }
    }
    const float* getScopeData(int channel) const { return scopeData[channel]; }
private:
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;
    float fifo[numChannels][fftSize] = {};
    float fftData[numChannels][2 * fftSize] = {};
    float scopeData[numChannels][scopeSize] = {};
    int fifoIndex[numChannels] = {};
    bool nextFFTBlockReady[numChannels] = {};
};
