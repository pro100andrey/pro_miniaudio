# pro_miniaudio

A Flutter plugin for advanced audio management using Dart FFI.

This plugin provides a comprehensive API for managing audio contexts, querying devices, generating waveforms, and controlling playback. Built on Dart’s Foreign Function Interface (FFI), it bridges the gap between Flutter and native audio libraries.

## Features

- Audio Context Management: Initialize, query, and manage audio devices.
- Playback Control: Start, stop, and push audio buffers to playback devices.
- Waveform Generation: Create and play procedural waveforms (sine, square, triangle, sawtooth).
- Dart FFI Integration: High-performance bindings to native audio libraries.

## Getting Started

This plugin uses Dart's Foreign Function Interface (FFI) to communicate with native C libraries. It provides an easy-to-use Dart API for managing audio playback and waveform generation.

### Installation

Add `pro_miniaudio` to your `pubspec.yaml`:

```yaml
dependencies:
  pro_miniaudio: 0.0.10
```

Then, run:

```bash
flutter pub get
```

## Usage

### Audio Context

The `AudioContext` class is the entry point for managing audio devices. Use it to query available playback and capture devices.

#### Example: Initialize and Query Devices

```dart
import 'package:pro_miniaudio/pro_miniaudio.dart';

void main() {
  final context = AudioContext();

  // Refresh devices
  context.refreshDevices();

  final playbackDevices = context.playbackDeviceInfos;
  final captureDevices = context.captureDeviceInfos;

  print('Playback Devices: $playbackDevices');
  print('Capture Devices: $captureDevices');

  // Dispose of the context when done
  context.dispose();
}
```

### PlaybackDevice

The `PlaybackDevice` class enables audio playback with precise control over buffers and playback state.

#### Example: Play Custom Audio

```dart
import 'dart:typed_data';
import 'package:pro_miniaudio/pro_miniaudio.dart';

void main() {

  final context = AudioContext()..refreshDevices();

  final deviceInfo = context.playbackDeviceInfos.first;
  final audioFormat = deviceInfo.audioFormats.first;

  final chunkDuration = 100; // ms

  final device = PlaybackDevice(
    context: context,
    deviceInfo: deviceInfo,
    config: PlaybackConfig.basedChunkDuration(
      audioFormat: audioFormat,
      chunkMs: chunkDuration,
    ),
  );

  // Generate a waveform (sine wave)
  final waveform = Waveform(
    config: WaveformSineConfig(
      sampleFormat: PcmFormat.f32,
      sampleRate: audioFormat.sampleRate,
      channels: audioFormat.channels,
      amplitude: 0.5,
      frequency: 440.0,
    ),
  );

  final framesCount = (audioFormat.sampleRate * chunkDuration / 1000).toInt();

  // Generate a sample audio buffer (stereo sine wave)
  final result = waveform.readWaveformPcmFrames(frameCount: framesCount)

  // Push the buffer to the device
  device.pushBuffer(buffer: result.frames, framesCount: result.framesRead);

  // Start playback
  device.start();

  // Stop playback after some time (not shown)
  device.stop();

  // Clean up
  device.dispose();
}
```

### Waveform Generation

The `Waveform` class provides an easy way to generate audio waveforms like sine, square, sawtooth, and triangle.

#### Example: Generate and Use a Sine Wave

```dart
import 'package:pro_miniaudio/pro_miniaudio.dart';

void main() {
  final waveform = Waveform(
    config: WaveformSineConfig(
      sampleFormat: PcmFormat.f32,
      sampleRate: 44100,
      channels: 2,
      amplitude: 0.5,
      frequency: 440.0,
    ),
  );

  final result = waveform.readWaveformPcmFrames(frameCount: 1024);

  print('Read ${result.framesRead} frames');
  print('Buffer length: ${result.frames.length}');

  // Dispose of the waveform when done
  waveform.dispose();
}
```

## Contributing

Contributions are welcome! If you encounter any issues or have feature requests, feel free to open an issue or submit a pull request.
