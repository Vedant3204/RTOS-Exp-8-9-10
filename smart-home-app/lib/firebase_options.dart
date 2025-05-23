// File generated by FlutterFire CLI.
// ignore_for_file: type=lint
import 'package:firebase_core/firebase_core.dart' show FirebaseOptions;
import 'package:flutter/foundation.dart'
    show defaultTargetPlatform, kIsWeb, TargetPlatform;

/// Default [FirebaseOptions] for use with your Firebase apps.
///
/// Example:
/// ```dart
/// import 'firebase_options.dart';
/// // ...
/// await Firebase.initializeApp(
///   options: DefaultFirebaseOptions.currentPlatform,
/// );
/// ```
class DefaultFirebaseOptions {
  static FirebaseOptions get currentPlatform {
    if (kIsWeb) {
      return web;
    }
    switch (defaultTargetPlatform) {
      case TargetPlatform.android:
        return android;
      case TargetPlatform.iOS:
        return ios;
      case TargetPlatform.macOS:
        return macos;
      case TargetPlatform.windows:
        return windows;
      case TargetPlatform.linux:
        throw UnsupportedError(
          'DefaultFirebaseOptions have not been configured for linux - '
          'you can reconfigure this by running the FlutterFire CLI again.',
        );
      default:
        throw UnsupportedError(
          'DefaultFirebaseOptions are not supported for this platform.',
        );
    }
  }

  static const FirebaseOptions web = FirebaseOptions(
    apiKey: 'AIzaSyDdIJEpTwwxC25ff7TUu6-gWwV4KpMOhEo',
    appId: '1:761819695197:web:204d043552c59f42cee5b4',
    messagingSenderId: '761819695197',
    projectId: 'smarthomesecurity-60a6e',
    authDomain: 'smarthomesecurity-60a6e.firebaseapp.com',
    databaseURL: 'https://smarthomesecurity-60a6e-default-rtdb.firebaseio.com',
    storageBucket: 'smarthomesecurity-60a6e.firebasestorage.app',
    measurementId: 'G-WPF1N5H8ZR',
  );

  static const FirebaseOptions android = FirebaseOptions(
    apiKey: 'AIzaSyDzOeSdCzdS5RW4SoygAkTnHWURMAeFsuU',
    appId: '1:761819695197:android:18d820dce2a03db7cee5b4',
    messagingSenderId: '761819695197',
    projectId: 'smarthomesecurity-60a6e',
    databaseURL: 'https://smarthomesecurity-60a6e-default-rtdb.firebaseio.com',
    storageBucket: 'smarthomesecurity-60a6e.firebasestorage.app',
  );

  static const FirebaseOptions ios = FirebaseOptions(
    apiKey: 'AIzaSyCvM6POC1qUB8PmWNo68WQuyKjCmfLQ2Q0',
    appId: '1:761819695197:ios:bece359a870d214bcee5b4',
    messagingSenderId: '761819695197',
    projectId: 'smarthomesecurity-60a6e',
    databaseURL: 'https://smarthomesecurity-60a6e-default-rtdb.firebaseio.com',
    storageBucket: 'smarthomesecurity-60a6e.firebasestorage.app',
    iosBundleId: 'com.example.smartHomeApp',
  );

  static const FirebaseOptions macos = FirebaseOptions(
    apiKey: 'AIzaSyCvM6POC1qUB8PmWNo68WQuyKjCmfLQ2Q0',
    appId: '1:761819695197:ios:bece359a870d214bcee5b4',
    messagingSenderId: '761819695197',
    projectId: 'smarthomesecurity-60a6e',
    databaseURL: 'https://smarthomesecurity-60a6e-default-rtdb.firebaseio.com',
    storageBucket: 'smarthomesecurity-60a6e.firebasestorage.app',
    iosBundleId: 'com.example.smartHomeApp',
  );

  static const FirebaseOptions windows = FirebaseOptions(
    apiKey: 'AIzaSyDdIJEpTwwxC25ff7TUu6-gWwV4KpMOhEo',
    appId: '1:761819695197:web:204d043552c59f42cee5b4',
    messagingSenderId: '761819695197',
    projectId: 'smarthomesecurity-60a6e',
    authDomain: 'smarthomesecurity-60a6e.firebaseapp.com',
    databaseURL: 'https://smarthomesecurity-60a6e-default-rtdb.firebaseio.com',
    storageBucket: 'smarthomesecurity-60a6e.firebasestorage.app',
    measurementId: 'G-WPF1N5H8ZR',
  );

}