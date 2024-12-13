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

  bool get dataIsRefreshed => _playbackInfos != null;

  Set<DeviceInfo> get playbackInfos => _playbackInfos ?? {};

  Set<DeviceInfo>? _playbackInfos;

  DeviceInfo? _selectedPlaybackDevice;
  DeviceInfo? get selectedPlaybackDevice => _selectedPlaybackDevice;

  bool get useDefaultPlaybackDevice => _useDefaultPlaybackDevice;
  bool _useDefaultPlaybackDevice = false;

  Timer? refreshTimer;

  void refreshDevices() {
    _context.refreshDevices();

    final playback =
        _context.getDeviceInfos(type: AudioDeviceType.playback).toSet();

    final newPlaybackInfos = playback.difference(playbackInfos);
    final removedPlaybackInfos = playbackInfos.difference(playback);

    final hasChanges =
        newPlaybackInfos.isNotEmpty || removedPlaybackInfos.isNotEmpty;

    if (!hasChanges) {
      return;
    }

    if (!removedPlaybackInfos.contains(_selectedPlaybackDevice)) {
      _selectedPlaybackDevice = null;
    }

    _playbackInfos = newPlaybackInfos;

    _selectedPlaybackDevice ??= newPlaybackInfos.isNotEmpty
        ? playback.firstWhere(
            (device) => device.isDefault,
            orElse: () => newPlaybackInfos.first,
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

      final newDevice = PlaybackWaveformDevice(
        context: _context,
        deviceId: _useDefaultPlaybackDevice ? null : selectedPlaybackDevice?.id,
        config: PlaybackConfig.basedChunkDuration(
          format: const AudioFormat(
            channels: 2,
            pcmFormat: PcmFormat.s16,
            sampleRate: 44100,
          ),
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

  @override
  void dispose() {
    _context.dispose();

    super.dispose();
  }

  List<PlaybackWaveformDevice> playbackDevices = [];

  void createPlaybackDevice() {
    const audioFormat = AudioFormat(
      channels: 2,
      pcmFormat: PcmFormat.s16,
      sampleRate: 44100,
    );

    playbackDevices.add(
      PlaybackWaveformDevice(
        context: _context,
        config: PlaybackConfig.basedChunkDuration(
          format: audioFormat,
          chunkMs: 32,
        ),
        deviceId: _useDefaultPlaybackDevice ? null : selectedPlaybackDevice?.id,
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
