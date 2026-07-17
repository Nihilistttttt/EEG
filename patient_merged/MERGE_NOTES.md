# Patient merged project

## Merge basis

- Main project: newer `patient` project.
- SSVEP source: legacy mode-1 patient project.
- The newer application entry, four-page `ViewPager2` framework, automatic connection, MI and inference functions are retained.

## Integrated SSVEP functions

- Full-screen single-target SSVEP stimulus for 11/13/15/17 Hz.
- Display-vsync timing through `Choreographer`, not millisecond `Handler` blinking.
- Doctor start/stop command response.
- First-stimulus-frame acknowledgement with display refresh rate.
- Doctor FBCCA result display and status replay after reconnect.

## Communication

- 41004: doctor-to-patient data channel only.
- 41005: patient-to-doctor command channel.
- 41006: bidirectional page/SSVEP control channel.
- 41007: UDP discovery.
- 41006 uses `PATIENT_READY` / `CONTROL_READY`, PING/PONG heartbeat, permanent reconnect supervision, generation-based stale-socket protection and state replay.

Doctor-to-patient page mapping:

- Doctor page 4 -> patient page 1 (SSVEP).
- Doctor page 5 -> patient page 2 (MI).
- Doctor page 6 -> patient page 3 (inference).
- Other doctor pages -> patient page 0 (connection/status).

## MI / SSVEP mutual exclusion

`TrainingModeCoordinator` prevents both paradigms from retaining active resources together:

- Leaving SSVEP stops the `Choreographer` callback and hides the stimulus.
- Leaving MI or losing the complete connection clears MI timer callbacks and trial state.
- Starting either mode first stops the other mode controller.

## Validation completed

- Java syntax parsing for all source files.
- XML well-formedness and resource-reference checks.
- Duplicate class and duplicate layout-ID checks.
- Patient-side 41006 integration test: discovery, dual handshake, heartbeat, page mapping, SSVEP start/stop/result, control-only reconnect and state replay.

`local.properties` retains the original SDK path and may need to be updated by Android Studio on another computer.
