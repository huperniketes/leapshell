#include "StdAfx.h"
#include "Interaction.h"
#include "Tile.h"
#include "Globals.h"

Interaction::Interaction() {
  m_panForce.Update(Vector3::Zero(), 0.0, 0.5f);
  m_lastViewUpdateTime = 0.0;
}

void Interaction::Update(const Leap::Frame& frame) {
  const Leap::HandList hands = frame.hands();
  const double deltaTime = m_prevFrame.isValid() ? TIME_STAMP_TICKS_TO_SECS * (frame.timestamp() - m_prevFrame.timestamp()) : 0.0f;
  const double timeSeconds = TIME_STAMP_TICKS_TO_SECS * frame.timestamp();

  // accumulate force from all hands
  Vector3 force(Vector3::Zero());
  for (int i=0; i<hands.count(); i++) {
    force += forceFromHand(hands[i]);
  }

  // scale the force by a constant to make sure we're not flying around the screen
  static const double FORCE_POSITION_SCALE = -0.1;
  force = FORCE_POSITION_SCALE * force;

  // make speeding up have less lag than slowing down
  static const float SPEED_UP_SMOOTH = 0.75f;
  static const float SLOW_DOWN_SMOOTH = 0.95f;
  const Vector3 prevForce = m_panForce.value;
  const float curSmooth = force.squaredNorm() > prevForce.squaredNorm() ? SPEED_UP_SMOOTH : SLOW_DOWN_SMOOTH;
  m_panForce.Update(force, timeSeconds, curSmooth);

  m_prevFrame = frame;
}

void Interaction::UpdateView(View &view) {
  const double deltaTime = Globals::curTimeSeconds - m_lastViewUpdateTime;
  assert(deltaTime > 0);

  // apply the force to the view camera
  view.ApplyVelocity(m_panForce.value, Globals::curTimeSeconds, deltaTime);

  const Leap::HandList hands = m_prevFrame.hands();
  for (int i=0; i<hands.count(); i++) {
    if (hands[i].isLeft()) {
      view.LeftHand().Update(hands[i], Globals::curTimeSeconds);
    } else if (hands[i].isRight()) {
      view.RightHand().Update(hands[i], Globals::curTimeSeconds);
    }
  }
  applyInfluenceToTiles(hands, view);

  m_lastViewUpdateTime = Globals::curTimeSeconds;
}

void Interaction::applyInfluenceToTiles(const Leap::HandList& hands, View& view) {
  static const float MAX_INFLUENCE_DISTANCE_SQ = 30 * 30;

  TileVector& tiles = view.Tiles();
  const Vector3& lookat = view.LookAt();
  Vector3 hitPoint;
  for (int i=0; i<hands.count(); i++) {
    // calculate projection point from camera to tiles
    if (!projectToPlane(hands[i], hitPoint)) {
      continue;
    }
    hitPoint += lookat;

    // calculate grab/pinch strength
    const float grabMultiplier = SmootherStep(std::max(hands[i].grabStrength(), hands[i].pinchStrength()));

    // find the closest tile to the projection point
    float closestDistSq = MAX_INFLUENCE_DISTANCE_SQ;
    Tile* closestTile = nullptr;
    for (TileVector::iterator it = tiles.begin(); it != tiles.end(); ++it) {
      Tile& tile = *it;
      const float distSq = (hitPoint - tile.m_position).squaredNorm();
      if (distSq < closestDistSq) {
        closestTile = &tile;
        closestDistSq = distSq;
      }
    }

    // increase activation for closest tile to hand
    if (closestTile) {
      // only allow activating the tile if it's already highlighted
      const float newActivation = closestTile->m_highlightSmoother.value > 0.95f ? grabMultiplier : 0.0f;
      closestTile->m_activationSmoother.Update(newActivation, Globals::curTimeSeconds, Tile::ACTIVATION_SMOOTH);
      closestTile->m_highlightSmoother.Update(1.0f, Globals::curTimeSeconds, Tile::ACTIVATION_SMOOTH);
    }
  }

  // decrease activation for all other tiles
  for (TileVector::iterator it = tiles.begin(); it != tiles.end(); ++it) {
    Tile& tile = *it;
    if (tile.m_activationSmoother.lastTimeSeconds != Globals::curTimeSeconds) {
      tile.m_activationSmoother.Update(0.0f, Globals::curTimeSeconds, Tile::ACTIVATION_SMOOTH);
      tile.m_highlightSmoother.Update(0.0f, Globals::curTimeSeconds, Tile::ACTIVATION_SMOOTH);
    }
  }
}

bool Interaction::projectToPlane(const Leap::Hand& hand, Vector3& hit) {
  static const Vector3 origin = View::CAM_DISTANCE_FROM_PLANE * Vector3::UnitZ();
  static const Vector3 center = Vector3::Zero();
  static const Vector3 normal = Vector3::UnitZ();

  const Vector3 position = hand.palmPosition().toVector3<Vector3>() + Globals::LEAP_OFFSET;
  const Vector3 direction = (position - origin).normalized();
  return RayPlaneIntersection(origin, direction, center, normal, hit);
}

Vector3 Interaction::forceFromHand(const Leap::Hand& hand) {
  Vector3 totalForce = Vector3::Zero();
  const Vector3 handDirection = hand.direction().toVector3<Vector3>();
  const Leap::FingerList fingers = hand.fingers();
  for (int i=0; i<fingers.count(); i++) {
    if (!fingers[i].isExtended()) {
      continue;
    }
    const Vector3 direction = fingers[i].direction().toVector3<Vector3>();
    const Vector3 velocity = fingers[i].tipVelocity().toVector3<Vector3>();
    const Vector3 normVelocity = velocity.normalized();
    const double dot = std::abs(normVelocity.dot(hand.palmNormal().toVector3<Vector3>()));
    const double match = direction.dot(handDirection);
    const float grabMultiplier = 1.0f - SmootherStep(std::max(hand.grabStrength(), hand.pinchStrength()));
    totalForce += grabMultiplier * grabMultiplier * grabMultiplier * dot * dot * match * velocity;
  }
  return totalForce;
}
