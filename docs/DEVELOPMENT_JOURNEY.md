# BrickSpace Development Journey

This document chronicles the evolution of BrickSpace from initial commit to current state, highlighting key milestones, technical challenges, and the iterative development process.

## Timeline Overview

**Project Duration**: June 26 - October 23, 2025 (4 months of active development)
**Initial Sprint**: 24-day intensive period (June-July 2025)
**Total Commits**: 411 commits across 9 contributors
**Current State**: Fully functional multiplayer VR building experience

---

## Phase 1: Foundation & Core Framework (June 26 - July 10)

### Week 1: Project Initialization
**June 26-30**

The project began with Dan Mapes' initial commit establishing the repository structure. The first week focused on foundational systems:

**Key Achievements:**
- Initial Unreal Engine project structure established
- **Vodget framework implemented** - Custom VR interaction system that would become the backbone of all user interactions
- BrickWorld pawn created with basic VR setup
- Android configuration for Quest deployment
- Basic grabbing mechanics prototyped

**Technical Decisions:**
- Chose to build custom interaction framework (Vodget) rather than relying solely on Unreal's built-in VR interaction
- Early decision to support Android/Quest platform from day one
- Established Git workflow for 9-person team

### Week 2: Colocation & Snapping
**July 1-10**

This week marked the beginning of the project's two most challenging technical systems: multiplayer colocation and brick snapping physics.

**Key Achievements:**
- **Colocation setup** initiated by Ana and PJ - enabling multiple users to share physical space
- **Brick class created** - foundation for all building blocks
- **Snapping algorithm prototyped** - complex physics for LEGO-style connections
- SnapBase system for studs and tubes designed
- Passthrough configuration for mixed reality
- Menu navigation with joystick input added

**Technical Challenges:**
- Snapping algorithm complexity - needed to handle 6DOF positioning with sub-centimeter accuracy
- Colocation errors requiring multiple team members to troubleshoot
- Balancing passthrough performance with visual quality

**Key Commit**: *"Snapping without grouping working"* - Dan Mapes (July 8)

---

## Phase 2: Building the Building Experience (July 11-22)

### Core Mechanics Implementation

This phase transformed BrickSpace from a tech demo into a functional building experience.

**Key Achievements:**
- **Snapping system fully functional** with stud/tube connections
- Multiple brick types added to expand building possibilities
- Depth API occlusion for realistic hand/brick interaction
- Material system with filters for vodgets and selectors
- **Assembly system initiated** - structured building experiences
- Groundplate established as building foundation
- Reveal and paint wands for brick manipulation

**Jasen's Contributions (July 17-22):**
- **Vodget button and spawner integration**
- **Manufacturer wall implementation** - automated brick generation
- Scene setup and configuration

**Technical Milestones:**
- Diagonal snap rejection prevents impossible brick placements
- Overlap detection on grab/release for smooth interaction
- Blueprint workflows established for rapid iteration

**Key Commit**: *"Assembly started"* - Dan Mapes (July 20)

---

## Phase 3: Assembly System & JSON Persistence (July 23 - August 5)

### Data-Driven Building

This phase introduced structured building challenges and data persistence, transforming BrickSpace into a goal-oriented experience.

**Key Achievements:**
- **JSON save/load system** for assembly definitions
- Rocket.json created as first structured building challenge
- **Assembly layers reveal as solved** - guided building progression
- **Brick spawner and eraser tools** (Jasen Blevins)
- Rocket takeoff animation on completion
- Niagara particle system integration for visual effects
- Translucent materials replace wireframe for reveal mode

**Jasen's Major Work (July 24):**
- *"Brick spawner fixed eraser added"* - complete brick lifecycle management
- Ground plate JSON integration

**Development Philosophy Shift:**
Instead of freeform building, the team pivoted toward guided assembly experiences that teach collaboration through structured challenges.

**Key Commits:**
- *"Creating JSON of assembly implemented"* - Dan Mapes (July 24)
- *"Assembly Saves/Reloads on headset build"* - Dan Mapes (July 27)
- *"Rocket takes off when finished"* - Dan Mapes (July 30)

---

## Phase 4: Network Architecture Overhaul (August 6 - September 15)

### Multiplayer Foundation

The longest and most technically challenging phase focused on making everything work seamlessly across multiple clients.

**Key Achievements:**
- **BrickActor and PainterActor** base classes for replication
- **BrickSpacePlayerState** for per-player replicated properties
- Hand material color changes synchronized across network
- Paintbrush position/rotation replication
- Movement to **Meta's fork of Unreal Engine 5.5**
- Plugin architecture for replication testing
- Server-authoritative state management

**Jasen's Networking Work (August 7-21):**
- *"net brick"* - Network replication for brick spawning
- *"updated brick spawning logic"* - multiplayer synchronization
- Smart anchor implementation

**Technical Challenges:**
This phase involved extensive debugging with commits like:
- "Clone test", "test", "Test" - iterative problem solving
- Multiple attempts at hand replication across clients
- Server vs. client authority issues
- Property replication timing bugs

**Team Collaboration:**
- Dan Mapes led network architecture design
- John Coss Vargas focused on hand networking
- Ryan worked on UI updates based on player position
- Everyone contributed to debugging network edge cases

**Key Insight**: Network replication in VR requires sub-frame timing precision. The team learned to use OnRep_ callbacks extensively and implemented prediction for smooth local interaction.

**Key Commit**: *"Replicate using testing"* - Dan Mapes (August 27)

---

## Phase 5: Hand Tracking & Calibration Revolution (September 16 - October 8)

### Controller-Free Interaction

This phase marked a major UX shift from controller-based to hand tracking-based interaction.

**Key Achievements:**
- **Full hand tracking implementation** (Ryan Suber)
- **Gesture recognition system** - pinch, grab, world grab
- **World grabber with gestures** - manipulate entire scene with hands
- **Calibration systems**:
  - Calib2Pt - Two-point world calibration
  - Calib2Hands - Two-handed calibration
  - JSON-based calibration save/load
- **"Sticky hands" feature** - maintains object position when hand tracking is lost
- Prediction system for grab placement during tracking loss

**Technical Innovation - Sticky Hands:**
When Quest cameras lose sight of hands, the system predicts hand position to prevent frustrating object drops. This showed the team's commitment to polishing the user experience.

**Ryan's Major Contributions:**
- *"WorldGrabber Works with Gestures!!!"* - Oct 7 (breakthrough moment)
- *"PinchGesture added and works"* - Oct 7
- *"Added a way to make a 'sticky hand'"* - Oct 7

**Dan's Calibration Work:**
- *"WorldGrabber replicated to sync units"* - Sept 22 (critical for multiplayer)
- *"2pt calib tested and working"* - Oct 2
- *"TwoHand calibration tested and working"* - Oct 21

**Development Philosophy:**
The team prioritized natural interaction over technical convenience. Hand tracking was harder to implement than controller input, but provided better accessibility and immersion.

---

## Phase 6: Polish & Feature Complete (October 9-23)

### The Final Sprint

The final two weeks focused on UI polish, additional features, and system integration.

**Key Achievements:**
- **Timer system** (Delvin Porterfield) - tracking build time
- **Wall lever mechanics** - interactive environment elements
- **Launch button** - tactile rocket launch
- **Hand menu system** - palm-attached UI
- **Eye contact detection** (Hemant/Nevin) - social presence features
- **Brick transfer between players** (Nevin)
- **Fire particle effects** from Fab marketplace
- Slider buttons and interactive UI components
- Floor opening mechanics
- Final bug fixes and code cleanup

**Delvin's UI Work:**
Transformed from Blueprint prototypes to C++ implementations for better performance and replication.

**Key Commits:**
- *"timer/lever Completed"* - dport55 (Oct 6)
- *"Launch Button Connected"* - dport55 (Oct 14)
- *"Brick Transfer Working version"* - Nevin (Oct 9)
- *"Fire affects added from fab"* - Ryan Suber (Oct 16)

---

## Current State (October 23, 2025)

### Feature-Complete Multiplayer VR Experience

BrickSpace has evolved into a fully-functional collaborative VR building platform with:

### Core Features
- ✅ **4-player multiplayer** with OculusXR colocation
- ✅ **Hand tracking** with gesture recognition
- ✅ **Snap-to-grid physics** for LEGO-style building
- ✅ **Assembly system** with JSON-defined building challenges
- ✅ **World calibration** (2-point and 2-hand methods)
- ✅ **Network replication** for all gameplay elements
- ✅ **Brick spawning/erasing** tools
- ✅ **Material/color** changing system

### Polish Features
- ✅ Timer tracking build time
- ✅ Interactive wall levers and buttons
- ✅ Rocket launch animation with particle effects
- ✅ Hand menu system
- ✅ Eye contact detection
- ✅ Brick transfer between players
- ✅ "Sticky hands" for tracking loss
- ✅ Multiple maps (Main, Lobby, Test)

### Technical Achievement
- **41 C++ classes** (41 .h files, 41 .cpp files)
- **Server-authoritative networking** with client prediction
- **Custom VR interaction framework** (Vodget/Selector)
- **JSON-based content pipeline** for assemblies
- **Comprehensive replication system** for all game objects

---

## Key Technical Challenges Overcome

### 1. Brick Snapping Physics
**Challenge**: Achieving LEGO-like snap feel with sub-centimeter precision while maintaining 72fps on Quest 2.

**Solution**: SnapBase system with studs and tubes, overlap detection, and diagonal snap rejection.

### 2. Multiplayer Synchronization
**Challenge**: Synchronizing brick positions across 4 players in real-time with sub-50ms latency.

**Solution**: Server-authoritative architecture with client prediction, OnRep_ callbacks, and optimized tick updates.

### 3. Spatial Colocation
**Challenge**: Ensuring all players see virtual objects in the same real-world positions.

**Solution**: OculusXR spatial anchors with custom calibration workflows (2-point and 2-hand calibration).

### 4. Hand Tracking Reliability
**Challenge**: Hand tracking breaks when cameras lose sight of hands.

**Solution**: "Sticky hands" prediction system maintains object position during tracking loss.

### 5. Network Authority Debugging
**Challenge**: Complex ownership and authority issues with replicated actors.

**Solution**: Extensive logging, iterative testing, and refactoring to consolidate RPCs in BrickSpacePawn.

---

## Development Methodology

### Iterative Problem-Solving
The commit history shows heavy iteration: "test", "Clone test", "Client hand rep take 1-4". This reflects a pragmatic approach - try, fail, learn, repeat.

### Parallel Development
Multiple developers worked on separate systems simultaneously:
- Dan: Networking core
- Ryan: Hand tracking
- Delvin: UI systems
- Jasen: Brick spawning
- John: Colocation
- Ana: Assembly system

### Rapid Prototyping
Blueprint workflows enabled quick iteration before committing to C++ implementations. Many features started in Blueprint, then moved to C++ for performance.

### Continuous Integration
Regular merges from feature branches kept the main branch stable while allowing experimental development.

---

## Lessons from the Journey

1. **Custom Frameworks Pay Off**: The Vodget system took time upfront but enabled rapid feature development later

2. **Network Complexity Compounds**: What works locally often breaks in multiplayer - plan for this early

3. **User Experience Matters**: "Sticky hands" didn't add gameplay but dramatically improved feel

4. **Calibration is Critical**: Multiplayer VR requires precise spatial alignment - invested heavily in this

5. **Iterative Development Works**: The team embraced failure as learning, shown in numerous test commits

---

## What's Next?

While the project achieved feature-complete status by October 23, potential future enhancements include:

- Additional assembly models beyond the rocket
- More interactive environment elements
- Enhanced social features (voice chat, gestures)
- Performance optimization for Quest 3
- Additional game modes (competitive building, timed challenges)
- Blueprint-to-C++ conversion for remaining systems

---

## Acknowledgments

This 4-month journey from concept to completion represents the collaborative effort of 9 developers working through complex technical challenges in VR, networking, and user experience design. The project stands as a testament to what can be achieved when a team commits to iterative development and mutual support.

**Development Period**: June 26 - October 23, 2025
**Total Development Time**: 4 months
**Initial Intensive Sprint**: 24 days (June-July 2025)
**Total Commits**: 411
**Team Size**: 9 developers
**Lines of C++ Code**: ~10,000+ (41 header/source file pairs)
