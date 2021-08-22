/*
  ==============================================================================

    LowPass.cpp
    Created: 7 Apr 2021 2:16:52pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LowPass.h"

//==============================================================================
LowPass::LowPass (std::vector<double> bCoeffs, std::vector<double> aCoeffs)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    filterOrder = std::max(bCoeffs.size(), aCoeffs.size());
    
    // zero-pad
    while (bCoeffs.size() != aCoeffs.size())
    {
        if (bCoeffs.size() < aCoeffs.size())
            bCoeffs.push_back (0);
        else
            aCoeffs.push_back (0);
    }
    b.reserve (filterOrder);
    a.reserve (filterOrder);
    
    x.resize (filterOrder, 0);
    y.resize (filterOrder, 0);

    for (int i = 0; i < filterOrder; ++i)
    {
        b.push_back (bCoeffs[i]);
        a.push_back (aCoeffs[i]);
    }
}

LowPass::~LowPass()
{
}

void LowPass::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("LowPass", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void LowPass::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

float LowPass::filter (float input)
{
    if (!active)
        return input;
    x[0] = input;

    output = b[0] * x[0];
    for (int i = 1; i < filterOrder; ++i)
    {
        output += (b[i] * x[i] - a[i] * y[i]);
    }
    
    y[0] = output;
    // update states
    for (int i = filterOrder-1; i > 0; --i)
    {
        x[i] = x[i-1];
        y[i] = y[i-1];
    }
    
    return output;
    
}

