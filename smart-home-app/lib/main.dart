import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_core/firebase_core.dart';
import 'firebase_options.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:firebase_messaging/firebase_messaging.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'dart:async';
import 'package:firebase_storage/firebase_storage.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp(
    options: DefaultFirebaseOptions.currentPlatform,
  );

  // Set up push notification handling
  await FirebaseMessaging.instance.requestPermission();
  FirebaseMessaging.onBackgroundMessage(_firebaseMessagingBackgroundHandler);

  // Configure local notifications
  const AndroidInitializationSettings initializationSettingsAndroid =
      AndroidInitializationSettings('app_icon');
  final InitializationSettings initializationSettings =
      InitializationSettings(android: initializationSettingsAndroid);
  await FlutterLocalNotificationsPlugin().initialize(initializationSettings);

  runApp(const SmartHomeApp());
}

// Handle background messages
Future<void> _firebaseMessagingBackgroundHandler(RemoteMessage message) async {
  print('Handling a background message: ${message.messageId}');
}

class SmartHomeApp extends StatelessWidget {
  const SmartHomeApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        primarySwatch: Colors.blueGrey,
      ),
      home: const LoginScreen(),
    );
  }
}

class LoginScreen extends StatefulWidget {
  const LoginScreen({super.key});

  @override
  _LoginScreenState createState() => _LoginScreenState();
}

class _LoginScreenState extends State<LoginScreen> {
  final TextEditingController _emailController = TextEditingController();
  final TextEditingController _passwordController = TextEditingController();
  final FirebaseAuth _auth = FirebaseAuth.instance;

  Future<void> signInWithEmailPassword() async {
    try {
      UserCredential userCredential = await _auth.signInWithEmailAndPassword(
        email: _emailController.text.trim(),
        password: _passwordController.text.trim(),
      );
      Navigator.pushReplacement(
        context,
        MaterialPageRoute(builder: (context) => const HomeScreen()),
      );
    } on FirebaseAuthException catch (e) {
      String errorMessage = 'An error occurred';
      if (e.code == 'user-not-found') {
        errorMessage = 'No user found for that email.';
      } else if (e.code == 'wrong-password') {
        errorMessage = 'Wrong password provided.';
      }
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text(errorMessage)),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: Padding(
          padding: const EdgeInsets.all(16.0),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              const Text(
                "Smart Home Login",
                style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold),
              ),
              const SizedBox(height: 20),
              TextField(
                controller: _emailController,
                decoration: const InputDecoration(
                  hintText: "Email",
                  border: OutlineInputBorder(),
                ),
              ),
              const SizedBox(height: 10),
              TextField(
                controller: _passwordController,
                obscureText: true,
                decoration: const InputDecoration(
                  hintText: "Password",
                  border: OutlineInputBorder(),
                ),
              ),
              const SizedBox(height: 20),
              ElevatedButton(
                onPressed: signInWithEmailPassword,
                child: const Text("Login"),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class HomeScreen extends StatefulWidget {
  const HomeScreen({super.key});

  @override
  _HomeScreenState createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  final FirebaseDatabase _database = FirebaseDatabase.instance;
  bool doorLockStatus = false;
  bool lightsStatus = false;
  String? capturedImageUrl;
  int lockoutTime = 0; // Stores lockout timer
  Timer? _timer;

  // Fetch captured image URL from Firebase
  void fetchCapturedImage() {
    final ref = _database.ref('captured_image');
    ref.onValue.listen((event) {
      final imageUrl = event.snapshot.value as String?;
      setState(() {
        capturedImageUrl = imageUrl;
      });
    });
  }

  // Fetch lockout timer from Firebase
  void fetchLockoutTimer() {
    final ref = _database.ref('lockout_timer');
    ref.onValue.listen((event) {
      final int? timerValue = event.snapshot.value as int?;
      if (timerValue != null && timerValue > 0) {
        setState(() {
          lockoutTime = timerValue;
        });

        _startCountdown();
      }
    });
  }

  void fetchStoredImages() async {
   List<Map<String, String>> imageList = [];
    final storageRef = FirebaseStorage.instance.ref().child('images/');
  
    try {
      final ListResult result = await storageRef.listAll();
    
      for (var ref in result.items) {
        try {
          final String downloadUrl = await ref.getDownloadURL();
          String filename = ref.name; // Example: "2025-04-03_14-30-45.jpg"

          // Extract timestamp from filename
          String timestamp = filename.replaceAll(".jpg", "").replaceAll("_", " ").replaceAll("-", ":");

          imageList.add({"url": downloadUrl, "timestamp": timestamp});
        } catch (e) {
          print("Error fetching URL for ${ref.name}: $e");
        }
      }

      Navigator.push(
        context,
        MaterialPageRoute(
          builder: (context) => CapturedImagesScreen(imageList: imageList),
        ),
      );
    } catch (e) {
     print("Error fetching stored images: $e");
    }
  }



  // Start countdown for lockout timer
  void _startCountdown() {
    _timer?.cancel();
    _timer = Timer.periodic(const Duration(seconds: 1), (timer) {
      if (lockoutTime > 0) {
        setState(() {
          lockoutTime--;
        });
        _database.ref('lockout_timer').set(lockoutTime);
      } else {
        timer.cancel();
      }
    });
  }

  @override
  void initState() {
    super.initState();
    fetchCapturedImage();
    fetchLockoutTimer();
  }

  @override
  void dispose() {
    _timer?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("Smart Home Dashboard"),
      ),
      body: Column(
        children: [
          if (lockoutTime > 0) ...[
            Padding(
              padding: const EdgeInsets.all(10.0),
              child: Text(
                "Keypad Locked: $lockoutTime seconds remaining",
                style: const TextStyle(fontSize: 18, color: Colors.red, fontWeight: FontWeight.bold),
              ),
            ),
          ],
          Expanded(
            child: GridView.count(
              crossAxisCount: 2,
              padding: const EdgeInsets.all(16),
              children: [
                DeviceCard(
                  deviceName: "Lights",
                  icon: Icons.lightbulb_outline,
                  onTap: () async {
                    setState(() {
                      lightsStatus = !lightsStatus;
                    });
                    await _database.ref('devices/lights').set({'status': lightsStatus ? 'on' : 'off'});
                  },
                ),
                DeviceCard(
                  deviceName: "Door Lock",
                  icon: Icons.lock_outline,
                  onTap: () async {
                    setState(() {
                      doorLockStatus = !doorLockStatus;
                    });
                    await _database.ref('devices/door_lock').set({'status': doorLockStatus ? 'unlocked' : 'locked'});
                  },
                ),
                DeviceCard(
                  deviceName: "Captured Images",
                  icon: Icons.image,
                  onTap: () {
                    print("Fetching stored images...");
                    fetchStoredImages(); // Navigate to Captured Images Screen
                  },
                ),
              ],
            ),
          ),
          if (capturedImageUrl != null) ...[
            const Text("Unauthorized Access Detected!"),
            Image.network(capturedImageUrl!),
          ],
        ],
      ),
    );
  }
}

// New CapturedImagesScreen Widget
class CapturedImagesScreen extends StatelessWidget {
  final List<Map<String, String>> imageList;

  const CapturedImagesScreen({super.key, required this.imageList});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text("Captured Images")),
      body: imageList.isEmpty
          ? const Center(child: Text("No images found"))
          : GridView.builder(
              padding: const EdgeInsets.all(10),
              gridDelegate: const SliverGridDelegateWithFixedCrossAxisCount(
                crossAxisCount: 3, // Adjust for better visibility
                crossAxisSpacing: 20,
                mainAxisSpacing: 20,
                childAspectRatio: 0.8, // Adjust for text below images
              ),
              itemCount: imageList.length,
              itemBuilder: (context, index) {
                return Column(
                  children: [
                    Expanded(
                      child: Image.network(
                        imageList[index]["url"]!,
                        fit: BoxFit.cover,
                        errorBuilder: (context, error, stackTrace) {
                          return const Center(child: Text("Error loading image"));
                        },
                      ),
                    ),
                    const SizedBox(height: 5),
                    Text(
                      imageList[index]["timestamp"]!,
                      style: const TextStyle(fontSize: 14, fontWeight: FontWeight.bold),
                    ),
                  ],
                );
              },
            ),
    );
  }
}

class DeviceCard extends StatelessWidget {
  final String deviceName;
  final IconData icon;
  final VoidCallback onTap;

  const DeviceCard({super.key, required this.deviceName, required this.icon, required this.onTap});

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Card(
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
        elevation: 4,
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Icon(icon, size: 50, color: Colors.blueGrey),
            const SizedBox(height: 10),
            Text(deviceName, style: const TextStyle(fontSize: 18)),
          ],
        ),
      ),
    );
  }
}

