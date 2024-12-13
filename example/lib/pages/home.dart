import 'package:collection/collection.dart';
import 'package:flutter/material.dart';
import 'package:pro_miniaudio/pro_miniaudio.dart';

import '../controllers/audio_controller.dart';
import '../dialogs/formats_info_dialog.dart';
import '../widgets/actions/device_actions.dart';
import '../widgets/builders/change_notifier_builder.dart';
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
              const SliverAppBar(floating: true),
              SliverPadding(
                padding: const EdgeInsets.symmetric(horizontal: 16),
                sliver: SliverToBoxAdapter(
                  child: Card(
                    child: Padding(
                      padding: const EdgeInsets.all(16),
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.end,
                        children: [
                          DevicesDropdown(
                            label: 'Playback',
                            selectedDevice: notifier.selectedPlaybackDevice,
                            onChanged: notifier.selectPlaybackDevice,
                            devices: notifier.playbackInfos.toList(),
                            onInfoPressed: () async => _onPlaybackInfoPressed(
                              context,
                              notifier.selectedPlaybackDevice!,
                            ),
                          ),
                          SizedBox(
                            width: 200,
                            child: CheckboxListTile(
                              title: const Text('Use default'),
                              value: notifier.useDefaultPlaybackDevice,
                              onChanged: (value) =>
                                  notifier.setUseDefaultPlaybackDevice(
                                value: value ?? false,
                              ),
                            ),
                          ),
                        ],
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
                          name: 'Playback ${index + 1}  '
                              '- ${device.name ?? 'Default'}',
                          onClosePressed: () =>
                              notifier.removePlaybackDevice(index),
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
                      createCapture: null,
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
          audioFormats: const [],
        );
      },
    );
  }
}
