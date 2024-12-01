import 'package:flutter/material.dart';
import 'package:pro_miniaudio/pro_miniaudio.dart';

class DevicesDropdown extends StatelessWidget {
  const DevicesDropdown({
    required this.label,
    required this.selectedDevice,
    required this.onChanged,
    required this.devices,
    required this.onInfoPressed,
    super.key,
  });

  final String label;
  final DeviceInfo? selectedDevice;
  final ValueChanged<DeviceInfo> onChanged;
  final List<DeviceInfo> devices;
  final VoidCallback? onInfoPressed;

  @override
  Widget build(BuildContext context) => Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Row(
            children: [
              Text(
                label,
                overflow: TextOverflow.ellipsis,
                style: Theme.of(context).textTheme.titleSmall!.copyWith(
                      fontWeight: FontWeight.bold,
                      color: Colors.grey.shade700,
                    ),
              ),
              const Spacer(),
              if (onInfoPressed != null)
                IconButton(
                  iconSize: 16,
                  padding: const EdgeInsets.all(4),
                  constraints: const BoxConstraints(),
                  icon: const Icon(Icons.info_outline),
                  onPressed: onInfoPressed,
                ),
            ],
          ),
          DropdownButton<DeviceInfo>(
            isExpanded: true,
            isDense: true,
            value: selectedDevice,
            style: Theme.of(context).textTheme.bodyMedium!.copyWith(
                  overflow: TextOverflow.ellipsis,
                ),
            items: devices
                .map(
                  (value) => DropdownMenuItem<DeviceInfo>(
                    value: value,
                    child: Text(value.name),
                  ),
                )
                .toList(),
            onChanged: (newValue) => onChanged(newValue!),
          ),
        ],
      );
}
