# BrickSpace - Portfolio Showcase

## Project Overview

**BrickSpace** is a collaborative AR/VR building environment for Meta Quest that enables up to 4 players to construct virtual LEGO-style structures together in shared physical space.

- **Role**: Core Systems Developer
- **Duration**: 4 months (June - October 2024)
- **Initial Sprint**: 24-day intensive period
- **Technologies**: Unreal Engine 5.5 (C++), Meta Quest SDK, OculusXR, Niagara VFX
- **Context**: Full Sail University VR/AR Labs Project
- **Scale**: 411 commits, 41 C++ classes, ~10,000+ lines of code

## The Challenge

Create a multiplayer VR experience where:
- Multiple users share the same physical space
- Brick movements synchronize in real-time across all clients
- Hand tracking provides natural, intuitive interaction
- Spatial alignment ensures consistent shared experience
- Network architecture maintains authoritative state

**Timeline Constraint**: Complete system from prototype to working demo in 24 days.

## My Role & Contributions

### Primary Responsibilities

**Early Development & Planning:**
- Assisted with early project development and technical roadmapping
- Contributed to initial architecture decisions and system design framework
- Helped establish development workflow for the 24-day sprint
- Participated in team planning and feature prioritization

**Asset Sourcing & Blueprint Architecture:**
- **Asset Integration**: Sourced and integrated 3D assets from marketplace and community resources
- **Blueprint Setup**: Established Unreal Blueprint architecture for rapid prototyping and team collaboration
- **Asset Pipeline**: Managed asset pipeline and integration with C++ systems
- **Material Configuration**: Set up material instances and data tables for visual customization

**Core Systems Development:**
- **Brick Spawning System**: Designed and implemented the core brick instantiation and management system
- **Multi-User Synchronization**: Developed real-time replication for brick movements across up to 4 concurrent users
- **Play Space Alignment**: Collaborated with John Coss Vargas on spatial calibration and colocation support
- **Performance Optimization**: Optimized spawning and network systems to maintain 72fps on Quest hardware

**Colocation Integration:**
- Assisted with OculusXR colocation system implementation
- Contributed to spatial anchor testing and debugging workflows
- Supported multi-user spatial alignment calibration
- Helped integrate colocation features with brick spawning system

### Technical Implementation

**Brick Spawning Architecture**
- Created server-authoritative spawning system using Unreal's replication framework
- Implemented object pooling for performance
- Developed custom RPC patterns for reliable state synchronization
- Built validation system ensuring consistent state across all clients

**Spatial Synchronization**
- Integrated OculusXR colocation APIs for multi-user spatial alignment
- Developed calibration workflow using spatial anchors
- Implemented coordinate system transformation for consistent world space
- Created real-time synchronization of brick positions and rotations

**Network Architecture**
- Designed client-server model with authoritative host
- Implemented custom replication for brick properties (position, rotation, material)
- Built OnRep_ callback system for smooth client-side updates
- Optimized network traffic for 4-player scenarios

## Technical Challenges Solved

### Challenge 1: Real-Space Synchronization
**Problem**: Four players in the same room need to see virtual objects in the exact same physical locations.

**Solution**:
- Implemented OculusXR colocation session system
- Created spatial anchor sharing mechanism
- Developed coordinate transformation pipeline
- Built calibration UI for spatial alignment verification

### Challenge 2: Network Performance
**Problem**: Real-time brick manipulation with 4 clients threatened to overwhelm network bandwidth.

**Solution**:
- Implemented delta compression for transform updates
- Created prediction system for smooth local movement
- Optimized RPC calls to batch related updates
- Used relevancy filtering for distant objects

### Challenge 3: Hand Tracking Integration
**Problem**: Natural brick manipulation required precise hand tracking with low latency.

**Solution**:
- Developed custom Vodget/Selector interaction framework
- Created grab and pinch gesture recognition
- Implemented visual feedback for grab states
- Built snap-to-grid system for satisfying placement mechanics

## Key Achievements

### Technical
- ✅ Contributed to project roadmap and early architecture planning
- ✅ Zero-to-working-prototype in 24 days (team achievement)
- ✅ Stable 4-player synchronization at 72fps on Quest 2
- ✅ Sub-50ms latency for brick movement replication
- ✅ Robust spatial calibration system with user-friendly workflow (with John Coss Vargas)
- ✅ Successful OculusXR colocation integration for shared physical space

### Design
- ✅ Intuitive hand tracking interactions requiring no tutorial
- ✅ Clear visual feedback for all interaction states
- ✅ Accessible design accommodating diverse user needs
- ✅ Structured building experience encouraging collaboration

### Leadership & Collaboration
- ✅ Contributed to early development planning and sprint organization
- ✅ Collaborated effectively across multiple systems (spawning, networking, colocation)
- ✅ Provided technical support for colocation system integration
- ✅ Helped establish development workflows for rapid prototyping

### Learning
- ✅ Deep proficiency in Unreal Engine's C++ networking API
- ✅ Practical experience with Meta Quest SDK and OculusXR colocation
- ✅ Real-world multiplayer VR development from planning to implementation
- ✅ Rapid prototyping and iteration under tight deadlines
- ✅ Cross-functional collaboration in fast-paced team environment

## Technologies Used

### Core Stack
- **Unreal Engine 5.x** (C++) - Primary development platform
- **Meta Quest SDK** - VR platform integration
- **OculusXR Colocation** - Multi-user spatial alignment
- **Visual Studio 2022** - Development environment

### Key Systems
- Unreal Networking (Replication, RPCs)
- Hand Tracking APIs
- Spatial Anchors
- Niagara Particle System
- UMG UI Framework
- JSON Serialization

### Development Tools
- Git for version control
- GitHub for collaboration
- Quest Developer Hub for deployment
- Performance profiler for optimization

## Project Impact

### Original Mission
BrickSpace evolved from research into assistive technology for individuals with autism, focusing on developing coping skills and communication in safe, structured environments.

### Broader Applications
The synchronization and interaction systems developed have potential applications in:
- Team training simulations
- Educational collaborative environments
- Therapeutic VR applications
- Remote collaboration tools

### Personal Growth
This project accelerated my development from Unreal Engine novice to contributing team member, demonstrating:
- Rapid skill acquisition under pressure
- Complex problem-solving in novel domains
- Effective collaboration in cross-functional team environment
- Leadership in early planning and architecture discussions
- Technical support across multiple systems (spawning, networking, colocation)
- Commitment to human-centered design principles
- Ability to contribute meaningfully to both planning and implementation phases

## Links & Resources

- **[GitHub Repository](https://github.com/JBlevins52736/brickspace)**
- **[Technical Documentation](../README.md)**
- **[Video Demonstrations](../media/EXTERNAL_MEDIA.md)** *(Links to be added)*
- **[Architecture Details](../CLAUDE.md)**

## Testimonials & Recognition

*[Add any team feedback, instructor comments, or recognition here]*

---

**For more information** or to discuss this project in detail, please contact me through my portfolio website.

*This project demonstrates capabilities in C++ development, VR/AR systems, network programming, real-time synchronization, and rapid prototyping—skills directly applicable to immersive computing research and development roles.*
