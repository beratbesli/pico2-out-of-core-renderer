# Security Policy

## Scope

This project is a Raspberry Pi Pico 2 hardware experiment. The supported
security surface is the firmware and host test code on `main` and the latest
tagged release. It has no hosted service or remote API.

## Safety-critical behavior

The demo writes raw sectors on a MicroSD card and can format a reserved region.
Treat a report involving an incorrect LBA, capacity check, confirmation gate,
or data-loss path as a security and safety issue. Use a disposable test card;
never include card images or private data in a report.

## Reporting a vulnerability

Please report issues privately through GitHub's **Report a vulnerability** flow
on this repository. Do not disclose an exploitable raw-sector sequence in a
public issue. If private reporting is unavailable, open a public issue with a
non-sensitive summary and request a private channel.

Include the affected commit, board/SDK version, card type, wiring, and a
minimal reproduction that does not contain sensitive data.

## Response

Reports are reviewed against the current `main` branch and coordinated with
the reporter before disclosure. Hardware behavior can vary by board, SD card,
and wiring, so reproducibility details are especially important.
