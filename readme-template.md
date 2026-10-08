# Tooling for Advanced NPC Patterns in AMD Schola, Team 9

Primary point of contact: Alexander Cann, alexander.cann@amd.com

AMD is an American semiconductor company. GPUOpen is an AMD initiative to offer open source advanced effects for computer games.

## Description about the project

An extension to AMD Schola (Unreal Engine's RL library) that makes learned NPC behavior efficient in real games. Schola currently runs inference every frame, one agent at a time, which wastes compute in turn-based games and can't scale to crowds of thousands (e.g., MassEntity simulations). Developers pick an NPC type, attach a trained model, and get efficient behavior without writing training or inference boilerplate.

## Key Features

Delivered as an Unreal Engine plugin with Python-side support:

1. **Adaptive NPCs:** Learn online from player behavior and adjust during play.
2. **Event-Based NPCs:** Run inference only when subscribed events fire (e.g., turn start, player enters zone).
3. **Background NPCs:** Affordable large crowds via batched inference, off-screen culling, and Mass Entity integration.
4. **Shared-Policy NPCs:** A common network base with specialized outputs per role, so related behaviors run faster than separate models.
5. **Advanced Strategy NPCs (stretch goal):** Look ahead by simulating opponent moves, as top strategy-game AIs do.
​
## Instructions
 * Clear instructions for how to use the application from the end-user's perspective
 * How do you access it? For example: Are accounts pre-created or does a user register? Where do you start? etc. 
 * Provide clear steps for using each feature described in the previous section.
 * This section is critical to testing your application and must be done carefully and thoughtfully.
 
 ## Development requirements
 * What are the technical requirements for a developer to set up on their machine or server (e.g. OS, libraries, etc.)?
 * Briefly describe instructions for setting up and running the application. You should address this part like how one would expect a README doc of real-world deployed application would be.
 * You can see this [example](https://github.com/alichtman/shallow-backup#readme) to get started.
 
 ## Deployment and Github Workflow=

**Branching:** `main` is the only long-lived branch. Each change gets a short-lived branch from `main` (in a fork, or a branch in the repo for team members), named `feat/<topic>`, `fix/<topic>`.

**Conflict avoidance:** PRs are small and focused, larger changes are discussed in an issue first, and every PR links its issue (`Fixes #123`). Generated protobuf code is never hand-edited. It is regenerated with `schola compile-proto`.

**Pull requests:** Branch → PR to `main`, using the repo's PR template. The author runs the relevant tests first (pytest for Python, Unreal automation tests for C++). At least one team member reviews and approves, then merges. Commits follow Conventional Commits (`feat(python): add X`) so history is scannable and release notes are easy to build.

**Deployment:** Schola is an Unreal Engine plugin, not a hosted app, so there is no live server. Users copy the plugin into their project's `/Plugins` folder, run `pip install -e Resources/python[all]`, and recompile in Unreal. Tools: Git/GitHub, pytest, Doxygen + Sphinx + Breathe for docs. We will use tagged releases because source-distributed plugins ship best that way.

## Coding Standards and Guidelines

C++ follows the Unreal Engine coding standard (`.clang-format`) with Doxygen comments. Python uses Black, PEP 8 and NumPy-style docstrings. New files carry the AMD copyright header.
​
## Licenses
​
MIT License, just like the original AMD Schola codebase. There is no significant effect on the development and use of this codebase.

## Deployed URL / Access Instructions

Provide a link to the deployed application or clear instructions for how to access it. For mobile apps, include TestFlight/APK links or emulator instructions. For APIs, include Postman collections or curl examples.

## D3 Improvement Highlight

Briefly describe what changed since D2 and how to find it (2-3 sentences). Save the full analysis for the product evolution report.
