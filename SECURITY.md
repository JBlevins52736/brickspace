# Security Policy

## Reporting a Vulnerability

If you discover a security vulnerability in BrickSpace, please report it responsibly:

1. **Do NOT** open a public GitHub issue
2. Email the maintainers directly (contact information available on portfolio site)
3. Include detailed steps to reproduce the vulnerability
4. Allow reasonable time for a fix before public disclosure

## Security Considerations

### Code Security

- **No Credentials in Code**: Never commit API keys, tokens, or credentials
- **Environment Variables**: Use environment variables or secure configuration files for sensitive data
- **Network Security**: All multiplayer communication uses Unreal's built-in encryption

### Build Security

- **Source Builds Only**: This project requires building from source
- **Dependency Verification**: Verify Unreal Engine source from official repositories
- **Plugin Verification**: Only use plugins from trusted sources

### Data Privacy

BrickSpace does not collect or transmit user data beyond what's required for:
- Local multiplayer functionality (IP addresses for session discovery)
- OculusXR colocation (handled by Meta's SDK)

**No Analytics**: This project includes no analytics or telemetry beyond Unreal Engine's default crash reporting (which can be disabled).

## Third-Party Dependencies

This project relies on:
- **Unreal Engine 5.5** (Oculus-VR fork)
- **Meta Quest SDK** (OculusXR plugins)
- **Meta Colocation API**

Security updates for these dependencies should be monitored through:
- [Epic Games Security Updates](https://www.unrealengine.com/en-US/security)
- [Meta Developer Security](https://developers.meta.com/horizon/)

## Deployment Security

When deploying to Quest devices:

1. **Development Mode**: Only enable Developer Mode on devices you control
2. **Sideloading**: Be cautious when sideloading builds from unknown sources
3. **Network Security**: Use trusted networks for multiplayer sessions
4. **Physical Security**: VR sessions require safe physical space

## Portfolio/Public Demonstration

When showcasing this project publicly:

- **Remove Personal Data**: Ensure no personal information in screenshots/videos
- **Sanitize Logs**: Remove any debug logs with sensitive information
- **Network Isolation**: Demo builds should not expose internal network configurations

## Known Limitations

- This is an educational/portfolio project, not production software
- Multiplayer uses local network discovery (not suitable for public internet)
- No authentication/authorization system implemented
- Assumes trusted network environment

## Updates and Patches

This project is maintained as a portfolio piece. Security updates will be applied:
- When critical vulnerabilities are discovered
- When Unreal Engine or Meta SDK updates address security issues
- On a best-effort basis

## Contact

For security concerns or questions, please refer to the contact information on the associated portfolio website.

---

**Last Updated**: October 2025
