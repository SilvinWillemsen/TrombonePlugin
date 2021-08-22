/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Global.h"
#include "Trombone.h"
#include "LowPass.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent, public Timer, public Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    void timerCallback() override;

    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    
    void sliderValueChanged (Slider* slider) override;

private:

    // Your private member variables go here...
    std::unique_ptr<Trombone> trombone;
    double fs;
    long t = 0;
    bool done = false;
    std::vector<std::vector<double>> geometry;
    double pressureVal, lipFreqVal, LVal;
    int controlHeight, controlY;
    
    bool record = true;
    double mouseLocX = 0;
    double mouseLocY = 0;
    bool mouseEllipseVisible = false;
    
    std::unique_ptr<LowPass> lowPass;
    
    Slider pressureSlider;
    Rectangle<int> sliderBounds { 0, 0, 100, 40 };
    Rectangle<int> bottomBar;
    bool init = true;
    double pressureValSave = 0;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
