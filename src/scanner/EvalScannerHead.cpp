#include <scanner/EvalScannerHead.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <MathConstants.h>
#include <maths/TrigoTricks.h>

// ***  M E T H O D S  *** //
// *********************** //
void EvalScannerHead::doSimStep(double pulseFreq_Hz){
    if (cfg_setting_rotatePerSec_rad != 0) {
        setCurrentRotateAngle_rad(
            state_currentExactRotateAngle_rad +
            cfg_setting_rotatePerSec_rad / pulseFreq_Hz
        );
        // WARNING! Error expression below requires deflectorAngle does not
        // lead to sin(deflectorAngle) approx 0
        double const theta = TrigoTricks::clipZeroSinRadians(
            *deflectorAngle, // Vertical angle (rad)
            zeroSinThreshold_rad
        );
        state_currentRotateAngle_rad = state_currentExactRotateAngle_rad -
            horizAngErrExpr->eval(theta);
    }
}

bool EvalScannerHead::rotateCompleted(){
    bool result = false;

    if (cfg_setting_rotatePerSec_rad < 0) {
        result = state_currentExactRotateAngle_rad<=cfg_setting_rotateStop_rad;
    }
    else {
        result = state_currentExactRotateAngle_rad>=cfg_setting_rotateStop_rad;
    }

    return result;
}

// ***  GETTERS and SETTERS  *** //
// ***************************** //
void EvalScannerHead::setCurrentRotateAngle_rad(double angle_rad){
    if(angle_rad == state_currentExactRotateAngle_rad) return;
    state_currentExactRotateAngle_rad = angle_rad;
    cached_mountRelativeAttitude = Rotation(
        cfg_device_rotateAxis,
        fmod(state_currentRotateAngle_rad, PI_2)
    );
}
