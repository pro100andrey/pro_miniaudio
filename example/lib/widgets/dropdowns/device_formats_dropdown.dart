import 'package:equatable/equatable.dart';
import 'package:flutter/material.dart';
import 'package:pro_miniaudio/pro_miniaudio.dart';

class DeviceFormatDropdownVm extends Equatable {
  const DeviceFormatDropdownVm({
    required this.selectedFormat,
    required this.onChanged,
    required this.formats,
  });

  final AudioFormat? selectedFormat;
  final ValueChanged<AudioFormat> onChanged;
  final List<AudioFormat> formats;

  @override
  List<Object?> get props => [selectedFormat, formats];
}

class DeviceFormatDropdown extends StatelessWidget {
  const DeviceFormatDropdown({
    required this.label,
    required this.vm,
    super.key,
  });

  final String? label;
  final DeviceFormatDropdownVm vm;

  @override
  Widget build(BuildContext context) => Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          if (label != null)
            Text(
              label!,
              style: Theme.of(context).textTheme.titleMedium!.copyWith(
                    fontWeight: FontWeight.bold,
                    color: Colors.grey.shade700,
                  ),
            ),
          DropdownButton<AudioFormat>(
            isExpanded: true,
            isDense: true,
            value: vm.selectedFormat,
            items: vm.formats
                .map(
                  (value) => DropdownMenuItem<AudioFormat>(
                    value: value,
                    child: Text(
                      _supportedFormatLabel(value),
                      overflow: TextOverflow.ellipsis,
                      style: Theme.of(context).textTheme.bodyMedium,
                    ),
                  ),
                )
                .toList(),
            onChanged: (newValue) => vm.onChanged(newValue!),
          ),
        ],
      );

  String _supportedFormatLabel(AudioFormat device) =>
      '${device.sampleFormat.name}, '
      '${device.channels} ch${device.channels > 1 ? "'s" : ''}, '
      '${device.sampleRate} Hz';
}
