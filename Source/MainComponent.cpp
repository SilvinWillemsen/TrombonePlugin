/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    
    // specify the number of input and output channels that we want to open
    
    setAudioChannels (Global::useMicInput ? 2 : 0, 2);
    startTimerHz (15);
    
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    stopTimer();
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{

    if (sampleRate != 44100)
        std::cout << "sampleRate is not 44100 Hz!!" << std::endl;
    
    fs = sampleRate;
    NamedValueSet parameters;
    
    //// Tube ////
    parameters.set ("T", 26.85);
    parameters.set ("LnonExtended", Global::LnonExtended);
    parameters.set ("Lextended", Global::Lextended);
    parameters.set ("L", Global::LnonExtended);
//    parameters.set ("L", 3.653);


    // Geometry
//    parameters.set ("mp", 0.015 * 0.015 * double_Pi);                   // mouthpiece cross-sectional area
//    parameters.set ("mpL", 0.01);                   // mouthpiece length (length ratio)
//    parameters.set ("m2tL", 0.01);                  // mouthpiece to tube (length ratio)
//    parameters.set ("tubeS", 0.01 * 0.01 * double_Pi);                 // tube cross-sectional area
    
    // Geometric information including formula from bell taken from T. Smyth "Trombone synthesis by model and measurement"
    geometry = {
        {0.708, 0.177, 0.711, 0.241, 0.254, 0.502},         // lengths
        {0.0069, 0.0072, 0.0069, 0.0071, 0.0075, 0.0107}    // radii
    };
    
    parameters.set ("flare", 0.7);                 // flare (exponent coeff)
    parameters.set ("x0", 0.0174);                    // position of bell mouth (exponent coeff)
    parameters.set ("b", 0.0063);                   // fitting parameter
    parameters.set ("bellL", 0.21);                  // bell (length ratio)
    
    //// Lip ////
    double f0 = 300;
    double H0 = 2.9e-4;
    parameters.set("f0", f0);                       // fundamental freq lips
    parameters.set("Mr", 5.37e-5);                  // mass lips
    parameters.set("omega0", 2.0 * double_Pi * f0); // angular freq
    
    parameters.set("sigmaR", 5);                    // damping
    parameters.set("H0", H0);                       // equilibrium
    parameters.set("barrier", -H0);                 // equilibrium

    parameters.set("w", 1e-2);                      // lip width
    parameters.set("Sr", 1.46e-5);                  // lip area
    
    parameters.set ("Kcol", 10000);
    parameters.set ("alphaCol", 3);
    
    //// Input ////
    parameters.set ("Pm", (Global::exciteFromStart ? 300 : 0) * Global::pressureMultiplier);
//    LVal = (*parameters.getVarPointer ("Lextended"));
    trombone = std::make_unique<Trombone> (parameters, 1.0 / fs, geometry);
    addAndMakeVisible (trombone.get());
    
    pressureVal = 0;
    LVal = (*parameters.getVarPointer ("LnonExtended")); // start by contracting
    lipFreqVal = 2.4 * trombone->getTubeC() / (trombone->getTubeRho() * LVal);

    trombone->setExtVals (pressureVal, lipFreqVal, LVal, true);
    
    lowPass = std::make_unique<LowPass> (std::vector<double> { 0.0001343, 0.0005374, 0.0008060, 0.0005374, 0.0001343 },
                                          std::vector<double> {1, -3.3964, 4.3648, -2.5119, 0.5456 });
    if (~Global::useMicInput)
    {
        pressureSlider.setRange (0, 6000);
        pressureSlider.setValue (300 * Global::pressureMultiplier);
        addAndMakeVisible (pressureSlider);
        pressureSlider.addListener (this);
        pressureSlider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    }
    setSize (800, 600);

}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    
    float* const channelData1 = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
    float* const channelData2 = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample);
    
    float output = 0.0;

//    if (Global::useMicInput)
//    {
//        const float* input = bufferToFill.buffer->getReadPointer (0, bufferToFill.startSample);
//        double avg = 0;
//        for (int i = 0; i < bufferToFill.numSamples; ++i)
//            avg += input[i] * input[i];
//        avg /= bufferToFill.numSamples;
//        avg = sqrt(avg);
//
//        pressureVal = 1200 * avg;
//        trombone->setExtVals(1200 * avg, lipFreqVal, LVal);
//    }
    
    for (int i = 0; i < bufferToFill.numSamples; ++i)
    {
        trombone->calculate();
        output = trombone->getOutput() * 0.001 * Global::oOPressureMultiplier;
        output = lowPass->filter (output);

//        if (!done && Global::saveToFiles && t >= Global::startSample)
//        {
//            trombone->saveToFiles();
//        }
        ++t;
        

        trombone->updateStates();
        channelData1[i] = Global::outputClamp (output);
        channelData2[i] = Global::outputClamp (output);
    }
    if (Global::saveToFiles && t >= Global::stopSample && !done)
    {
        done = true;
        trombone->closeFiles();
        std::cout << "done" << std::endl;
    }
    trombone->refreshLipModelInputParams();
//    std::cout << output << std::endl;
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    if (lowPass2->isOn())
//        g.fillAll (Colours::green);
//    else if (lowPass4->isOn())
//        g.fillAll (Colours::yellow);
//    else
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
               + String (floor(LVal * pow(10, numDecimals)) * pow(10, -numDecimals)),
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

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    controlHeight = 0.3 * getHeight();
    controlY = getHeight() - controlHeight;
    trombone->setBounds (getLocalBounds().withHeight (controlY));

    if (!Global::useMicInput)
        pressureSlider.setBounds (sliderBounds);
}

void MainComponent::timerCallback()
{
    repaint();
}


void MainComponent::mouseDown (const MouseEvent& e)
{
//    pressureVal = e.y * Global::pressureMultiplier;
//    lipFreqVal = e.x;
//    lipFreqVal = e.y;
    if (e.x > getWidth() - 10)
    {
//        trombone->changeSetting();
//        setting = !setting;
//        record = true;
//        lowPass->toggleOnOff();
        return;
    }
    
//    double xRatio = e.x / static_cast<double> (getWidth());
//    double fineTuneRange = 0.05;
//    double fineTune = fineTuneRange * 2 * (e.y - controlY - controlHeight * 0.5) / controlHeight;
////    lipFreqVal = ((1-xRatio) + xRatio * Global::LnonExtended/Global::Lextended) * Global::nonExtendedLipFreq * (1 + fineTune);
//    LVal = Global::LnonExtended + 1.06 * e.x / static_cast<double> (getWidth());
//    lipFreqVal = 2.25 * trombone->getTubeC() / (trombone->getTubeRho() * LVal) * (1.0 + fineTune);
////    pressureVal = 300 * Global::pressureMultiplier * (1.0+fineTune);
//    pressureVal = 300 * Global::pressureMultiplier;
//
//    trombone->setExtVals (pressureVal, lipFreqVal, LVal);
    mouseEllipseVisible = true;
}


void MainComponent::mouseDrag (const MouseEvent& e)
{
//    if (e.x > getWidth() - 10)
//    {
//        return;
//    }
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

void MainComponent::mouseUp (const MouseEvent& e)
{
    pressureVal = 0;
//    lipFreqVal = e.x;
//    lipFreqVal = e.y;
//    double xRatio = e.x / static_cast<double> (getWidth());
//    lipFreqVal = ((1-xRatio) + xRatio * 2.658/3.718) * 520;
//    LVal = 2.658 + 1.06 * e.x / static_cast<double> (getWidth());

    trombone->setExtVals (pressureVal, lipFreqVal, LVal);
    mouseEllipseVisible = false;

}

void MainComponent::sliderValueChanged (Slider* slider)
{
    pressureValSave = pressureSlider.getValue();
//    if (slider == &pressureSlider)
//        pressureVal = pressureSlider.getValue();
//    trombone->setExtVals (pressureVal, lipFreqVal, LVal);
}
