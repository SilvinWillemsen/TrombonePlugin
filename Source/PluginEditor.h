/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class TrombonePluginAudioProcessorEditor  : public juce::AudioProcessorEditor, public Timer, public Slider::Listener
{
public:
    TrombonePluginAudioProcessorEditor (TrombonePluginAudioProcessor&);
    ~TrombonePluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;
    void sliderValueChanged (Slider* slider) override;
    
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TrombonePluginAudioProcessor& audioProcessor;
    std::shared_ptr<Trombone> trombone;
    
    Slider pressureSlider;
    Rectangle<int> sliderBounds { 0, 0, 100, 40 };
    Rectangle<int> bottomBar;

    int controlHeight, controlY;
    
    double mouseLocX = 0;
    double mouseLocY = 0;
    bool mouseEllipseVisible = false;
    
    bool init = true;
    double pressureVal, lipFreqVal, LVal;
    double pressureValSave = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrombonePluginAudioProcessorEditor)
};
