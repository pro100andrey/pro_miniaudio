# pro_miniaudio

A Flutter plugin for managing contexts, playback devices, and waveform generation via Dart FFI.

## Features

- Manage audio contexts for device initialization and control.
- Query playback and capture devices.
- Generate and play waveforms (sine, square, sawtooth, triangle).
- Push audio buffers to playback devices for custom playback.

## Getting Started

This plugin uses Dart's Foreign Function Interface (FFI) to communicate with native C libraries. It provides an easy-to-use Dart API for managing audio playback and waveform generation.

### Installation

Add the following to your `pubspec.yaml`:

```yaml
dependencies:
  pro_miniaudio: 0.0.7
```

## Context

The `Context` class manages the initialization and querying of audio devices.

### Example: Initialize and Query Devices

```dart
import 'package:pro_miniaudio/pro_miniaudio.dart';

void main() {
  final context = Context();

  // Refresh devices
  context.refreshDevices();

  final playback = context.playbackDeviceInfos;
  final capture = context.captureDeviceInfos;

  print('Playback devices: $playbackDevices');
  print('Capture devices: $captureDevices');

  // Clean up
  context.dispose();
}
```

### PlaybackDevice

The `PlaybackDevice` class manages audio playback, including starting, stopping, and pushing audio buffers.

#### Example: Create and Play Audio

```dart
import 'dart:typed_data';
import 'package:pro_miniaudio/pro_miniaudio.dart';

void main() {

  final deviceInfo = ...;

  final device = PlaybackDevice(
    deviceInfo: deviceInfo,
    audioFormat: AudioFormat(
        sampleFormat: SampleFormat.f32,
        sampleRate: 8000,
        channels: 2,
      ),
      bufferSizeInBytes: 4096,
  );

  // Generate a sample audio buffer (stereo sine wave)
  final result = waveform.readWaveformPcmFrames(frameCount: 1024)

  // Push the buffer to the device
  device.pushBuffer(buffer: result.frames, framesCount: 44100);

  // Start playback
  device.start();

  // Stop playback after some time (not shown)
  device.stop();

  // Clean up
  device.dispose();
}
```

### Waveform

The `Waveform` class provides an easy way to generate audio waveforms like sine, square, sawtooth, and triangle.

#### Example: Generate a Sine Wave

```dart
import 'dart:typed_data';
import 'package:pro_miniaudio/pro_miniaudio.dart';

void main() {
  final waveform = Waveform(
    config: WaveformSineConfig(
      format: SampleFormat.f32,
      sampleRate: 44100,
      channels: 2,
      amplitude: 0.5,
      frequency: 440.0,
    ),
  );

  // Read PCM frames
  final result = waveform.readWaveformPcmFrames(frameCount: 1024);
  print('Read ${result.framesRead} frames');
  print('Buffer length: ${result.frames.length}');

  // Clean up
  waveform.dispose();
}
```
