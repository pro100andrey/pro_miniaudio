import 'package:flutter/material.dart';

class StatusBar extends StatelessWidget implements PreferredSizeWidget {
  const StatusBar({
    required this.contextIsInitialized,
    super.key,
  });

  final bool contextIsInitialized;

  @override
  Widget build(BuildContext context) {
    final color = contextIsInitialized ? Colors.green : Colors.red;

    return Padding(
      padding: const EdgeInsets.all(8),
      child: Row(
        children: [
          Icon(
            contextIsInitialized ? Icons.check : Icons.close,
            color: color,
          ),
          const SizedBox(width: 8),
          Flexible(
            child: Text(
              contextIsInitialized
                  ? 'Audio context initialized'
                  : 'Audio context not initialized',
              style: TextStyle(color: color),
              overflow: TextOverflow.ellipsis,
            ),
          ),
        ],
      ),
    );
  }

  @override
  Size get preferredSize => const Size.fromHeight(48);
}
