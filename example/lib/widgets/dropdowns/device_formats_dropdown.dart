import 'package:equatable/equatable.dart';
import 'package:flutter/material.dart';
import 'package:pro_miniaudio/pro_miniaudio.dart';

class DeviceFormatDropdownVm extends Equatable {
  const DeviceFormatDropdownVm({
    required this.selectedFormat,
    required this.onChanged,
    required this.formats,
  });

  final SupportedFormat? selectedFormat;
  final ValueChanged<SupportedFormat> onChanged;
  final List<SupportedFormat> formats;

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
          DropdownButton<SupportedFormat>(
            isExpanded: true,
            isDense: true,
            value: vm.selectedFormat,
            items: vm.formats
                .map(
                  (value) => DropdownMenuItem<SupportedFormat>(
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

  String _supportedFormatLabel(SupportedFormat device) =>
      '${device.format.name}, '
      '${device.channels} ch${device.channels > 1 ? "'s" : ''}, '
      '${device.sampleRate} Hz';
}
