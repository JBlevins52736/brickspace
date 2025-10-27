# Contributing to BrickSpace

Thank you for your interest in contributing to BrickSpace! This document provides guidelines for contributing to the project.

## Getting Started

1. Fork the repository on GitHub
2. Clone your fork locally
3. Open `BrickSpace.uproject` in Unreal Engine
4. Make sure you can build and run the project before making changes

## Development Setup

### Prerequisites

- Unreal Engine 5.x (matching the engine version in `.uproject`)
- Visual Studio 2022 with C++ development tools
- Meta Quest device (for testing VR features)
- Git for version control

### Building the Project

1. Right-click `BrickSpace.uproject` → Generate Visual Studio project files
2. Open the generated `.sln` file in Visual Studio
3. Build in Development Editor configuration
4. Launch through Unreal Editor

## Making Changes

### Branch Naming

Use descriptive branch names:
- `feature/your-feature-name` for new features
- `fix/bug-description` for bug fixes
- `docs/documentation-update` for documentation changes

### Code Style

- Follow Unreal Engine C++ coding standards
- Use `U` prefix for UObject-derived classes
- Use `A` prefix for AActor-derived classes
- Use `F` prefix for structs and plain C++ classes
- Keep `Public/` headers clean - implementation details in `Private/`
- Use `UPROPERTY()` macros for replicated and Blueprint-exposed properties
- Mark network functions with `Server`, `Client`, or `NetMulticast` prefixes

### Network Replication

All gameplay-modifying actions must go through server RPCs:
1. Add Server RPC to `ABrickSpacePawn` (e.g., `Server_YourAction`)
2. Mark properties for replication with `Replicated` or `ReplicatedUsing`
3. Implement `OnRep_*` functions for replicated properties
4. Test in multiplayer (PIE with multiple clients)

### Vodget/Selector Pattern

When creating new interactive components:
- Inherit from `UVodget` (UI elements) or `UGrabber` (grabbable objects)
- Override gesture methods: `Focus()`, `ForePinch()`, `MiddlePinch()`, `Grip()`
- Set appropriate `selectionFilter` bitmask

## Testing

### Before Submitting

- Test in Unreal Editor (PIE mode)
- Test in VR (deploy to Quest if possible)
- Test multiplayer functionality (PIE with 2+ clients)
- Verify no compiler warnings
- Check that Blueprint dependencies still work

### Testing Checklist

- [ ] Code compiles without errors or warnings
- [ ] Changes work in single player
- [ ] Changes work in multiplayer (if applicable)
- [ ] No Blueprint references are broken
- [ ] Hand tracking interactions work correctly
- [ ] Brick snapping behavior is correct

## Submitting Changes

### Commit Messages

Write clear, descriptive commit messages:

```
Add paintbrush color selection feature

- Implemented color picker Vodget UI
- Added Server_ChangeColor RPC
- Updated material data table with new colors
```

### Pull Requests

1. Push your changes to your fork
2. Create a pull request from your branch to `main`
3. Provide a clear description of:
   - What the PR does
   - Why the change is needed
   - How to test it
   - Any potential issues or limitations

### PR Checklist

- [ ] Code follows Unreal Engine coding standards
- [ ] All tests pass
- [ ] Documentation updated (if needed)
- [ ] CLAUDE.md updated (if architecture changed)
- [ ] No unnecessary files committed (Intermediate/, Saved/, Binaries/)
- [ ] Changes are focused and don't include unrelated modifications

## What to Contribute

### Good First Issues

- Bug fixes
- Documentation improvements
- New assembly JSON files (models to build)
- UI/UX improvements
- Performance optimizations

### Larger Features

For significant features, please open an issue first to discuss:
- New VR interaction methods
- Major architectural changes
- New game modes
- Integration with external services

## Questions?

If you have questions about contributing:
- Open an issue on GitHub
- Review existing issues and pull requests
- Check [CLAUDE.md](CLAUDE.md) for architecture details

## Code of Conduct

This project follows our [Code of Conduct](CODE_OF_CONDUCT.md). Please be respectful and constructive in all interactions.

Thank you for contributing to BrickSpace!
