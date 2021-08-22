/*
  ==============================================================================

    LipModel.cpp
    Created: 5 Sep 2020 1:11:22pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LipModel.h"

//==============================================================================
LipModel::LipModel (NamedValueSet& parameters, double k) : k (k),
                                            lipFreqVal (*parameters.getVarPointer ("f0")),
                                            omega0 (*parameters.getVarPointer ("omega0")),
                                            M (*parameters.getVarPointer ("Mr")),
                                            sig (*parameters.getVarPointer ("sigmaR")),
                                            Kcol (*parameters.getVarPointer ("Kcol")),
                                            alpha (*parameters.getVarPointer ("alphaCol")),
                                            H0 (*parameters.getVarPointer ("H0")),
                                            b (*parameters.getVarPointer ("barrier")),
                                            Pm (*parameters.getVarPointer ("Pm"))

{
    if (Global::connectedToLip)
    {
        Sr  = *parameters.getVarPointer ("Sr");
        w = *parameters.getVarPointer ("w");
        yPrev = H0;
    } else {
        w = 0;
        Sr = 0;
        yPrev = 0;
    }
    
    if (Global::exciteFromStart)
    {
        yPrev = H0;
        pressureVal = Pm;
    } else {
        yPrev = 0; // energy might not like this..
        pressureVal = 0;
    }
    //    Pm = 0;
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    oOk = 1.0 / k;
    oOM = 1.0 / M;
    
    oO2k = 1.0 / (2.0 * k);
    omega0Sq = omega0 * omega0;
    kO2M = 0.5 * oOM * k;
    
    a1Coeff = 2.0 * oOk + omega0Sq * k + sig;
    a2 = Sr * oOM;
    
    psiPrev = 0;
    
    y = 0;
}

LipModel::~LipModel()
{
}

void LipModel::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

//    g.fillAll (Colours::yellow);   // clear the background;
    
}

void LipModel::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
void LipModel::setTubeParameters (double hIn, double rho, double c, double SBar0In, double SHalf0In)
{
    h = hIn;
    SBar0 = SBar0In;
    SHalf0 = SHalf0In;
    bCoeff = h * SBar0 / (rho * c * c * k);
    c1Coeff = w * sqrt (2.0 / rho);

}

void LipModel::calculateCollision()
{
    eta = b - y;
    etaPrev = b - yPrev;
    // calculate yNext without collision force
    g = 0;
    calculateDeltaP();
    calculate();
    etaNext = b - yNext;
    
    kappa = psiPrev < 0 ? -1 : 1;
    if (eta >= 0)
    {
        g = kappa * sqrt(Kcol * (alpha+1) / 2) * pow(Global::subplus (eta), (alpha - 1.0) / 2.0);
    } else {
        if(etaNext - etaPrev != 0)
        {
            g = -2.0 * psiPrev / (etaNext - etaPrev);
            divisBy0Flag = false;
        } else {
            if (!divisBy0Flag)
            {
                DBG("DIVISION BY 0");
                divisBy0Flag = true;
            }
        }
        
    }
}

void LipModel::calculateDeltaP()
{
    a1 = a1Coeff + g * g * kO2M;
    oOa1 = 1.0 / a1;
//    a2 = Sr / M;
    a3 = 2.0 * oOk * oOk * (y - yPrev) - omega0Sq * yPrev + g * oOM * psiPrev;
    b1 = SHalf0 * vNext0 + bCoeff * (Pm  - p0);
    b2 = bCoeff;
    c1 = c1Coeff * Global::subplus (y + H0);
    c2 = b2 + a2 * Sr * oOa1;
    c3 = b1 - a3 * Sr * oOa1;
    
    deltaPTerm = (-c1 + sqrt(c1 * c1 + 4.0 * c2 * abs (c3))) / (2.0 * c2);
    deltaP = Global::sgn(c3) * deltaPTerm * deltaPTerm;
    
}
void LipModel::calculate()
{
    //// Scheme ////
    gammaR = g * k * kO2M;
    oOAlpha = 1.0 / (2.0 + omega0Sq * k * k + sig * k + g * gammaR);
    beta = sig * k - 2.0 - omega0Sq * k * k + g * gammaR;
    xi = 2.0 * Sr * k * k * oOM;
    
    yNext = 4.0 * oOAlpha * y + beta * oOAlpha * yPrev + xi * oOAlpha * deltaP + 4.0 * gammaR * psiPrev * oOAlpha;
    
    //// Collision potential ////
    psi = psiPrev - 0.5 * g * (yNext - yPrev);
    
    
    //// Flow Velocities ////
    Ub = c1 * Global::sgn (deltaP) * sqrt (abs (deltaP));
    Ur = Sr * oO2k * (yNext - yPrev);

}

void LipModel::updateStates()
{
    yPrev = y;
    y = yNext;
    
    psiPrev = psi;
}

double LipModel::getLipEnergy()
{
    double lipEnergy = M * 0.5 * ((oOk * (y - yPrev)) * (oOk * (y - yPrev)) + omega0Sq * (y * y + yPrev * yPrev) * 0.5);
    
    if (lipEnergy1 < 0)
        lipEnergy1 = lipEnergy;
    
    return lipEnergy;
}

double LipModel::getCollisionEnergy()
{
    double colEnergy = psiPrev * psiPrev * 0.5;
    
    if (colEnergy1 < 0)
        colEnergy1 = colEnergy;
    
    return colEnergy;
}

double LipModel::getDampEnergy()
{
    double dampEnergy = M * sig * (1.0 * oO2k * (yNext - yPrev)) * (1.0 * oO2k * (yNext - yPrev)) + Ub * deltaP;
    double qH = k * dampEnergy + qHPrev;
    double qHPrevTmp = qHPrev;
    qHPrev = qH;
//    return qHPrevTmp == 0 ? qHPrev : qHPrevTmp;
    return qHPrevTmp;
//    return qH;
}

double LipModel::getPower()
{
    double power = -(Ub + Ur) * Pm;
    double pH = k * power + pHPrev;
    double pHPrevTmp = pHPrev;
    pHPrev = pH;
//    return pHPrevTmp == 0 ? pHPrev : pHPrevTmp;
    return pHPrevTmp;
//    return pH;
    
}

void LipModel::refreshInputParams()
{
    Pm = pressureVal;
    omega0 = lipFreqVal * 2.0 * double_Pi;
    omega0Sq = omega0 * omega0;
    a1Coeff = 2.0 * oOk + omega0Sq * k + sig;
}
