/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Trombone.h"
#include "LowPass.h"

//==============================================================================
/**
*/
class TrombonePluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TrombonePluginAudioProcessor();
    ~TrombonePluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    std::shared_ptr<Trombone> getTrombonePtr() { return trombone; };
    
private:
    //==============================================================================
#ifdef NOEDITOR
    // Parameters
    AudioParameterFloat* tubeLength;
    AudioParameterFloat* lipFrequency;
    AudioParameterFloat* pressure;
#endif
    
    // Sample rate
    double fs;
    
    // Trombone variables
    std::shared_ptr<Trombone> trombone;
    std::vector<std::vector<double>> geometry;
    int controlHeight, controlY;
        
    std::unique_ptr<LowPass> lowPass;
    
    bool init = true;
    double LVal, LValPrev, lipFreqVal, lipFreqValPrev, pressureVal;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrombonePluginAudioProcessor)
};
