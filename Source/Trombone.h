/*
  ==============================================================================

    Trombone.h
    Created: 5 Sep 2020 1:12:46pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Global.h"
#include "Tube.h"
#include "LipModel.h"
//==============================================================================
/*
*/
class Trombone  : public juce::Component
{
public:
    Trombone (NamedValueSet& parameters, double k, std::vector<std::vector<double>>& geometry);
    ~Trombone() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void calculate();
    void calculateEnergy();
    
    float getOutput() { return tube->getOutput(); };
    float getLipOutput() { return lipModel->getY(); };
    
    void updateStates();

    void refreshLipModelInputParams() { lipModel->refreshInputParams(); };

    double getLVal() { return tube->getLVal(); };
    void setExtVals (double pVal, double lFVal, double LVal, bool wait = false) {
//        lipModel->setExtVals (pVal, 2.0 * tube->getC() / (LVal));
        shouldWait = wait;
        lipModel->setExtVals (pVal, lFVal);
        tube->setExtVals (LVal);
//        std::cout << lFVal << " " << 2.0 * tube->getC() / (LVal) << std::endl;
    };
    
    void changeSetting () { tube->changeSetting(); };
    void changeSetting (bool b) { tube->changeSetting(b); };
    
    double getTubeC()  { return tube->getC(); };
    double getTubeRho()  { return tube->getRho(); };

    void setWait (bool w) { shouldWait = w; };
    
    double getBellRad() { tube->getBellRad(); };
private:
    std::unique_ptr<Tube> tube;
    std::unique_ptr<LipModel> lipModel;
    
    double k, Pm;
    
    double scaledTotEnergy = 0;
    
    bool shouldLowPassConnection = false;
    bool shouldDispCorr = Global::useDispCorr;

    bool shouldWait = false; // wait with changing L?
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Trombone)
};
