import 'dart:async';

import 'package:flutter/foundation.dart';
import 'package:pro_miniaudio/pro_miniaudio.dart';

import 'playback_waveform_device.dart';

class AudioController extends ChangeNotifier {
  AudioController() {
    refreshDevices();

    refreshTimer = Timer.periodic(const Duration(seconds: 2), (timer) {
      refreshDevices();
    });
  }

  final _context = AudioContext();

  bool get dataIsRefreshed => _deviceInfos != null;

  ({Set<DeviceInfo> capture, Set<DeviceInfo> playback}) get devicesInfos =>
      _deviceInfos ?? (capture: {}, playback: {});

  ({Set<DeviceInfo> capture, Set<DeviceInfo> playback})? _deviceInfos;

  DeviceInfo? _selectedPlaybackDevice;
  DeviceInfo? get selectedPlaybackDevice => _selectedPlaybackDevice;

  DeviceInfo? _selectedCaptureDevice;
  DeviceInfo? get selectedCaptureDevice => _selectedCaptureDevice;

  bool get useDefaultPlaybackDevice => _useDefaultPlaybackDevice;
  bool _useDefaultPlaybackDevice = false;

  Timer? refreshTimer;

  void refreshDevices() {
    _context.refreshDevices();

    final playback = _context.playbackDeviceInfos.toSet();
    final capture = _context.captureDeviceInfos.toSet();

    final newPlaybackDevices = playback.difference(devicesInfos.playback);
    final removedPlaybackDevices = devicesInfos.playback.difference(playback);

    final newCaptureDevices = capture.difference(devicesInfos.capture);
    final removedCaptureDevices = devicesInfos.capture.difference(capture);

    final hasChanges = newPlaybackDevices.isNotEmpty ||
        removedPlaybackDevices.isNotEmpty ||
        newCaptureDevices.isNotEmpty ||
        removedCaptureDevices.isNotEmpty;

    if (!hasChanges) {
      return;
    }

    if (!removedPlaybackDevices.contains(_selectedPlaybackDevice)) {
      _selectedPlaybackDevice = null;
    }

    if (!removedCaptureDevices.contains(_selectedCaptureDevice)) {
      _selectedCaptureDevice = null;
    }

    _deviceInfos = (capture: newCaptureDevices, playback: newPlaybackDevices);

    _selectedPlaybackDevice ??= newPlaybackDevices.isNotEmpty
        ? playback.firstWhere(
            (device) => device.isDefault,
            orElse: () => newPlaybackDevices.first,
          )
        : null;

    _selectedCaptureDevice ??= newCaptureDevices.isNotEmpty
        ? capture.firstWhere(
            (device) => device.isDefault,
            orElse: () => newCaptureDevices.first,
          )
        : null;

    notifyListeners();
  }

  void setUseDefaultPlaybackDevice({required bool value}) {
    _useDefaultPlaybackDevice = value;

    notifyListeners();
  }

  void selectPlaybackDevice(DeviceInfo device) {
    _selectedPlaybackDevice = device;

    notifyListeners();

    final devices = List<PlaybackWaveformDevice>.from(playbackDevices);

    for (var i = 0; i < devices.length; i++) {
      final current = playbackDevices.removeAt(i);
      final currentWaveformType = current.waveformType;
      final currentIsPlaying = current.isPlaying;

      current.dispose();

      final audioFormat = device.audioFormats.first;

      final newDevice = PlaybackWaveformDevice(
        context: _context,
        deviceInfo:_useDefaultPlaybackDevice ? null : _selectedPlaybackDevice!,
        config: PlaybackConfig.basedChunkDuration(
          format: audioFormat,
          chunkMs: 100,
        ),
      );

      playbackDevices.insert(i, newDevice);
      newDevice.setWaveformType(currentWaveformType);

      if (currentIsPlaying) {
        newDevice.play();
      }
    }

    notifyListeners();
  }

  void selectCaptureDevice(DeviceInfo device) {
    _selectedCaptureDevice = device;

    notifyListeners();
  }

  @override
  void dispose() {
    _context.dispose();

    super.dispose();
  }

  List<PlaybackWaveformDevice> playbackDevices = [];

  void createPlaybackDevice() {
    final audioFormats = selectedPlaybackDevice!.audioFormats;
    final audioFormat = audioFormats.first;

    playbackDevices.add(
      PlaybackWaveformDevice(
        context: _context,
        config: PlaybackConfig.basedChunkDuration(
          format: audioFormat,
          chunkMs: 32,
        ), //PlaybackConfig.withAudioFormat(audioFormat),
        deviceInfo:  _useDefaultPlaybackDevice ? null : _selectedPlaybackDevice,
      ),
    );

    notifyListeners();
  }

  void createCaptureDevice() {
    throw UnimplementedError();
  }

  void removePlaybackDevice(int index) {
    playbackDevices.removeAt(index).dispose();
    notifyListeners();
  }

  bool isPlaying(int index) => playbackDevices[index].isPlaying;

  void playPlaybackDevice(int index) {
    playbackDevices[index].play();
    notifyListeners();
  }

  void stopPlaybackDevice(int index) {
    playbackDevices[index].stop();
    notifyListeners();
  }

  void setPlaybackDeviceFrequency(int index, double value) {
    playbackDevices[index].setFrequency(value);
    notifyListeners();
  }

  void setPlaybackDeviceAmplitude(int index, double value) {
    playbackDevices[index].setAmplitude(value);
    notifyListeners();
  }

  void setPlaybackDeviceWaveformType(int index, WaveformType type) {
    playbackDevices[index].setWaveformType(type);
    notifyListeners();
  }
}
