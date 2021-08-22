/*
  ==============================================================================

    LowPass.h
    Created: 7 Apr 2021 2:16:52pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class LowPass  : public juce::Component
{
public:
    LowPass (std::vector<double> bCoeffs, std::vector<double> aCoeffs);
    ~LowPass() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    float filter (float input);
    void toggleOnOff() { active = !active; };
    
    bool isOn() { return active; };
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LowPass)
    
    std::vector<double> b;
    std::vector<double> a;
    
    std::vector<double> x;
    std::vector<double> y;
    float output;
    
    int filterOrder;
    bool active = true;
};
