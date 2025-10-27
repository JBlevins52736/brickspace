# BrickSpace

> **Portfolio Repository**: This is Jasen Blevins' personal portfolio version of the BrickSpace project, originally developed as part of Full Sail University's VR/AR Labs program.

A collaborative AR/VR building environment for Meta Quest that brings teams together to build and create in shared physical space. Designed to inspire teamwork and develop communication skills through intuitive, hands-on interaction with virtual LEGO-style bricks.

## Project Background

BrickSpace evolved from research into assistive technology for individuals with autism, focusing on helping people develop coping skills for transitioning between challenging environments. Through exploring how different individuals handle stress and process spatial information, the project transformed into something broader: a collaborative building platform where teams of up to four people work together in an immersive environment that feels safe, structured, and engaging.

Developed as part of Full Sail University's VR/AR Labs program starting with an intensive 24-day sprint (June-July 2024), then evolved over 4 months of active development through October 2024. BrickSpace demonstrates how thoughtful AR/VR design can create experiences that enhance collaboration for everyone.

The project evolved through 6 major development phases:
1. **Foundation & Framework** - Core VR interaction systems (Vodget framework)
2. **Building Mechanics** - Snap-to-grid physics and brick system
3. **Assembly System** - JSON-driven guided building experiences
4. **Network Architecture** - Server-authoritative multiplayer infrastructure
5. **Hand Tracking** - Controller-free interaction with gesture recognition
6. **Polish & Features** - UI systems, particle effects, and social features

The project's technical innovations include real-time brick synchronization across 4 players, custom spatial calibration systems, and "sticky hands" prediction for robust hand tracking.

## Key Features

### Collaborative Building
- **Multi-User Support**: Up to 4 players can build together in the same physical space
- **Real-Space Synchronization**: Brick movements are synchronized across all users with seamless real-time updates
- **Play Space Alignment**: Automatic spatial calibration ensures all players see and interact with the same virtual space
- **OculusXR Colocation**: Leverages Meta's spatial anchoring for precise multi-user alignment

### Intuitive Interaction
- **Hand Tracking**: Natural brick manipulation using Meta Quest's native hand tracking
- **Snap-to-Grid Physics**: Authentic LEGO-style stud and tube snapping system for satisfying building mechanics
- **Clear Visual Feedback**: Intuitive visual cues guide building and collaboration
- **Material System**: Color selection and material switching for creative expression

### Structured Building Experience
- **Layer-by-Layer Assembly**: Guided building mode with predefined models (rockets, structures, etc.)
- **Assembly Validation**: Real-time feedback ensures correct brick placement and progression
- **Collaborative Problem-Solving**: Designed to encourage teamwork and communication

### Technical Foundation
- **Server-Authoritative Architecture**: Robust network replication ensures consistent state across all clients
- **Performance Optimized**: Efficient spawning system handles complex multi-user interactions maintaining 72fps on Quest 2
- **Extensible Design**: JSON-based assembly definitions for easy content creation
- **Custom VR Framework**: Vodget/Selector system provides foundation for all interactions
- **Advanced Calibration**: Multiple calibration methods (2-point, 2-hand) for precise spatial alignment
- **Robust Hand Tracking**: "Sticky hands" prediction system handles tracking loss gracefully

### Current State (October 2024)

BrickSpace is feature-complete with:
- ✅ Full multiplayer support (up to 4 players)
- ✅ Controller-free hand tracking with gesture recognition
- ✅ Complete assembly system with rocket launch sequence
- ✅ Timer and interactive UI elements
- ✅ Particle effects and visual polish
- ✅ Eye contact detection and social features
- ✅ Multiple maps (Main, Lobby, Test)
- ✅ Comprehensive network replication (41 C++ classes)

## Requirements

- **Unreal Engine 5.x** (custom engine build - see `.uproject` for association)
- **Meta Quest** device with hand tracking support
- **Windows 10/11** for development
- **Visual Studio 2022** (for C++ compilation)

## Getting Started

### Opening the Project

1. Clone this repository
2. Double-click `BrickSpace.uproject` to open in Unreal Engine
3. Wait for initial compilation and shader compilation to complete

### Building for Quest

1. Ensure Android development tools are installed in Unreal Engine
2. File → Package Project → Android → Android (ASTC)
3. Select output directory and wait for packaging to complete
4. Deploy to Quest via SideQuest or adb

## Portfolio & Media

### Documentation
- **[Development Journey](docs/DEVELOPMENT_JOURNEY.md)** - Complete 4-month evolution from concept to completion
- **[Technical Documentation](docs/)** - Detailed architecture and API documentation
- **[Contributors](docs/CONTRIBUTORS.md)** - Detailed breakdown of team contributions
- **[Media Assets](media/)** - Screenshots, diagrams, and video demonstrations
- **[External Media](media/EXTERNAL_MEDIA.md)** - Links to videos and high-resolution materials

### Key Highlights

**Technical Achievements:**
- Implemented real-time multi-user synchronization for up to 4 players
- Developed custom Vodget/Selector VR interaction framework
- Built server-authoritative network architecture from scratch
- Created spatial colocation system using OculusXR APIs

**Development Timeline:**
- **Duration**: 4 months (June - October 2024)
- **Initial Sprint**: 24-day intensive period (June-July 2024)
- **Role**: Core Systems Developer (Brick Spawning, Multi-User Synchronization, Early Development Planning, Asset Sourcing, Blueprint Architecture, Colocation Support)
- **Team**: Full Sail University VR/AR Labs (9-person collaborative team)
- **Technologies**: Unreal Engine 5.5 (C++), Meta Quest SDK, OculusXR Colocation, Niagara VFX
- **Scale**: 411 commits, 41 C++ classes, ~10,000+ lines of code

**Impact:**
- Demonstrates practical application of assistive technology research
- Showcases human-centered design in VR development
- Highlights rapid prototyping and iterative problem-solving
- Proves viability of controller-free hand tracking for complex interactions
- Establishes patterns for server-authoritative VR multiplayer

### For Recruiters & Portfolio Viewers

This project demonstrates:
- **C++ Proficiency**: Custom component architecture, network replication, real-time systems
- **VR Development**: Hand tracking, spatial computing, multi-user experiences
- **Problem Solving**: Synchronization challenges, performance optimization, UX design
- **Rapid Development**: Complete multiplayer VR system in 24 days
- **Team Collaboration**: Working within VR/AR labs environment

See [SECURITY.md](SECURITY.md) for information about data privacy and security considerations.

## Development Journey

BrickSpace evolved through 6 major development phases over 4 months (June-October 2024), starting with an intensive 24-day sprint. The project progressed from basic VR interaction to a feature-complete multiplayer experience with hand tracking, network replication, and advanced calibration systems.

**Read the complete story**: [Development Journey](docs/DEVELOPMENT_JOURNEY.md) - A detailed chronicle of challenges, breakthroughs, and technical evolution.

### Project Structure

- `Source/BrickSpace/` - Main C++ module
  - `Public/` - Header files
  - `Private/` - Implementation files
- `Content/` - Blueprint assets, materials, models, maps
- `Plugins/` - Custom plugins (Jacobs_VFX, ReplicationTest)
- `Config/` - Engine and project configuration, assembly JSON files

### Key Architecture

See [CLAUDE.md](CLAUDE.md) for detailed architecture documentation including:
- Vodget/Selector interaction system
- Brick snapping mechanics
- Assembly system
- Network replication patterns

### Assembly JSON Files

Assembly instructions are defined in `Config/*.json` files. See `Config/Rocket.json` for an example format.

## Design Philosophy

BrickSpace was built on the principle that the best assistive technologies don't just accommodate differences—they reveal better ways for everyone to interact with digital systems. The clear visual feedback, intuitive interactions, and collaborative problem-solving environment enhance teamwork for all users, regardless of background or ability.

The project demonstrates that when you design for accessibility and human-centered interaction from the ground up, you create experiences that are simply better for everyone. This approach to AR/VR development prioritizes:

- **Natural Interaction**: Hand tracking and spatial interfaces that feel intuitive, not learned
- **Collaborative Design**: Systems that encourage communication and teamwork by default
- **Structured Freedom**: Guided experiences that provide clear goals while allowing creative expression
- **Universal Accessibility**: Interfaces that work for diverse users without special accommodation modes

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on contributing to this project.

## Code of Conduct

This project follows our [Code of Conduct](CODE_OF_CONDUCT.md). Please read it before contributing.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Potential Applications

While BrickSpace was designed as a collaborative building experience, the underlying principles and technical systems have broader applications:

- **Training Simulations**: The multi-user synchronization and spatial alignment systems could enhance team coordination training
- **Educational Environments**: Collaborative problem-solving in safe, structured virtual spaces
- **Therapeutic Applications**: Building on the original assistive technology research for stress management and social skill development
- **Remote Collaboration**: Techniques for synchronizing real-space interactions could improve remote teamwork in AR/VR

## Acknowledgments

Developed as part of Full Sail University's VR/AR Labs program during a 24-day intensive sprint. This project represents collaborative work exploring the intersection of assistive technology, human-centered design, and immersive collaboration systems.

Special thanks to the Full Sail VR/AR Labs team and all contributors who helped shape BrickSpace from concept to working prototype.

### Team Contributors

BrickSpace was developed by a 9-person team during the summer 2024 VR/AR Labs intensive:

- **Jasen Blevins (Repository Owner)** - **Core Systems Developer, Early Development & Planning, Asset Sourcing, Blueprint Architecture (15 commits)**
  - Brick spawning system and multi-user synchronization
  - Early project roadmapping and architecture planning
  - Asset sourcing and Unreal Blueprint setup
  - Colocation support with John Coss Vargas
- **Dan Mapes** - Lead Network Engineer & Calibration Systems (151 commits)
- **Delvin Porterfield** - UI Systems & Interactive Components (76 commits)
- **Ryan Suber** - Hand Tracking & VR Interaction (76 commits)
- **John Coss Vargas** - Colocation & Network Integration (28 commits)
- **AnaDiaz34** - Assembly System & Materials (20 commits)
- **PJ Glover** - Polish & Integration (18 commits)
- **Nevin & Hemant** - Social Interaction Features (27 commits)
- **Bryan Santiago Sinigaglia** - Supporting Development (5 commits)

For detailed contribution breakdown, see [CONTRIBUTORS.md](docs/CONTRIBUTORS.md).
