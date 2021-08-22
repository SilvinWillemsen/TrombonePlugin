/*
  ==============================================================================

    Global.h
    Created: 5 Sep 2020 1:13:49pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once
//#def NOEDITOR

namespace Global {
    
    static double pressureMultiplier = 10.0;
    static double oOPressureMultiplier = 1.0 / pressureMultiplier;

    static bool setTubeTo1 = false;
    static bool connectedToLip = true;
    static bool fixedNonInterpolatedL = false;
    static bool exciteFromStart = false;
    static bool plotPressure = false;
    static double nonExtendedLipFreq = 520 * 0.5;
        
    static bool useMicInput = false;
    
    static double lambdaFact = 0.999;
    static double Nmaxdiff = 20.0;
    static bool useDispCorr = true;
    static bool correctV = false;
    
    static double LnonExtended = 2.593;
    static double Lextended = 3.653;
    
    static std::vector<double> linspace (double start, double finish, int N)
    {
        std::vector<double> res (N, 0);
        for (int i = 0; i < N; ++i)
        {
            res[i] = start + i * (finish - start) / static_cast<double> (N - 1);
        }
        return res;
    }
    static double linspace (double start, double finish, int N, int idx)
    {
        if (idx >= N)
        {
            std::cout << "Idx is outside of range" << std::endl;
            return -1;
            
        }
        return start + idx * (finish - start) / static_cast<double> (N - 1);
    }
    
    static inline double subplus (double val) { return (val + abs(val)) * 0.5; };
    
    static inline int sgn (double val) { return (0 < val) - (val < 0); };
    
    static double limit (double val, double min, double max)
    {
        if (val < min)
        {
            val = min;
            return val;
        }
        else if (val > max)
        {
            val = max;
            return val;
        }
        return val;
    }
    
    static double outputClamp (double val)
    {
        return limit (val, -1.0, 1.0);
    }
    
    
}
