import 'package:collection/collection.dart';
import 'package:flutter/material.dart';
import 'package:pro_miniaudio/pro_miniaudio.dart';

import '../controllers/audio_controller.dart';
import '../dialogs/formats_info_dialog.dart';
import '../widgets/actions/device_actions.dart';
import '../widgets/builders/change_notifier_builder.dart';
import '../widgets/dropdowns/device_formats_dropdown.dart';
import '../widgets/dropdowns/devices_dropdown.dart';
import '../widgets/inputs/frequency_input.dart';
import '../widgets/items/playback_device_item.dart';
import '../widgets/items/playback_item.dart';
import '../widgets/selectors/waveform_selector.dart';

class HomePage extends StatefulWidget {
  const HomePage({super.key});

  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  final _audioController = AudioController();

  @override
  void initState() {
    _audioController.refreshDevices();
    super.initState();
  }

  @override
  void dispose() {
    _audioController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) => ChangeNotifierBuilder(
        notifier: _audioController,
        builder: (context, notifier, child) => Scaffold(
          body: CustomScrollView(
            slivers: [
              SliverAppBar(
                floating: true,
                actions: [
                  IconButton(
                    icon: const Icon(Icons.refresh),
                    onPressed: _audioController.refreshDevices,
                  ),
                ],
              ),
              SliverPadding(
                padding: const EdgeInsets.symmetric(horizontal: 16),
                sliver: SliverToBoxAdapter(
                  child: Card(
                    child: Padding(
                      padding: const EdgeInsets.all(16),
                      child: LayoutBuilder(
                        builder: (context, constraints) {
                          const spacing = 16.0;
                          final maxWidth = (constraints.maxWidth - spacing) / 2;

                          return Wrap(
                            spacing: spacing,
                            runSpacing: 16,
                            children: [
                              ConstrainedBox(
                                constraints: BoxConstraints(
                                  minWidth: 200,
                                  maxWidth: maxWidth > 200
                                      ? maxWidth
                                      : double.infinity,
                                ),
                                child: DevicesDropdown(
                                  label: 'Playback',
                                  selectedDevice:
                                      notifier.selectedPlaybackDevice,
                                  onChanged: notifier.selectPlaybackDevice,
                                  devices: notifier.devicesInfos.playback,
                                  onInfoPressed: () async =>
                                      _onPlaybackInfoPressed(
                                    context,
                                    notifier.selectedPlaybackDevice!,
                                  ),
                                ),
                              ),
                              if (notifier.devicesInfos.capture.isNotEmpty)
                                ConstrainedBox(
                                  constraints: BoxConstraints(
                                    minWidth: 200,
                                    maxWidth: maxWidth > 200
                                        ? maxWidth
                                        : double.infinity,
                                  ),
                                  child: DevicesDropdown(
                                    label: 'Capture',
                                    selectedDevice:
                                        notifier.selectedCaptureDevice,
                                    onChanged: notifier.selectCaptureDevice,
                                    devices: notifier.devicesInfos.capture,
                                    onInfoPressed: notifier
                                                .selectedCaptureDevice !=
                                            null
                                        ? () async => _onPlaybackInfoPressed(
                                              context,
                                              notifier.selectedCaptureDevice!,
                                            )
                                        : null,
                                  ),
                                ),
                            ],
                          );
                        },
                      ),
                    ),
                  ),
                ),
              ),
              const SliverToBoxAdapter(
                child: SizedBox(height: 16),
              ),
              SliverPadding(
                padding: const EdgeInsets.symmetric(horizontal: 16),
                sliver: SliverList(
                  delegate: SliverChildListDelegate(
                    [
                      ...notifier.playbackDevices.mapIndexed(
                        (index, device) => PlaybackDeviceItem(
                          name: device.name,
                          onClosePressed: () =>
                              notifier.removePlaybackDevice(index),
                          deviceFormatDropdown: DeviceFormatDropdownVm(
                            selectedFormat: device.supportedFormat,
                            onChanged: (format) =>
                                notifier.setPlaybackDeviceSupportedFormat(
                              index,
                              format,
                            ),
                            formats: device.formats,
                          ),
                          waveformSelector: WaveformSelectorVm(
                            type: device.waveformType,
                            onChanged: (type) => notifier
                                .setPlaybackDeviceWaveformType(index, type),
                          ),
                          frequencyInput: FrequencyInputVm(
                            frequency: device.frequency,
                            onChanged: (value) => notifier
                                .setPlaybackDeviceFrequency(index, value),
                          ),
                          playback: PlaybackItemVm(
                            isPlaying: device.isPlaying,
                            onPlayPressed: () =>
                                notifier.playPlaybackDevice(index),
                            onStopPressed: () =>
                                notifier.stopPlaybackDevice(index),
                          ),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              SliverToBoxAdapter(
                child: Align(
                  alignment: Alignment.centerRight,
                  child: DeviceActions(
                    vm: DeviceActionsVm(
                      createCapture: notifier.selectedCaptureDevice != null
                          ? notifier.createCaptureDevice
                          : null,
                      createPlayback: notifier.selectedPlaybackDevice != null
                          ? notifier.createPlaybackDevice
                          : null,
                    ),
                  ),
                ),
              ),
            ],
          ),
        ),
      );

  Future<void> _onPlaybackInfoPressed(
    BuildContext context,
    DeviceInfo device,
  ) async {
    await showDialog(
      context: context,
      builder: (context) {
        final name = device.name;
        final isDefault = device.isDefault;
        final title = 'Name: $name (${isDefault ? 'Default' : 'Not Default'})';

        return FormatsInfoDialog(
          title: title,
          formats: device.supportedFormats,
        );
      },
    );
  }
}
