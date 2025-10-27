# BrickSpace Contributors

This document details the contributions to the BrickSpace project during Full Sail University's VR/AR Labs development period (June-October 2024).

## Commit Summary

| Contributor | Commits | Primary Focus Areas |
|-------------|---------|---------------------|
| **Jasen Blevins (Repository Owner)** | **15** | **Brick Spawning, Multi-User Sync, Early Planning, Asset Sourcing, Blueprint Architecture** |
| Dan Mapes | 151 | Network Architecture, World Calibration, Replication Systems |
| Delvin Porterfield (dport55) | 76 | Timer System, Wall Lever, Launch Button, UI Components |
| Ryan Suber | 76 | Hand Tracking, Particle Effects, Gesture Recognition, World Grabber |
| John Coss Vargas | 28 | Colocation, Network Testing, Integration |
| AnaDiaz34 | 20 | Assembly System, Materials, Scene Setup |
| PJ Glover | 18 | Polish, Testing, Integration |
| Nevin/Hemant | 27 | Eye Contact Detection, Face-to-Face System, Brick Transfer |
| Bryan Santiago Sinigaglia | 5 | Additional Features |

**Total Contributors**: 9 team members
**Total Commits**: 411 commits
**Development Period**: June - October 2024 (4 months)
**Initial Sprint**: 24-day intensive period (June-July 2024)
**Code Scale**: 41 C++ classes (~10,000+ lines of code)

> **Note**: This is Jasen Blevins' personal portfolio repository. While contributions are listed by commit count, all team members played crucial roles throughout the 4-month development journey. See [DEVELOPMENT_JOURNEY.md](DEVELOPMENT_JOURNEY.md) for the complete project evolution.

---

## Detailed Contributions

### Jasen Blevins (Repository Owner) - 15 commits
**Role**: Core Systems Developer - Brick Spawning, Multi-User Synchronization, Project Planning & Blueprint Architecture

**Major Contributions:**

**Early Development & Planning:**
- Assisted with early project development and technical roadmapping
- Contributed to initial architecture decisions and system design framework
- Helped establish development workflow and sprint planning
- Participated in feature prioritization and technical decision-making

**Asset Sourcing & Blueprint Architecture:**
- **Sourced and integrated 3D assets** from marketplace and community resources
- **Set up Unreal Blueprint architecture** for rapid prototyping and iteration
- Established Blueprint workflows for team collaboration
- Integrated assets with C++ systems for optimal performance
- Created Blueprint templates for brick types and interactive elements
- Configured material instances and data tables

**Core Systems Implementation:**
- **Brick spawning system architecture and implementation**
- **Network brick (NetBrick) replication system**
- **Brick eraser tool for removing bricks**
- **Manufacturer wall for brick production**
- Updated brick spawning logic for multiplayer
- Ground plate JSON integration
- Scene setup and configuration
- Vodget button integration with spawner

**Colocation Support:**
- Assisted John Coss Vargas with OculusXR colocation implementation
- Contributed to spatial anchor testing and debugging
- Supported multi-user spatial alignment workflows
- Helped integrate colocation features with brick spawning system

**Key Commits:**
- "Brick spawner fixed eraser added" (July 24)
- "net brick" (Aug 7)
- "updated brick spawning logic" (Aug 21)
- "manufacturer" (July 22)
- "vodgetbutton and spawner" (July 17)
- "bp groundplate json push" (Aug 28)
- "fixed manufacturing wall" (Aug 21)

**Technical Focus:**
- Real-time brick instantiation across multiple clients
- Server-authoritative spawning with client prediction
- Brick lifecycle management (spawn, update, delete)
- Integration with assembly system
- Performance optimization for multi-user brick manipulation
- Early architecture planning and technical roadmapping
- Asset pipeline management and Blueprint architecture
- C++ to Blueprint integration and workflow optimization
- Colocation system testing and integration support
- Material system setup and data table configuration

---

### Dan Mapes (151 commits)
**Role**: Lead Network Engineer & Calibration Systems

**Major Contributions:**
- Network replication architecture and RPC system design
- World grabber replication for shared physical space manipulation
- 2-point and 2-hand calibration systems (Calib2Pt, Calib2Hands)
- World scale synchronization across clients
- Hand mesh position and color replication
- Assembly actor network architecture
- Fix for client marker positioning in multiplayer
- Material change replication system
- Server-authoritative state management
- JSON-based calibration save/load system
- Performance optimization for network tick updates

**Key Commits:**
- "WorldGrabber replicated to sync units" (Sept 22)
- "2pt calib tested and working" (Oct 2)
- "World scale replicated and code cleanup" (Sept 21)
- "Setting calibrated scale properly through replication" (Oct 8)
- "TwoHand calibration tested and working" (Oct 21)

---

### Delvin Porterfield - dport55 (76 commits)
**Role**: UI Systems & Interactive Components

**Major Contributions:**
- Timer system implementation with network replication
- Wall lever mechanics and interaction
- Launch button system for rocket assembly
- Slider button component
- Reset button functionality
- Floor opening mechanics
- Wall mover system
- Pause button implementation
- C++ conversion of Blueprint systems

**Key Commits:**
- "Timer Finished" (Sept 27)
- "timer/lever Completed" (Oct 6)
- "Launch Button Connected" (Oct 14)
- "WallMover Completed" (Oct 23)
- "Lever SnapBack Completed" (Oct 23)

---

### Ryan Suber (76 commits combined - Ryan Suber + Ryan accounts)
**Role**: Hand Tracking & VR Interaction Specialist

**Major Contributions:**
- Hand tracking implementation and gesture recognition
- Pinch gesture system for brick manipulation
- World grabber gesture controls
- "Sticky hands" feature for camera tracking loss
- Hand tracking prediction for lost visibility
- Particle effects for rocket launch (Niagara)
- Fire effects integration from Fab marketplace
- Hand menu system implementation
- Grab mechanics refinement (index + thumb pinch)
- Blueprint reorganization and project structure
- Calib2Pt debugging and fixes

**Key Commits:**
- "WorldGrabber Works with Gestures!!!" (Oct 7)
- "PinchGesture added and works" (Oct 7)
- "Added a way to make a 'sticky hand' if the camera loses view" (Oct 7)
- "Prediction of placement on loss of hand visibility mostly works" (Oct 16)
- "Fire affects added from fab and adjustments made for our app" (Oct 16)
- "Fixed the rocket particle system" (Sept 25)

---

### John Coss Vargas (28 commits)
**Role**: Colocation & Network Integration

**Major Contributions:**
- OculusXR colocation implementation
- Meta Quest colocation testing and debugging
- Network session management
- Multi-player connection handling
- Integration testing for spatial anchors
- Colocation session setup workflows

---

### AnaDiaz34 (20 commits)
**Role**: Assembly System & Materials

**Major Contributions:**
- Assembly system implementation
- Material data table setup
- Reveal/solid material switching
- Assembly validation logic
- Layer progression system
- JSON assembly definitions
- Scene and level setup

---

### PJ Glover (18 commits)
**Role**: Polish & Integration

**Major Contributions:**
- Feature integration and testing
- UI polish and refinement
- Bug fixing and debugging
- Build testing and validation
- Cross-system integration

---

### Nevin & Hemant (27 commits combined)
**Role**: Social Interaction Features

**Major Contributions:**
- Eye contact detection system
- Face-to-face communication feedback
- Brick transfer between players
- Blue eye icon feedback system
- Timed face detection
- Dot product calculations for face orientation
- Menu system prototyping

**Key Commits:**
- "Brick Transfer Working version" (Oct 9)
- "instant Detection Works" (Oct 4)
- "blue eye icon for eyecontact feed back in progress" (Oct 7)

---

### Bryan Santiago Sinigaglia (5 commits)
**Role**: Supporting Development

**Major Contributions:**
- Additional feature implementation
- Testing and bug fixes
- Integration support

---

## Project Timeline Highlights

### Phase 1: Foundation (Early July)
- Initial scene setup and brick spawning system
- Basic network architecture
- Controller-based interaction

### Phase 2: Core Development (Mid July - August)
- Brick spawning system completed (Jasen)
- Network replication architecture established (Dan)
- Hand tracking integration (Ryan)
- Timer and UI systems (Delvin)

### Phase 3: Integration (Late August - September)
- World grabber and calibration systems (Dan, Ryan)
- Assembly system integration (Ana)
- Colocation implementation (John)
- Social features (Nevin, Hemant)

### Phase 4: Polish (October)
- Bug fixes across all systems
- Performance optimization
- Final feature integration
- Gesture refinement
- Particle effects and polish

---

## Technology Stack

**Core Technologies:**
- Unreal Engine 5.x (C++)
- Meta Quest SDK
- OculusXR Colocation API
- Niagara Particle System

**Key Systems:**
- Network Replication (Unreal Networking)
- Hand Tracking (Meta XR)
- Spatial Anchors (OculusXR)
- JSON Serialization

---

## Acknowledgments

This project was developed as part of Full Sail University's VR/AR Labs program. Special thanks to all contributors who completed this ambitious multiplayer VR system in just 24 days.

**Project Coordination**: Full Sail VR/AR Labs Faculty
**Development Period**: June-August 2024
**Final Repository**: https://github.com/JBlevins52736/brickspace

---

*This document was generated from git commit history. For technical architecture details, see [CLAUDE.md](../CLAUDE.md). For portfolio presentation, see [PORTFOLIO_SHOWCASE.md](PORTFOLIO_SHOWCASE.md).*
