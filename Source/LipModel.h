/*
  ==============================================================================

    LipModel.h
    Created: 5 Sep 2020 1:11:22pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Global.h"

//==============================================================================
/*
*/
class LipModel  : public juce::Component
{
public:
    LipModel (NamedValueSet& parameters, double k);
    ~LipModel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setTubeParameters (double hIn, double rho, double c, double SBar0In, double SHalf0In);
    void setTubeStates (double p, double vNext) { p0 = p; vNext0 = vNext; };
    void calculateCollision();
    void calculateDeltaP();
    
    double getUb() { return Ub; };
    double getUr() { return Ur; };

    double getY() { return y; };
    
    void calculate();
    void updateStates();
    
    double getLipEnergy();
    double getLipEnergy1() { return lipEnergy1; };

    double getCollisionEnergy();
    double getCollisionEnergy1() { return colEnergy1; };

    double getDampEnergy();
    double getPower();
    
    void refreshInputParams();
    
    void setExtVals (double pValIn, double lFVal) { pressureVal = pValIn; lipFreqVal = lFVal; };
    
    
    
private:
    double k, omega0, M, sig, Sr, w, Kcol, alpha, H0, b, g, kappa, psi, psiPrev, Pm, Ub, Ur;
    double etaNext, eta, etaPrev;
    double oOk, omega0Sq, kO2M, oOM, oOa1, oO2k;
    double h, SBar0, SHalf0, vNext0, p0;
    
    double a1, a2, a3, b1, b2, c1, c2, c3;
    double a1Coeff, bCoeff, c1Coeff;
    double deltaPTerm, deltaP;
    
    double oOAlpha, beta, xi, gammaR;
    
    double yNext;
    double y;
    double yPrev;
//    double yTmp;
    
    double lipEnergy1 = -1;
    double colEnergy1 = -1;
    
    double pHPrev, qHPrev = 0;
    
    double pressureVal;
    double lipFreqVal;

    bool divisBy0Flag = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LipModel)
};
