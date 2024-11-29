import 'package:flutter/material.dart';

/// A widget that rebuilds when a [ChangeNotifier] notifies its listeners.
///
/// This widget simplifies listening to changes in a [ChangeNotifier]
/// and rebuilding a widget tree when those changes occur.
class ChangeNotifierBuilder<T extends ChangeNotifier> extends StatelessWidget {
  const ChangeNotifierBuilder({
    required this.notifier,
    required this.builder,
    super.key,
    this.child,
  });

  /// The [ChangeNotifier] that this widget listens to.
  final T notifier;

  /// The builder function to construct the widget tree.
  ///
  /// - [context]: The build context.
  /// - [notifier]: The current [ChangeNotifier] instance.
  /// - [child]: An optional static child widget.
  final Widget Function(BuildContext context, T notifier, Widget? child)
      builder;

  /// An optional child widget that does not rebuild when the [notifier]
  /// changes.
  ///
  /// Useful for optimizing parts of the UI that remain constant.
  final Widget? child;

  @override
  Widget build(BuildContext context) => ListenableBuilder(
        listenable: notifier,
        builder: (context, widget) => builder(context, notifier, widget),
        child: child,
      );
}
