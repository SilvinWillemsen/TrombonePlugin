/*
  ==============================================================================

    Trombone.cpp
    Created: 5 Sep 2020 1:12:46pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Trombone.h"

//==============================================================================
Trombone::Trombone (NamedValueSet& parameters, double k, std::vector<std::vector<double>>& geometry) : k (k),
Pm (*parameters.getVarPointer ("Pm"))
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    tube = std::make_unique<Tube> (parameters, k, geometry);
    addAndMakeVisible (tube.get());
    lipModel = std::make_unique<LipModel> (parameters, k);
    lipModel->setTubeParameters (tube->getH(),
                                 tube->getRho(),
                                 tube->getC(),
                                 tube->getSBar(0),
                                 tube->getSHalf(0));
    addAndMakeVisible (lipModel.get());
}

Trombone::~Trombone()
{
}

void Trombone::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
}

void Trombone::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
//    Rectangle<int> totArea = getLocalBounds();
//    tube->setBounds (totArea.removeFromTop(getHeight));
//    lipModel->setBounds (totArea);
    tube->setBounds (getLocalBounds());

}

void Trombone::calculate()
{
    if (!Global::fixedNonInterpolatedL && !shouldWait)
        tube->updateL();
    if (shouldLowPassConnection)
        tube->lowPassConnection();
        
    tube->calculateVelocity();
    
    if (Global::connectedToLip)
    {   lipModel->setTubeStates (tube->getP (1, 0), tube->getV (0, 0));
        lipModel->calculateCollision();
        lipModel->calculateDeltaP();
        lipModel->calculate();
        tube->setFlowVelocities (lipModel->getUb(), lipModel->getUr());
    }
    
    tube->calculatePressure();
    tube->calculateRadiation();
    
    if (shouldDispCorr)
        tube->dispCorr();

    if (Global::fixedNonInterpolatedL)
        calculateEnergy();
}

void Trombone::calculateEnergy()
{
    bool excludeLip = !Global::connectedToLip;
//    bool excludeLip = false;

    double kinEnergy = tube->getKinEnergy();
    double potEnergy = tube->getPotEnergy();
    double radEnergy = tube->getRadEnergy();
    double radDamp = tube->getRadDampEnergy();
    double lipEnergy = lipModel->getLipEnergy();
    double lipCollisionEnergy = lipModel->getCollisionEnergy();
    double lipPower = lipModel->getPower();
    double lipDamp = lipModel->getDampEnergy();
    
    double totEnergy = kinEnergy + potEnergy + radEnergy + (excludeLip ? 0 : (lipEnergy + lipCollisionEnergy));
    double energy1 = tube->getKinEnergy1() + tube->getPotEnergy1() + tube->getRadEnergy1() + (excludeLip ? 0 : (lipModel->getLipEnergy1() + lipModel->getCollisionEnergy1()));
    
//     scaledTotEnergy = (totEnergy + lipModel->getPower() + lipModel->getDampEnergy() + tube->getRadDampEnergy() - energy1) / energy1;
    scaledTotEnergy = (totEnergy + lipPower + lipDamp + radDamp - energy1) / pow(2, floor (log2 (energy1)));
}

void Trombone::updateStates()
{
    tube->updateStates();
    lipModel->updateStates();
}
