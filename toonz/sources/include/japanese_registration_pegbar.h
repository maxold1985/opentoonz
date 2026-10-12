#pragma once

// Japanese animation paper registration geometry.
// Coordinate system: scene/viewer units, Y positive upward.
// This component is intentionally independent of Qt and OpenGL.
#include <array>
#include <cmath>

namespace JapaneseRegistration {

struct Point {
    double x = 0.0;
    double y = 0.0;
};

struct Pose {
    Point origin;
    double rotationRadians = 0.0;
};

enum class HoleType { Round, HorizontalSlot };

struct Hole {
    Point center;
    HoleType type;
    double width;
    double height;
};

class PegBar {
public:
    // Dimensions in millimetres, configurable for a studio's tap standard.
    double outerSpacingMm = 100.0;
    double roundDiameterMm = 5.0;
    double slotWidthMm = 8.0;
    double slotHeightMm = 5.0;
    Pose pose;

    Point transform(Point p) const {
        const double c = std::cos(pose.rotationRadians);
        const double s = std::sin(pose.rotationRadians);
        return {pose.origin.x + c * p.x - s * p.y,
                pose.origin.y + s * p.x + c * p.y};
    }

    Point inverseTransform(Point p) const {
        const double x = p.x - pose.origin.x;
        const double y = p.y - pose.origin.y;
        const double c = std::cos(pose.rotationRadians);
        const double s = std::sin(pose.rotationRadians);
        return {c * x + s * y, -s * x + c * y};
    }

    std::array<Hole, 3> holes(double unitsPerMm) const {
        const double half = outerSpacingMm * unitsPerMm * 0.5;
        return {{{transform({-half, 0.0}), HoleType::HorizontalSlot,
                  slotWidthMm * unitsPerMm, slotHeightMm * unitsPerMm},
                 {transform({0.0, 0.0}), HoleType::Round,
                  roundDiameterMm * unitsPerMm, roundDiameterMm * unitsPerMm},
                 {transform({half, 0.0}), HoleType::HorizontalSlot,
                  slotWidthMm * unitsPerMm, slotHeightMm * unitsPerMm}}};
    }

    // Converts a drawing point between two pegbar poses.
    // Used for temporary shift/trace registration without editing source art.
    static Point reRegister(Point drawingPoint, const PegBar &from,
                            const PegBar &to) {
        return to.transform(from.inverseTransform(drawingPoint));
    }
};

}  // namespace JapaneseRegistration
