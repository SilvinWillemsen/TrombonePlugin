/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TrombonePluginAudioProcessorEditor::TrombonePluginAudioProcessorEditor (TrombonePluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    trombone = audioProcessor.getTrombonePtr();
    addAndMakeVisible (trombone.get());

    startTimerHz (15);
    if (!Global::useMicInput)
    {
        pressureSlider.setRange (0, 6000);
        pressureSlider.setValue (300 * Global::pressureMultiplier);
        addAndMakeVisible (pressureSlider);
        pressureSlider.addListener (this);
        pressureSlider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    }
    pressureVal = 0;
    LVal = Global::LnonExtended;
    lipFreqVal = 2.4 * trombone->getTubeC() / (trombone->getTubeRho() * LVal);
    
    setSize (800, 600);

}

TrombonePluginAudioProcessorEditor::~TrombonePluginAudioProcessorEditor()
{
}

//==============================================================================
void TrombonePluginAudioProcessorEditor::paint (juce::Graphics& g)
{

    g.fillAll (Colours::lightgrey);

    int bottombarHeight = 40;
    int textWidth = 50;
    int numDecimals = 2;
    int margin = 20;
    
    String decimalString = "";
    
    for (int i = 0; i < numDecimals; ++i)
    {
        decimalString += "8";
    }
    
    bottomBar = getLocalBounds()
        .withHeight (bottombarHeight)
        .withY (getHeight()-bottombarHeight);
    bottomBar.removeFromLeft (margin);
    
    Font font = g.getCurrentFont();
    StringArray strings = { "Lip Frequency (Hz): ", "Length (m): ", "Pressure (Pa): " };
    // Lipfreq label
    g.drawText(strings[0]
               + String (floor(lipFreqVal * pow(10, numDecimals)) * pow(10, -numDecimals)),
               bottomBar.removeFromLeft (font.getStringWidth(strings[0] + "888." + decimalString)),
               Justification::centredLeft);
    bottomBar.reduce (margin, 0);
    
    // Tubelength label
    g.drawText (strings[1]
               + String (floor(trombone->getLVal() * pow(10, numDecimals)) * pow(10, -numDecimals)),
               bottomBar.removeFromLeft (font.getStringWidth(strings[1] + "8." + decimalString)),
               Justification::centredLeft);
    bottomBar.removeFromLeft (margin);
    
    // Pressure label
    g.setColour (pressureVal == 0 ? Colours::grey : Colours::black);
    g.drawText(strings[2]
               + String (floor((Global::useMicInput ? pressureVal : pressureValSave) * pow(10, numDecimals)) * pow(10, -numDecimals)),
               bottomBar.removeFromLeft (font.getStringWidth (strings[2] + "8888." + decimalString)),
               Justification::centredLeft);
    
//    g.drawText("LowPass: " + String (lowPass->isOn() ? "on" : "off"),
//               getWidth() - 120, getHeight() - 40, 100, 40, Justification::centredRight);

    g.setColour (Colours::gold);
    g.setOpacity (mouseEllipseVisible ? 1.0 : 0.0);
    g.fillEllipse (mouseLocX-5, mouseLocY-5, 10, 10);
    g.setColour (Colours::black);
    g.drawLine (0, getHeight() - controlHeight * 0.5, getWidth(), getHeight() - controlHeight * 0.5);

    if (init)
    {
        sliderBounds = bottomBar;
        resized();
        init = false;
    }

}

void TrombonePluginAudioProcessorEditor::resized()
{
    controlHeight = 0.3 * getHeight();
    controlY = getHeight() - controlHeight;
    trombone->setBounds (getLocalBounds().withHeight (controlY));

    if (!Global::useMicInput)
        pressureSlider.setBounds (sliderBounds);
}

void TrombonePluginAudioProcessorEditor::timerCallback()
{
    repaint();
}

void TrombonePluginAudioProcessorEditor::mouseDown (const MouseEvent& e)
{
    mouseEllipseVisible = true;
}

void TrombonePluginAudioProcessorEditor::mouseDrag(const MouseEvent& e)
{
    double xRatio = e.x / static_cast<double> (getWidth());

    double fineTuneRange = 0.5;
    double fineTune = fineTuneRange * 2 * (e.y - controlY - controlHeight * 0.5) / controlHeight;
    //    lipFreqVal = ((1-xRatio) + xRatio * Global::LnonExtended/Global::Lextended) * Global::nonExtendedLipFreq * (1 + fineTune);
    LVal = Global::LnonExtended + (Global::Lextended - Global::LnonExtended) * e.x / static_cast<double> (getWidth());
    lipFreqVal = 2.4 * trombone->getTubeC() / (trombone->getTubeRho() * LVal) * (1.0 + fineTune);
    lipFreqVal = Global::limit (lipFreqVal, 20, 1000);

    //    lipFreqVal = 1.2 * trombone->getTubeC() / (trombone->getTubeRho() * LVal);
    if (!Global::useMicInput)
    {
        pressureVal = pressureSlider.getValue();
        pressureValSave = pressureVal; // used for text
    }
    trombone->setExtVals (pressureVal, lipFreqVal, LVal);
    mouseLocX = e.x;
    mouseLocY = e.y;
}

void TrombonePluginAudioProcessorEditor::mouseUp (const MouseEvent& e)
{
    pressureVal = 0;

    trombone->setExtVals (pressureVal, lipFreqVal, LVal);
    mouseEllipseVisible = false;

}


void TrombonePluginAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    pressureValSave = pressureSlider.getValue();
//    if (slider == &pressureSlider)
//        pressureVal = pressureSlider.getValue();
//    trombone->setExtVals (pressureVal, lipFreqVal, LVal);
}
