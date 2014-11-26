#include "ParticleFilterLocalizer.h"
#include "Maths.h"
#include "RobotConstants.h"

#include <iostream>
#include <string>
#include <sstream>

ParticleFilterLocalizer::ParticleFilterLocalizer(int particleCount, float forwardNoise, float turnNoise, float distanceSenseNoise, float angleSenseNoise) : particleCount(particleCount), forwardNoise(forwardNoise), turnNoise(turnNoise), distanceSenseNoise(distanceSenseNoise), angleSenseNoise(angleSenseNoise) {
    for (int i = 0; i < particleCount; i++) {
        particles.push_back(new Particle(
                Math::randomFloat(0.0f, RobotConstants::fieldWidth),
                Math::randomFloat(0.0f, RobotConstants::fieldHeight),
                Math::randomFloat(0.0f, Math::TWO_PI),
                1.0f
                ));
    }

    json = "null";
}

ParticleFilterLocalizer::~ParticleFilterLocalizer() {
    for (ParticleList::const_iterator it = particles.begin(); it != particles.end(); it++) {
        delete *it;
    }

    particles.clear();

    for (LandmarkMap::const_iterator it = landmarks.begin(); it != landmarks.end(); it++) {
        delete it->second;
    }

    landmarks.clear();
}

void ParticleFilterLocalizer::addLandmark(Landmark* landmark) {
    landmarks[landmark->name] = landmark;
}

void ParticleFilterLocalizer::addLandmark(std::string name, float x, float y) {
    addLandmark(new Landmark(name, x, y));
}

void ParticleFilterLocalizer::setPosition(float x, float y, float orientation) {
    for (unsigned int i = 0; i < particles.size(); i++) {
        particles[i]->orientation = orientation;
        particles[i]->x = x;
        particles[i]->y = y;
        particles[i]->probability = 1;
    }
}

void ParticleFilterLocalizer::move(float velocityX, float velocityY, float omega, float dt, bool exact) {
    float particleVelocityX, particleVelocityY, particleOrientationNoise;

    for (unsigned int i = 0; i < particles.size(); i++) {
        if (exact) {
            particleVelocityX = velocityX;
            particleVelocityY = velocityY;
            particleOrientationNoise = 0;
        } else {
            // TODO Add noise in FORWARD direction
            /*particleVelocityX = velocityX + velocityX * Math::randomGaussian(forwardNoise);
            particleVelocityY = velocityY + velocityY * Math::randomGaussian(forwardNoise);*/
            particleVelocityX = velocityX + Math::randomGaussian(forwardNoise);
            particleVelocityY = velocityY + Math::randomGaussian(forwardNoise);
            particleOrientationNoise = Math::randomGaussian(turnNoise) * dt;
        }

        particles[i]->orientation = particles[i]->orientation + omega * dt + particleOrientationNoise;
        particles[i]->x += (particleVelocityX * Math::cos(particles[i]->orientation) - particleVelocityY * Math::sin(particles[i]->orientation)) * dt;
        particles[i]->y += (particleVelocityX * Math::sin(particles[i]->orientation) + particleVelocityY * Math::cos(particles[i]->orientation)) * dt;
    }
}

void ParticleFilterLocalizer::update(const Measurements& measurements) {
    Particle* particle;
    float maxProbability = -1;

    for (unsigned int i = 0; i < particles.size(); i++) {
        particle = particles[i];

        //Util::confineField(particle->x, particle->y);

        particle->probability = getMeasurementProbability(particle, measurements);

        if (maxProbability == -1 || particle->probability > maxProbability) {
            maxProbability = particle->probability;
        }
    }

    if (maxProbability == 0) {
        return;
    }

    for (unsigned int i = 0; i < particles.size(); i++) {
        particles[i]->probability /= maxProbability;
    }

    resample();
}

float ParticleFilterLocalizer::getMeasurementProbability(Particle* particle, const Measurements& measurements) {
    float probability = 1.0f;
    float expectedDistance;
    float expectedAngle;
    float measuredDistance;
    float measuredAngle;
    std::string landmarkName;
    Landmark* landmark;
    LandmarkMap::iterator landmarkSearch;

    for (Measurements::const_iterator it = measurements.begin(); it != measurements.end(); it++) {
        landmarkName = it->first;
        Measurement measurement = it->second;
        measuredDistance = measurement.distance;
        measuredAngle = measurement.angle;
        landmarkSearch = landmarks.find(landmarkName);

        if (landmarkSearch == landmarks.end()) {
            std::cout << "- Didnt find landmark '" << landmarkName << "', this should not happen" << std::endl;

            continue;
        }

        landmark = landmarkSearch->second;
        expectedDistance = Math::distanceBetween(particle->x, particle->y, landmark->x, landmark->y);
        expectedAngle = Math::getAngleBetween(Math::Position(landmark->x, landmark->y), Math::Position(particle->x, particle->y), particle->orientation);

        probability *= Math::getGaussian(expectedDistance, distanceSenseNoise, measuredDistance) * 0.75f
                + Math::getGaussian(expectedAngle, angleSenseNoise, measuredAngle) * 0.25f;
    }

    return probability;
}

void ParticleFilterLocalizer::resample() {
    ParticleList resampledParticles;
    int particleCount = (int) particles.size();
    int index = Math::randomInt(0, particleCount - 1);
    float beta = 0.0f;
    float maxProbability = 1.0f;

    for (int i = 0; i < particleCount; i++) {
        beta += Math::randomFloat() * 2.0f * maxProbability;

        while (beta > particles[index]->probability) {
            beta -= particles[index]->probability;
            index = (index + 1) % particleCount;
        }

        resampledParticles.push_back(new Particle(
                particles[index]->x,
                particles[index]->y,
                particles[index]->orientation,
                particles[index]->probability
                ));
    }

    for (ParticleList::const_iterator it = particles.begin(); it != particles.end(); it++) {
        delete *it;
    }

    particles.clear();

    particles = resampledParticles;
}

Math::Position ParticleFilterLocalizer::getPosition() {
    float xSum = 0.0f;
    float ySum = 0.0f;
    float orientationSum = 0.0f;
    unsigned int particleCount = particles.size();
    Particle* particle;

    if (particleCount == 0) {
        std::cout << "@ NO PARTICLES FOR POSITION" << std::endl;

        return Math::Position();
    }

    for (unsigned int i = 0; i < particleCount; i++) {
        particle = particles[i];

        xSum += particle->x;
        ySum += particle->y;
        orientationSum += particle->orientation;
    }

    x = xSum / (float) particleCount;
    y = ySum / (float) particleCount;
    orientation = Math::floatModulus(orientationSum / (float) particleCount, Math::TWO_PI);

    //Util::confineField(x, y);

    // generate the state JSON
    std::stringstream stream;

    stream << "{";
    stream << "\"x\": " << x << ",";
    stream << "\"y\": " << y << ",";
    stream << "\"orientation\": " << orientation << ",";
    stream << "\"particles\": [";

    for (unsigned int i = 0; i < particleCount / 10; i++) {
        particle = particles[i];

        if (i > 0) {
            stream << ",";
        }

        stream << "[" << particle->x << ", " << particle->y << "]";
    }

    stream << "]}";

    json = stream.str();

    return Math::Position(
            x,
            y,
            orientation
            );
}
