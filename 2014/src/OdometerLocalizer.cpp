#include "OdometerLocalizer.h"
#include "Maths.h"
#include "RobotConstants.h"

#include <string>
#include <sstream>

OdometerLocalizer::OdometerLocalizer() {
    x = RobotConstants::fieldWidth / 2.0f;
    y = RobotConstants::fieldHeight / 2.0f;
    orientation = 0.0f;
}

void OdometerLocalizer::move(float velocityX, float velocityY, float omega, float dt) {
    orientation = Math::floatModulus(orientation + omega * dt, Math::TWO_PI);
    x += ( velocityX * Math::cos(orientation) - velocityY * Math::sin(orientation)) * dt;
    y += ( velocityX * Math::sin(orientation) + velocityY * Math::cos(orientation)) * dt;
}

