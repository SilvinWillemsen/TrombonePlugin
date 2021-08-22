/*
  ==============================================================================

    Tube.h
    Created: 5 Sep 2020 1:11:57pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Global.h"


//==============================================================================
/*
*/
class Tube  : public juce::Component
{
public:
    Tube (NamedValueSet& parameters, double k, std::vector<std::vector<double>>& geometry);
    ~Tube() override;

    Path drawGeometry (Graphics& g, int topOrBottom);
    Path visualiseState (Graphics& g, double visualScaling, bool pressure);
    void paint (juce::Graphics&) override;
    void resized() override;

    void calculateThermodynamicConstants();
    void calculateGeometry();
    void calculateRadii();
    void calculateVelocity();
    void calculatePressure();
    void calculateRadiation();

    void setFlowVelocities (double UbIn, double UrIn)
    {
        Ub = UbIn;
        Ur = UrIn;
    };
    float getOutput() {
//        if (setting)
//        {
//        outputVal = 0;
//        for (int i = 0; i < floor(numOutputPoints); ++i)
//            outputVal += wp[1][Mw-i];
//
//        if (numOutputPoints - floor(numOutputPoints) != 0)
//            outputVal +=wp[1][Mw-static_cast<int>(ceil(numOutputPoints))] * (numOutputPoints - floor(numOutputPoints));
//        return outputVal / numOutputPoints;
//        } else {
        return wp[1][Mw];
//        }
//    return getP (1, maxM + Mw + 1);
    };
    
    void updateStates();
    
    double getP (int n, int l) {
        if (l <= maxM)
            return up[n][l];
        else
            return wp[n][l-maxM-1];
    };
    
    double getV (int n, int l) {
        if (l <= maxM-1)
            return uv[n][l];
        else
            return wv[n][l-maxM];
    };
        
    int getNint() { return Nint; };
    double getN() { return N; };

    int getM() { return M; };
    int getMw() { return Mw; };
    int getMaxM() { return maxM; };
    int getMaxMw() { return maxMw; };
    int getMaxN() { return Nextended; }
    
    double getAlf() { return alf; };

    double getH() { return h; };
    double getRho() { return rho; };
    double getC() { return c; };

    double getS (int idx) {
        if (idx >= S.size())
            return 0;
        else
            return S[idx];
    };
    double getSHalf (int idx) {
        if (idx >= SHalf.size())
            return 0;
        else
            return SHalf[idx];
    };
    double getSBar (int idx) {
        if (idx >= SBar.size())
            return 0;
        else
            return SBar[idx];
        
    };
    
    double getKinEnergy();
    double getPotEnergy();
    double getRadEnergy();
    double getRadDampEnergy();
    
    double getKinEnergy1() { return kinEnergy1; };
    double getPotEnergy1() { return potEnergy1; };
    double getRadEnergy1() { return radEnergy1; };

    double getLVal() { return L; };
    void setExtVals (double LVal) { LtoGoTo = Global::limit (LVal, Global::LnonExtended, Global::Lextended); };
    void updateL();
    
    void lowPassConnection();
    void dispCorr();
    void addRemovePoint();
    void createCustomIp();
        
    void changeSetting () { setting = !setting; };
    void changeSetting (bool b) { setting = b; };
    
    void calculateVRel(); // experimental bowing
    
    double getBellRad() { return radii[radii.size()-1]; };
    
private:
    double k, h, c, lambda, rho, L, T;
    int Nint, NintPrev, M, Mw, maxM, maxMw;
    int NnonExtended, Nextended;
    double N;
    double alf;
    
    std::vector<double> quadIp;
    std::vector<double> customIp;

    // Radiation vars
    double R1, rL, Lr, R2, Cr, z1, z2, z3, z4;
    double p1Next, p1, v1Next, v1;
    double oORadTerm;
    
    double Ub, Ur;
    
    double lambdaOverRhoC;
    std::vector<std::vector<double>> uvVecs;
    std::vector<std::vector<double>> upVecs;
    
    std::vector<std::vector<double>> wvVecs;
    std::vector<std::vector<double>> wpVecs;
    
    double uvMphPrev = 0;
    double wvmhPrev = 0;
    
    std::vector<std::vector<double>> geometry;
    double b, x0, flare;
    
    double upMp1, wpm1, uvNextMph, uvMph, wvNextmh, wvmh;
    double uvMmhPrev, wvhPrev;
    
    // pointers to states
    std::vector<double*> uv;
    std::vector<double*> up;
    
    std::vector<double*> wv;
    std::vector<double*> wp;

    // states
    std::vector<std::vector<double>> uVecs;
    std::vector<std::vector<double>> wVecs;

    // tube geometry
    std::vector<double> S, SHalf, SBar, oOSBar, radii;
    
    double* uvTmp = nullptr;
    double* upTmp = nullptr;
    double* wvTmp = nullptr;
    double* wpTmp = nullptr;

    double kinEnergy1 = -1;
    double potEnergy1 = -1;
    double radEnergy1 = -1;

    bool raisedCos = false;
    bool init = true;
    
    double qHRadPrev = 0;
    
    double LtoGoTo, Lprev;
    
    double lpExponent = 10;
    
    bool setting = true;
    
    double outerSlideLoc1 = 0;
    double outerSlideLoc2 = 0;
    
    double vrel, vrelPrev, Fb, Vb, BM, beta, a, eps, tol;
    double bowExcitation = 0;
    
    double outputVal = 0;
    double numOutputPoints = 10;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Tube)
};
