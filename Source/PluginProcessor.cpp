/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TrombonePluginAudioProcessor::TrombonePluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
#ifdef NOEDITOR
    addParameter (gain = new AudioParameterFloat ("gain", // parameter ID
        "Gain", // parameter name
        0.0f,   // minimum value
        1.0f,   // maximum value
        0.1f)); // default value
    addParameter (frequency = new AudioParameterFloat ("frequency", // parameter ID
        "Frequency", // parameter name
        20.0f,   // minimum value
        880.0f,   // maximum value
        440.0f)); // default value
#endif
    
}

TrombonePluginAudioProcessor::~TrombonePluginAudioProcessor()
{
}

//==============================================================================
const juce::String TrombonePluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TrombonePluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TrombonePluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TrombonePluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TrombonePluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TrombonePluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TrombonePluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TrombonePluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TrombonePluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void TrombonePluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TrombonePluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
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
    trombone = std::make_shared<Trombone> (parameters, 1.0 / fs, geometry);
    
    double LVal = (*parameters.getVarPointer ("LnonExtended")); // start by contracting
    double lipFreqVal = 2.4 * trombone->getTubeC() / (trombone->getTubeRho() * LVal);

    trombone->setExtVals (0, lipFreqVal, LVal, true);
    
    lowPass = std::make_unique<LowPass> (std::vector<double> { 0.0001343, 0.0005374, 0.0008060, 0.0005374, 0.0001343 },
                                          std::vector<double> {1, -3.3964, 4.3648, -2.5119, 0.5456 });
}

void TrombonePluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TrombonePluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TrombonePluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    float output = 0.0;
    auto* channelData = buffer.getWritePointer(0);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        trombone->calculate();
        output = trombone->getOutput() * 0.001 * Global::oOPressureMultiplier;
        output = lowPass->filter (output);

        trombone->updateStates();
        channelData[i] = Global::outputClamp (output);
    }
    if (totalNumOutputChannels > 1)
    {
        auto* channelDataL = buffer.getReadPointer(0);
        auto* channelDataR = buffer.getWritePointer(1);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            channelDataR[i] = channelDataL[i];
        
    }
    trombone->refreshLipModelInputParams();

}

//==============================================================================
bool TrombonePluginAudioProcessor::hasEditor() const
{
#ifdef NOEDITOR
    return false; // (change this to false if you choose to not supply an editor)
#else
    return true;
#endif
}

juce::AudioProcessorEditor* TrombonePluginAudioProcessor::createEditor()
{
    return new TrombonePluginAudioProcessorEditor (*this);
}

//==============================================================================
void TrombonePluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TrombonePluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TrombonePluginAudioProcessor();
}
